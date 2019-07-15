/*-----------------------------

MIT License

Copyright (c) 2019 Zhihan Gao

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-------------------------------

 [resampler.c]
 - Alexander Brandt 2019

 Alex here: because sReSample() is the hearth of this
 tiny program, the copyright goes to Zhihan Gao.

 https://github.com/cpuimage/resampler
 A Simple and Efficient Audio Resampler Implementation in C

 $ clang -I./include sketches/resampler.c ./libjapan-dbg.a -g -O0 -o ./resampler

-----------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "sound.h"

#define RE_FREQUENCY 11025
#define RE_FORMAT SOUND_I8
#define RE_CHANNELS 1
#define RE_BUFFER_LEN 1024 // Somewhat in (channels * frames)
                           // as it depends on sound format


// Next mapping only valid for Wav, Mp3 and Flac
// (but, Wav have an extension to personalize the mapping)
#define CH_MONO 0
#define CH_LEFT 0
#define CH_RIGHT 1
#define CH_CENTER 2
#define CH_SUB 3
#define CH_REAR_LEFT 4
#define CH_REAR_RIGHT 5
#define CH_REAR_ALT_LEFT 6
#define CH_REAR_ALT_RIGHT 7


struct Resampled
{
	size_t size; // In bytes
	void* data;
};


/*-----------------------------

 sReSample()
-----------------------------*/
static void sReSample(const float* in, size_t in_size, size_t channels, int in_frequency, float* out, int out_frequency)
{
	size_t out_len = (in_size / sizeof(float) / channels) * out_frequency / in_frequency;

	if(in_frequency == out_frequency)
		return;

	double step_dist = ((double)in_frequency / (double)out_frequency);
	const size_t fixed_fraction = (1LL << 32);
	const double norm_fixed = (1.0 / (1LL << 32));
	size_t step = ((size_t)(step_dist * fixed_fraction + 0.5));
	size_t cur_offset = 0;

	for (size_t i = 0; i < out_len; i += 1)
	{
		for (size_t c = 0; c < channels; c += 1)
		{
			*out++ = (float)(in[c] + (in[c + channels] - in[c]) * ((double)(cur_offset >> 32) +
			                                                      ((cur_offset & (fixed_fraction - 1)) * norm_fixed)));
		}

		cur_offset += step;
		in += (cur_offset >> 32) * channels;
		cur_offset &= (fixed_fraction - 1);
	}
}


/*-----------------------------

 sCommonToOutput()
-----------------------------*/
static void sCommonToOutput(float* in, size_t in_size, size_t channels, void* out, enum SoundFormat out_format)
{
	union {
		void* raw;
		int8_t* i8;
		int16_t* i16;
		int32_t* i32;
		float* f32;
		double* f64;
	} dest;

	dest.raw = out;

	if (out_format == SOUND_F32)
	{
		memcpy(out, in, in_size);
		return;
	}

	// Cycle in sample steps (not frames!)
	for (size_t i = 0; i < (in_size / sizeof(float) / channels); i++)
	{
		switch (out_format)
		{
		case SOUND_I8:
			*dest.i8 = (int8_t)((float)INT8_MAX * in[i]);
			break;
		case SOUND_I16:
			*dest.i16 = (int16_t)((float)INT16_MAX * in[i]);
			break;
		case SOUND_I32:
			*dest.i32 = (int32_t)((float)INT32_MAX * in[i]);
			break;
		case SOUND_F64:
			*dest.f32 = (float)in[i];
			break;
		default:
			break;
		}

		dest.i8 += SoundBps(out_format);
	}
}


/*-----------------------------

 sInputToCommon()
-----------------------------*/
static void sInputToCommon(void* in, size_t in_size, size_t in_channels, enum SoundFormat in_format,
                           float* out, size_t out_channels)
{
	union {
		void* raw;
		int8_t* i8;
		int16_t* i16;
		int32_t* i32;
		float* f32;
		double* f64;
	} org;

	org.raw = in;

	float mix[16];

	// Cycle in frame steps
	for (size_t bytes_read = 0; bytes_read < in_size; bytes_read += (SoundBps(in_format) * in_channels))
	{
		// Re format (now cycling though samples)
		for (size_t c = 0; c < in_channels; c++)
		{
			switch (in_format)
			{
			case SOUND_I8:
				mix[c] = ((float)org.i8[c]) / (float)INT8_MAX;
				break;
			case SOUND_I16:
				mix[c] = ((float)org.i16[c]) / (float)INT16_MAX;
				break;
			case SOUND_I32:
				mix[c] = ((float)org.i32[c]) / (float)INT32_MAX;
				break;
			case SOUND_F32:
				mix[c] = org.f32[c];
				break;
			case SOUND_F64:
				mix[c] = (float)org.f64[c];
			}
		}

		// Re mix
		if (out_channels == 1) // Mono
		{
			out[CH_MONO] = mix[CH_MONO];

			for (size_t c = 1; c < in_channels; c++)
			{
				out[CH_MONO] += mix[c];
				out[CH_MONO] /= 2.f;
			}
		}
		else
		{
			fprintf(stderr, "Unsupported!\n");
			exit(EXIT_FAILURE);
		}

		// Next
		org.i8 += (SoundBps(in_format) * in_channels);
		out += out_channels;
	}
}


/*-----------------------------

 sProccess()
-----------------------------*/
static struct Resampled sProccess(const char* filename, struct Buffer* single_buffer, struct Status* st)
{
	struct SoundEx ex = {0};
	FILE* file = NULL;

	StatusSet(st, "Proccess", STATUS_SUCCESS, NULL);

	// Open file and read head
	if ((file = fopen(filename, "r")) == NULL)
	{
		StatusSet(st, "Proccess", STATUS_FS_ERROR, NULL);
		goto return_failure;
	}

	if (SoundExLoad(file, &ex, st) != 0)
		goto return_failure;

	if (fseek(file, ex.data_offset, SEEK_SET) != 0)
	{
		StatusSet(st, "Proccess", STATUS_UNEXPECTED_EOF, "at data seek ('%s')", filename);
		goto return_failure;
	}

	// Resize buffer according the follow memory requirements
	size_t out_size = SoundBps(RE_FORMAT) * RE_CHANNELS * (ex.length * RE_FREQUENCY / ex.frequency);

	size_t in_buffer_size = ex.minimum_unit_size * ex.channels * RE_BUFFER_LEN;
	size_t fmt1_buffer_size = sizeof(float) * RE_CHANNELS * RE_BUFFER_LEN;
	size_t fmt2_buffer_size = sizeof(float) * RE_CHANNELS * (RE_BUFFER_LEN * RE_FREQUENCY / ex.frequency);

	if (BufferResize(single_buffer, out_size + in_buffer_size + fmt1_buffer_size + fmt2_buffer_size) == NULL)
	{
		StatusSet(st, "Proccess", STATUS_MEMORY_ERROR, NULL);
		goto return_failure;
	}

	void* out = single_buffer->data;
	void* in_buffer = ((uint8_t*)single_buffer->data) + out_size;
	float* fmt1_buffer = (float*)(((uint8_t*)single_buffer->data) + out_size + in_buffer_size);
	float* fmt2_buffer = (float*)(((uint8_t*)single_buffer->data) + out_size + in_buffer_size + fmt1_buffer_size);

	printf("'%s':\n", filename);
	printf("- Input buffer size: %zu bytes\n", in_buffer_size);
	printf("- Format 1 buffer size: %zu bytes\n", fmt1_buffer_size);
	printf("- Format 2 buffer size: %zu bytes\n", fmt2_buffer_size);
	printf("- Output data size: %zu bytes\n", out_size);

	// Read data in (in_buffer_size) steps
	uint8_t* out_cursor = out;

	for (size_t bytes_read = 0; bytes_read < in_buffer_size * (ex.uncompressed_size / in_buffer_size);
		 bytes_read += in_buffer_size)
	{
		// Fill 'in_buffer' with file data
		SoundExRead(file, ex, in_buffer_size, in_buffer, st);

		if (st->code != STATUS_SUCCESS)
			goto return_failure;

		// Convert file data to a common format (float),
		// reads from 'in_buffer' and outputs to 'fmt1_buffer'
		sInputToCommon(in_buffer, in_buffer_size, ex.channels, ex.format, fmt1_buffer, RE_CHANNELS);

		// Where the magic happens,
		// reads from 'fmt1_buffer' and outputs to 'fmt2_buffer'
		sReSample(fmt1_buffer, fmt1_buffer_size, RE_CHANNELS, ex.frequency, fmt2_buffer, RE_FREQUENCY);

		// Finally we convert from the common format to the desired
		// output format. Writting to the final destination 'out_cursor'
		sCommonToOutput(fmt2_buffer, fmt2_buffer_size, RE_CHANNELS, out_cursor, RE_FORMAT);

		out_cursor += fmt2_buffer_size / sizeof(float);
	}

	// Read data tail
	size_t tail_size = ex.uncompressed_size - in_buffer_size * (ex.uncompressed_size / in_buffer_size);
	size_t tail_len = tail_size / SoundBps(ex.format) / ex.channels; // In frames

	SoundExRead(file, ex, tail_size, in_buffer, st);
	if (st->code != STATUS_SUCCESS)
		goto return_failure;

	sInputToCommon(in_buffer, tail_size, ex.channels, ex.format, fmt1_buffer, RE_CHANNELS);
	sReSample(fmt1_buffer, sizeof(float) * tail_len, RE_CHANNELS, ex.frequency, fmt2_buffer, RE_FREQUENCY);
	sCommonToOutput(fmt2_buffer, sizeof(float) * (tail_len * RE_FREQUENCY / ex.frequency), RE_CHANNELS, out_cursor,
					RE_FORMAT);

	// Bye!
	fclose(file);
	return (struct Resampled){.size = out_size, .data = single_buffer->data};

return_failure:
	if (file != NULL)
		fclose(file);

	return (struct Resampled){.size = 0, .data = NULL};
}


/*-----------------------------

 main()
-----------------------------*/
int main(int argc, char* argv[])
{
	struct Buffer buffer = {0};
	struct Resampled re = {0};
	struct Status st = {0};
	struct Sound snd = {0};

	if (argc == 1)
	{
		StatusSet(&st, "main", STATUS_INVALID_ARGUMENT, "no in specified");
		goto return_failure;
	}

	re = sProccess(argv[1], &buffer, &st);

	if (st.code == STATUS_SUCCESS)
	{
		snd.format = RE_FORMAT;
		snd.frequency = RE_FREQUENCY;
		snd.channels = RE_CHANNELS;
		snd.size = re.size;
		snd.length = re.size / SoundBps(RE_FORMAT) / RE_CHANNELS;
		snd.data = re.data;

		SoundSaveAu(&snd, "/tmp/resampled.au");
		SoundSaveWav(&snd, "/tmp/resampled.wav");
		SoundSaveRaw(&snd, "/tmp/resampled.data");
	}
	else
		goto return_failure;

	// Bye!
	BufferClean(&buffer);
	return EXIT_SUCCESS;

return_failure:
	BufferClean(&buffer);
	StatusPrint("Resampler sketch", st);
	return EXIT_FAILURE;
}

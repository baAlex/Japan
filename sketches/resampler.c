
// $ clang -I./include sketches/resampler.c ./japan-dbg.a -g -O0 -o ./resampler
#include <stdio.h>
#include <stdlib.h>

#include "sound.h"

#define RE_FREQUENCY 8000
#define RE_FORMAT SOUND_I8
#define RE_CHANNELS 1
#define RE_BUFFER_LEN 1024 // In (channels * frames)

#define CH_MONO 0
#define CH_LEFT 0
#define CH_RIGHT 1


struct Resampled
{
	size_t length; // In frames
	void* data;
};


/*-----------------------------

 sStore()
-----------------------------*/
static void sStore(float* in, size_t in_size, size_t channels, size_t in_frequency, void* out, enum SoundFormat out_format)
{
	size_t out_bps = SoundBps(out_format);

	union {
		void* raw;
		int8_t* i8;
		int16_t* i16;
		int32_t* i32;
		float* f32;
		double* f64;
	} dest;

	dest.raw = out;

	size_t in_len = (in_size / sizeof(float) / channels);

	for (size_t i = 0; i < in_len; i++)
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
		case SOUND_F32:
			*dest.f32 = in[i];
			break;
		case SOUND_F64:
			*dest.f32 = (float)in[i];
		}

		dest.i8 += (out_bps);
	}
}


/*-----------------------------

 sReSample()
-----------------------------*/
static size_t sReSample(const float* in, size_t in_size, size_t channels, int in_frequency, float* out, int out_frequency)
{
	size_t out_len = (in_size / sizeof(float) / channels) * out_frequency / in_frequency;

	if (out == NULL)
		return out_len;

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

	return out_len;
}


/*-----------------------------

 sReFormat()
-----------------------------*/
static void sReFormat(void* in, size_t in_size, size_t in_channels, enum SoundFormat in_format, float* out,
			   size_t out_channels)
{
	size_t in_bps = SoundBps(in_format);

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

	for (size_t bytes_read = 0; bytes_read < in_size; bytes_read += (in_bps * in_channels))
	{
		// Re format
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

		// Next frame
		org.i8 += (in_bps * in_channels);
		out += out_channels;
	}
}


/*-----------------------------

 sProccess()
-----------------------------*/
static struct Resampled* sProccess(const char* filename, struct Status* st)
{
	struct SoundEx ex = {0};
	FILE* file = NULL;

	void* raw_buffer = NULL;
	float* fmt1_buffer = NULL;
	float* fmt2_buffer = NULL;

	size_t raw_buffer_size = 0;
	size_t fmt1_buffer_size = 0;
	size_t fmt2_buffer_size = 0;

	struct Resampled* resampled = NULL;
	size_t resampled_data_size = 0;


	StatusSet(st, "Proccess", STATUS_SUCCESS, NULL);

	if ((file = fopen(filename, "r")) == NULL)
	{
		StatusSet(st, "Proccess", STATUS_FS_ERROR, NULL);
		return NULL;
	}

	if (SoundExLoad(file, &ex, st) != 0)
		goto return_failure;

	if (fseek(file, ex.data_offset, SEEK_SET) != 0)
	{
		StatusSet(st, "Proccess", STATUS_UNEXPECTED_EOF, "at data seek ('%s')", filename);
		goto return_failure;
	}

	// Allocate everything
	raw_buffer_size = ex.minimum_unit_size * ex.channels * RE_BUFFER_LEN;
	fmt1_buffer_size = sizeof(float) * RE_CHANNELS * RE_BUFFER_LEN;
	fmt2_buffer_size = sizeof(float) * RE_CHANNELS * (RE_BUFFER_LEN * RE_FREQUENCY / ex.frequency);
	resampled_data_size = SoundBps(RE_FORMAT) * RE_CHANNELS * (ex.length * RE_FREQUENCY / ex.frequency);

	if ((raw_buffer = malloc(raw_buffer_size)) == NULL || (fmt1_buffer = malloc(fmt1_buffer_size)) == NULL ||
		(fmt2_buffer = malloc(fmt2_buffer_size)) == NULL ||
		(resampled = malloc(sizeof(struct Resampled) + resampled_data_size)) == NULL)
	{
		StatusSet(st, "Proccess", STATUS_MEMORY_ERROR, NULL);
		goto return_failure;
	}

	resampled->length = ex.length * RE_FREQUENCY / ex.frequency;
	resampled->data = ((struct Resampled*)resampled + 1);

	printf("Raw buffer size: %zu bytes\n", raw_buffer_size);
	printf("Format 1 buffer size: %zu bytes\n", fmt1_buffer_size);
	printf("Format 2 buffer size: %zu bytes\n", fmt2_buffer_size);

	// Read data in (raw_buffer_size) steps
	uint8_t* final_dest = resampled->data;

	for (size_t bytes_read = 0; bytes_read < raw_buffer_size * (ex.uncompressed_size / raw_buffer_size);
		 bytes_read += raw_buffer_size)
	{
		SoundExRead(file, ex, raw_buffer_size, raw_buffer, st);
		sReFormat(raw_buffer, raw_buffer_size, ex.channels, ex.format, fmt1_buffer, RE_CHANNELS);
		sReSample(fmt1_buffer, fmt1_buffer_size, RE_CHANNELS, ex.frequency, fmt2_buffer, RE_FREQUENCY);
		sStore(fmt2_buffer, fmt2_buffer_size, RE_CHANNELS, RE_FREQUENCY, final_dest, RE_FORMAT);

		final_dest += fmt2_buffer_size/sizeof(float);

		if (st->code != STATUS_SUCCESS)
			goto return_failure;
	}

	// Read data tail
	size_t tail_size = ex.uncompressed_size - raw_buffer_size * (ex.uncompressed_size / raw_buffer_size);

	SoundExRead(file, ex, tail_size, raw_buffer, st);
	sReFormat(raw_buffer, tail_size, ex.channels, ex.format, fmt1_buffer, RE_CHANNELS);
	sReSample(fmt1_buffer, fmt1_buffer_size, RE_CHANNELS, ex.frequency, fmt2_buffer,
			  RE_FREQUENCY); // TODO: It should not be 'fmt1_buffer_size'
	// sStore(fmt2_buffer, fmt2_buffer_size, RE_CHANNELS, final_dest, RE_FORMAT);

	if (st->code != STATUS_SUCCESS)
		goto return_failure;

	// Bye!
	fclose(file);
	free(raw_buffer);
	free(fmt1_buffer);
	free(fmt2_buffer);
	return resampled;

return_failure:
	fclose(file);
	if (raw_buffer != NULL)
		free(raw_buffer);
	if (fmt1_buffer != NULL)
		free(fmt1_buffer);
	if (fmt2_buffer != NULL)
		free(fmt2_buffer);
	if (resampled != NULL)
		free(resampled);
	return NULL;
}


/*-----------------------------

 main()
-----------------------------*/
int main(int argc, char* argv[])
{
	struct Resampled* re = NULL;
	struct Status st = {0};

	if (argc == 1)
	{
		StatusSet(&st, "main", STATUS_INVALID_ARGUMENT, "no in specified");
		goto return_failure;
	}

	if ((re = sProccess(argv[1], &st)) != NULL)
	{
		struct Sound snd = {0};

		snd.format = RE_FORMAT;
		snd.frequency = RE_FREQUENCY;
		snd.channels = RE_CHANNELS;
		snd.length = re->length;
		snd.size = re->length * SoundBps(RE_FORMAT) * RE_CHANNELS;
		snd.data = re->data;

		SoundSaveAu(&snd, "/tmp/resampled.au");
		SoundSaveWav(&snd, "/tmp/resampled.wav");
		SoundSaveRaw(&snd, "/tmp/resampled.data");
	}
	else
		goto return_failure;

	// Bye!
	free(re);
	return EXIT_SUCCESS;

return_failure:
	StatusPrint(st);
	return EXIT_FAILURE;
}

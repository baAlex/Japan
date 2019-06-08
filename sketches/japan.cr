@[Link(ldflags: "~/japan-dbg.a")] # Hack!

lib LibC
	alias File = Void
	fun fopen(filename : LibC::Char*, mode : LibC::Char*) : File*
	fun fclose(file : File*)
end

lib LibJapan

	enum Endianness
		System = -1
		Little = 0
		Big
	end

	fun endian_system = "EndianSystem" : Endianness

	fun endian_reverse_64 = "EndianReverse_64"(value : LibC::UInt64T) : LibC::UInt64T
	fun endian_reverse_32 = "EndianReverse_32"(value : LibC::UInt32T) : LibC::UInt32T
	fun endian_reverse_16 = "EndianReverse_16"(value : LibC::UInt16T) : LibC::UInt16T

	fun endian_to_64 = "EndianTo_64"(value : LibC::UInt64T, from : Endianness, to : Endianness) : LibC::UInt64T
	fun endian_to_32 = "EndianTo_32"(value : LibC::UInt32T, from : Endianness, to : Endianness) : LibC::UInt32T
	fun endian_to_16 = "EndianTo_16"(value : LibC::UInt16T, from : Endianness, to : Endianness) : LibC::UInt16T

	###############################

	enum StatusCode
		Success
		Error
		FileSystemError
		IOError
		UnexpectedEOF
		UnexpectedData
		UnknownFileFormat
		UnsupportedFeature
		ObsoleteFeature
		UnknownDataFormat
		InvalidArgument
	end

	struct Status
		code : StatusCode
		function_name : LibC::Char[32]
		explanation : LibC::Char[224]
	end

	fun status_set = "StatusSet"(st : Status*, function_name : LibC::Char*, code : StatusCode, explanation_fmt : LibC::Char*, ...) : Void
	fun status_print = "StatusPrint"(st : Status) : LibC::Int

	###############################

	enum ImageFormat
		Gray8
		GrayA8
		RGB8
		RGBA8
		Gray16
		GrayA16
		RGB16
		RGBA16
	end

	struct Image
		width : LibC::SizeT
		height : LibC::SizeT
		size : LibC::SizeT
		format : ImageFormat
		data : Void*
	end

	fun image_create = "ImageCreate"(format : ImageFormat, width : LibC::SizeT, height : LibC::SizeT) : Image*
	fun image_delete = "ImageDelete"(Image*) : Void

	fun image_load = "ImageLoad"(filename : LibC::Char*, out_st : Status*) : Image*
	fun image_save_sgi = "ImageSaveSgi"(Image*, filename : LibC::Char*) : Status
	fun image_save_raw = "ImageSaveRaw"(Image*, filename : LibC::Char*) : Status

	###############################

	enum SoundFormat
		Integer8
		Integer16
		Integer32
		Float32
		Float64
	end

	enum SoundCompression
		Uncompressed
		ALaw
		ULaw
	end

	struct Sound
		frequency : LibC::SizeT
		channels : LibC::SizeT
		length : LibC::SizeT
		size : LibC::SizeT
		format : SoundFormat
		data : Void*
	end

	struct SoundEx
		frequency : LibC::SizeT
		channels : LibC::SizeT
		length : LibC::SizeT
		uncompressed_size : LibC::SizeT
		minimum_unit_size : LibC::SizeT

		endianness : Endianness
		compression : SoundCompression
		format : SoundFormat

		unsigned_8bit : LibC::Int
		unspecified_size : LibC::Int

		data_offset : LibC::SizeT
	end

	fun sound_create = "SoundCreate"(format : SoundFormat, length : LibC::SizeT, channels : LibC::SizeT, frequency : LibC::SizeT) : Sound*
	fun sound_delete = "SoundDelete"(sound : Sound*) : Void

	fun sound_load = "SoundLoad"(filename : LibC::Char*, out_st : Status*) : Sound*
	fun sound_save_au = "SoundSaveAu"(sound : Sound*, filename : LibC::Char*) : Status
	fun sound_save_wav = "SoundSaveWav"(sound : Sound*, filename : LibC::Char*) : Status
	fun sound_save_raw = "SoundSaveRaw"(sound : Sound*, filename : LibC::Char*) : Status

	fun sound_ex_load = "SoundExLoad"(file : LibC::File*, out_ex : SoundEx*, out_st : Status*) : LibC::Int
	fun sound_ex_read = "SoundExRead"(file : LibC::File*, ex : SoundEx, out_size : LibC::SizeT, out_dest : Void*, out_st : Status*) : LibC::SizeT
end

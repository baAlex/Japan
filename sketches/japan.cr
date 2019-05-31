
@[Link(ldflags: "~/japan-dbg.a")] # Hack!
lib LibJapan

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

	####

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

	####

	enum SoundFormat
		Integer8
		Integer16
		Integer32
		Float32
		Float64
	end

	struct Sound
		frequency : LibC::SizeT
		channels : LibC::SizeT
		length : LibC::SizeT
		size : LibC::SizeT
		format : SoundFormat
		data : Void*
	end

	fun sound_create = "SoundCreate"(format : SoundFormat, length : LibC::SizeT, channels : LibC::SizeT, frequency : LibC::SizeT) : Sound*
	fun sound_delete = "SoundDelete"(Sound*) : Void

	fun sound_load = "SoundLoad"(filename : LibC::Char*, out_st : Status*) : Sound*
	fun sound_save_au = "SoundSaveAu"(Sound*, filename : LibC::Char*) : Status
	fun sound_save_wav = "SoundSaveWav"(Sound*, filename : LibC::Char*) : Status
	fun sound_save_raw = "SoundSaveRaw"(Sound*, filename : LibC::Char*) : Status
end

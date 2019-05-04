
require "ffi"

module Japan

	extend FFI::Library
	ffi_lib "japan-dbg"

	# ----------------

	ErrorCode = enum(:no_error,
	                 :unkhown,
	                 :filesystem,
	                 :input_output,
	                 :broken,
	                 :unsupported,
	                 :unkhown_format,
	                 :obsolete,
	                 :argument)

	class Error < FFI::Struct
	layout :code, ErrorCode,
	       :function, [:char, 32],
	       :explanation, [:char, 224]
	end

	attach_function :ErrorPrint, [Japan::Error.by_value], :int

	# ----------------

	SoundFormat = enum(:i8, :i16, :i32, :f32, :f64)

	class Sound < FFI::Struct
	layout :frequency, :size_t,
	       :channels, :size_t,
	       :length, :size_t,
	       :size, :size_t,
	       :format, SoundFormat,
	       :data, :pointer
	end

	attach_function :SoundLoad, [:string, Japan::Error], Japan::Sound
	attach_function :SoundDelete, [Japan::Sound], :void
	attach_function :SoundSaveAu, [Japan::Sound, :string], Japan::Error.by_value
	attach_function :SoundSaveWav, [Japan::Sound, :string], Japan::Error.by_value
	attach_function :SoundSaveRaw, [Japan::Sound, :string], Japan::Error.by_value

	# ----------------

	ImageFormat = enum(:gray8, :graya8, :rgb8, :rgba8,
	                   :gray16, :graya16, :rgb16, :rgba16)

	class Image < FFI::Struct
	layout :width, :size_t,
	       :height, :size_t,
	       :size, :size_t,
	       :format, ImageFormat,
	       :data, :pointer
	end

	attach_function :ImageLoad, [:string, Japan::Error], Japan::Image
	attach_function :ImageDelete, [Japan::Image], :void
	attach_function :ImageSaveSgi, [Japan::Image, :string], Japan::Error.by_value
	attach_function :ImageSaveRaw, [Japan::Image, :string], Japan::Error.by_value
end

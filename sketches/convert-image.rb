
# $ LD_LIBRARY_PATH=../ ruby convert-image.rb [file]

require "./japan.rb"

if ARGV.length != 0 then

	snd = nil
	err = Japan::Error.new()

	if (snd = Japan.ImageLoad(ARGV[0], err)) != nil then
		Japan.ImageSaveSgi(snd, "#{ARGV[0]}-conversion.sgi")
		Japan.ImageSaveRaw(snd, "#{ARGV[0]}-conversion.data")
		Japan.SoundDelete(snd)
	else
		Japan.ErrorPrint(err)
	end
else
	puts("No input specified")
end

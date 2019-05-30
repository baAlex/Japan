
# $ LD_LIBRARY_PATH=../ ruby convert-image.rb [file]

require "./japan.rb"

if ARGV.length != 0 then

	snd = nil
	st = Japan::Status.new()

	if (snd = Japan.ImageLoad(ARGV[0], st)) != nil then
		Japan.ImageSaveSgi(snd, "#{ARGV[0]}-conversion.sgi")
		Japan.ImageSaveRaw(snd, "#{ARGV[0]}-conversion.data")
		Japan.SoundDelete(snd)
	else
		Japan.StatusPrint(st)
	end
else
	printf("No input specified\n")
end

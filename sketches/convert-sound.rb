
# $ LD_LIBRARY_PATH=../ ruby convert-sound.rb [file]

require "./japan.rb"

if ARGV.length != 0 then

	snd = nil
	err = Japan::Error.new()

	if (snd = Japan.SoundLoad(ARGV[0], err)) != nil then
		Japan.SoundSaveAu(snd, "#{ARGV[0]}-conversion.au")
		Japan.SoundSaveWav(snd, "#{ARGV[0]}-conversion.wav")
		Japan.SoundSaveRaw(snd, "#{ARGV[0]}-conversion.data")
		Japan.SoundDelete(snd)
	else
		Japan.ErrorPrint(err)
	end
else
	printf("No input specified\n")
end


# $ LD_LIBRARY_PATH=../ ruby sound.rb [file]

require "./japan.rb"

if ARGV.length != 0 then

	snd = nil
	err = Japan::Error.new()

	if (snd = Japan.SoundLoad(ARGV[0], err)) != nil then
		Japan.SoundSaveAu(snd, "test.au")
		Japan.SoundSaveWav(snd, "test.wav")
		Japan.SoundSaveRaw(snd, "test.raw")
		Japan.SoundDelete(snd)
	else
		Japan.ErrorPrint(err)
	end
else
	puts("No input specified")
end

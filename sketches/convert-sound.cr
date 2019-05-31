
# $ crystal ./sketches/convert-sound.cr [file]

require "./japan.cr"

if ARGV.size != 0

	st = LibJapan::Status.new()
	sound = LibJapan.sound_load(ARGV[0], pointerof(st))

	if sound.null? == false
		LibJapan.sound_save_au(sound, "#{ARGV[0]}-conversion.au")
		LibJapan.sound_save_wav(sound, "#{ARGV[0]}-conversion.wav")
		LibJapan.sound_save_raw(sound, "#{ARGV[0]}-conversion.data")
		LibJapan.sound_delete(sound)
	else
		LibJapan.status_print(st)
	end
else
	printf("No input specified\n")
end

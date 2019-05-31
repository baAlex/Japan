
# $ crystal ./sketches/convert-image.cr [file]

require "./japan.cr"

if ARGV.size != 0

	st = LibJapan::Status.new()
	image = LibJapan.image_load(ARGV[0], pointerof(st))

	if image.null? == false
		LibJapan.image_save_sgi(image, "#{ARGV[0]}-conversion.sgi")
		LibJapan.image_save_raw(image, "#{ARGV[0]}-conversion.data")
		LibJapan.image_delete(image)
	else
		LibJapan.status_print(st)
	end
else
	printf("No input specified\n")
end

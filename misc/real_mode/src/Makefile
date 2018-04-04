all: loader

loader: loader.s
	as --32 loader.s -o loader.o
	ld -melf_i386 loader.o -o loader
	objcopy -O binary --only-section=.text loader loader.img

run:
	qemu-system-i386 -bios ./bin/bios.bin -drive file=./bin/loader.img,format=raw

clean:
	rm -vf loader.o loader loader.img

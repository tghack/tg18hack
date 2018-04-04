AS=nasm
AFLAGS=-felf64
LD=ld

.PHONY: all clean

all: shellcode

shellcode: shellcode.s
	$(AS) $(AFLAGS) $^ -o $@.o
	$(LD) $@.o -o $@
	strip $@

clean:
	rm -vf shellcode shellcode.o

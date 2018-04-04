.globl _start

.text
.code16
_start:
	movw	$0x800, %ax
	movw	%ax, %ds
	xorw	%si, %si

loop:    
	# lodsb loads a byte from ds:si into al
	# and increments si
	lodsb
	cmpb	$0, %al		
	jz		done		# if (c == '\0') exit loop

	movb	$14, %ah		# else print c
	movw	$0x0002, %bx
	# int 0x10 sends a character to the display
	# ah = 0xe (14)
	# al = character to write
	# bh = active page number (we use 0x00)
	# bl = foreground color (we use 0x02)
	int	$0x10	         
	jmp	loop

done:
	retw			# Return to caller

# boot loader signature
.org 510,0
.word 0xaa55

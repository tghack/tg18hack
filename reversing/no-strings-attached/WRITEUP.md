# No Strings Attached Writeup

We are given a binary and we need to figure out the secret password. Let's begin by running the binary to see what we're dealing with.

```
$ ./checker
Usage: ./checker <password>
$ ./checker testingyo
Nope :(
```

One hint can be found in the title of the task. We can examine the binary using the `strings` utility to see if the password is stored in plain text.


```
/lib64/ld-linux-x86-64.so.2
libc.so.6
puts
__stack_chk_fail
strlen
stderr
fprintf
strcmp
__libc_start_main
__gmon_start__
GLIBC_2.4
GLIBC_2.2.5
UH-`
AWAVA
AUATL
[]A\A]A^A_
$*oGX,(X
Usage: %s <password>
z3r0_c00l_w4s_h3r3
Congrats! The flag is:
Nope :(
;*3$"
GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.5) 5.4.0 20160609
crtstuff.c
__JCR_LIST__
deregister_tm_clones
__do_global_dtors_aux
completed.7585
__do_global_dtors_aux_fini_array_entry
frame_dummy
__frame_dummy_init_array_entry
checker.c
print_flag
__FRAME_END__
__JCR_END__
__init_array_end
_DYNAMIC
__init_array_start
__GNU_EH_FRAME_HDR
_GLOBAL_OFFSET_TABLE_
__libc_csu_fini
_ITM_deregisterTMCloneTable
puts@@GLIBC_2.2.5
_edata
strlen@@GLIBC_2.2.5
__stack_chk_fail@@GLIBC_2.4
__libc_start_main@@GLIBC_2.2.5
__data_start
strcmp@@GLIBC_2.2.5
fprintf@@GLIBC_2.2.5
__gmon_start__
__dso_handle
_IO_stdin_used
__libc_csu_init
__bss_start
main
_Jv_RegisterClasses
__TMC_END__
_ITM_registerTMCloneTable
stderr@@GLIBC_2.2.5
.symtab
.strtab
.shstrtab
.interp
.note.ABI-tag
.note.gnu.build-id
.gnu.hash
.dynsym
.dynstr
.gnu.version
.gnu.version_r
.rela.dyn
.rela.plt
.init
.plt.got
.text
.fini
.rodata
.eh_frame_hdr
.eh_frame
.init_array
.fini_array
.jcr
.dynamic
.got.plt
.data
.bss
.comment
```

One of the strings stand out: `z3r0_c00l_w4s_h3r3`. Let's try this as the password:

```
$ ./checker z3r0_c00l_w4s_h3r3
Congrats! The flag is: TG18{I_w4s_ly1ng_ab0ut_th3_str1ngs}
```

Nice! Alternatively, we can find the password with `ltrace` by looking for library calls like `strcmp()` used when comparing the password with our input.

```
$ ltrace ./checker test
__libc_start_main(0x4007bf, 2, 0x7ffcfb683ad8, 0x400850 <unfinished ...>
strcmp("test", "z3r0_c00l_w4s_h3r3")             = -6
puts("Nope :("Nope :(
)                                  = 8
+++ exited (status 0) +++
```

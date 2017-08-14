.file "msvcrt_iface.S"
#ifndef _WIN64
.bss
.globl __imp____getmainargs
.globl __imp___setmode
.globl __imp___onexit

.align 4
MSVCRT_IAT_SUBST:
__imp____getmainargs:
	.space 4
__imp___setmode:
	.space 4
__imp___onexit:
	.space 4
	
.section .rdata,"dr"
MSVCRT_FN_NAME_TABLE:
	.long _NAME___getmainargs
	.long _NAME__setmode
	.long _NAME__onexit

_NAME___getmainargs:
	.ascii "__getmainargs\0"
_NAME__setmode:
	.ascii "_setmode\0"
_NAME__onexit:
	.ascii "_onexit\0"
	
_NAME_msvcrt:
	.ascii "msvcrt.dll\0"
_NAME_msvcrt20:
	.ascii "msvcrt20.dll\0"

.text                           /* Code section */
/* Make functions visible to linker, CDECL name mangling */
.globl _PreLoadMsvcrt
.globl ___getmainargs
.globl __setmode
.globl __onexit
/* Defining symbols, COFF format, storage-class 2 (external), attribute 32 (function) */
.def _PreLoadMsvcrt;           .scl 2; .type 32; .endef
.def __imp__GetProcAddress@8;  .scl 2; .type 32; .endef
.def __imp__LoadLibraryA@4;    .scl 2; .type 32; .endef
.def ___getmainargs;           .scl 2; .type 32; .endef
.def __setmode;                .scl 2; .type 32; .endef
.def __onexit;                 .scl 2; .type 32; .endef

_PreLoadMsvcrt:
	/* 
	Function prototype is HANDLE __cdecl PreLoadMsvcrt()
	EAX ECX EDX
	1 DWORD is passed on the stack (ESP offsets): 
	 0x00 Return address
	*/
	pushl %ebx                  /* Saving EBX */
	pushl %ebp                  /* Creating stack frame */
	movl %esp, %ebp
	pushl $_NAME_msvcrt20
	pushl $_NAME_msvcrt
msvcrt_next:
	call *__imp__LoadLibraryA
	testl %eax, %eax
	jz wrong_msvcrt
	movl %eax, %ebx             /* Saving MSVCRT module handle to EBX */
	/*
	No branching if LoadLibrary("msvcrt.dll") returns valid handle (most common case)
	Next come MSVCRT IAT substitute population
	*/
	pushl $_NAME___getmainargs
	pushl %ebx
	call *__imp__GetProcAddress
	testl %eax, %eax
	jz exit_fn
	movl %eax, __imp____getmainargs
	/* END OF IAT POPULATION */
	movl %ebx, %eax
exit_fn:
	leave                       /* Leaving stack frame */
	popl %ebx                   /* Restoring EBX */
	ret
wrong_msvcrt:
	cmpl %esp, %ebp             /* Test if no MSVCRT variants remain */
	jne msvcrt_next
	jmp exit_fn
	
___getmainargs:
	jmp *__imp____getmainargs
	
__setmode:
	jmp *__imp___setmode
	
__onexit:
	jmp *__imp___onexit

#endif
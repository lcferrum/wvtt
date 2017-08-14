.file "asm_patches.S"
.text                                 /* Code section */
#ifndef _WIN64
.globl _GetSystemMetricsSehWrapper@8  /* Make function visible to linker, STDCALL name mangling */
/* Defining symbols, COFF format, storage-class 2 (external), attribute 32 (function) */
.def _GetSystemMetricsSehWrapper@8;  .scl 2; .type 32; .endef
.def __imp__GetSystemMetrics@4;      .scl 2; .type 32; .endef
.def __imp__RtlUnwind@16;            .scl 2; .type 32; .endef
    
_GetSystemMetricsSehWrapper@8:
	/* 
	Function prototype is bool __stdcall GetSystemMetricsSehWrapper(int, int*)
	3 DWORDs are passed on the stack (stack frame offsets): 
	 0x04 Return address
	 0x08 System metric index
	 0x0C Memory pointer to int variable where GetSystemMetrics result will be stored
	*/
	pushl %ebp                        /* Creating stack frame (we need to save EBP anyway, see below) */
	movl %esp, %ebp                
	subl $16, %esp                    /* Allocating space for local variables */
	movl %ebx, -0x04(%ebp)            /* Save current EBX, ESI, EDI and EBP (saved earlier) register state ... */
	movl %esi, -0x08(%ebp)
	movl %edi, -0x0C(%ebp)            /* ...because neither failed call nor RtlUnwind guarantees not to trash non-volatile registers */
	movl $seh_handler, -0x10(%ebp)    /* Offset to exception handler */    
	pushl %fs:0                       /* At [FS:0] is a pointer to current EXCEPTION_REGISTRATION struct (ESP=EBP-0x14) */
	/* 
	Now on the stack we have this structure (stack frame offsets, ESP offsets):
	 EXCEPTION_REGISTRATION {
	    DWORD prev = [FS:0]           -0x14    0x00
	    DWORD handler = seh_handler   -0x10    0x04
	 }
	And also this additional data:
	 SAVED_REGS {
	    DWORD edi                     -0x0C    0x08
	    DWORD esi                     -0x08    0x0C
	    DWORD ebx                     -0x04    0x10
	    DWORD ebp                     -0x00    0x14
	 }
	*/
	movl %esp, %fs:0                  /* Install new exception handler by assigning EXCEPTION_REGISTRATION struct pointer (which is current stack top) to [FS:0] (ESP=EBP-0x14) */
	pushl 0x8(%ebp)                   /* Push GetSystemMetrics parameter to stack */
	call *__imp__GetSystemMetrics     /* Calling GetSystemMetrics - it's STDCALL function and it potentially can cause memory access violation */
	/* SEH wasn't triggered */
	movl 0xC(%ebp), %ecx              /* Saving GetSystemMetrics result to variable pointed by second argument */
	movl %eax, (%ecx)
	popl %fs:0                        /* Restore old exception handler by restoring pointer to previous EXCEPTION_REGISTRATION struct (ESP=EBP-0x14) */
	movl $1, %eax                     /* Return true */
	leave                             /* Leaving stack frame */
	ret $8                            /* Clean up call stack (8 bytes) conforming to STDCALL convention */
seh_handler:
	/* 
	SEH was triggered and OS has called this label as the following function:
	EXCEPTION_DISPOSITION __cdecl seh_handler(EXCEPTION_RECORD*, EXCEPTION_REGISTRATION*, CONTEXT*, DISPATCHER_CONTEXT*)
	5 DWORDs are passed on the stack (ESP offsets): 
	 0x00 Exception handler return address
	 0x04 Pointer to EXCEPTION_RECORD struct
	 0x08 Pointer to our own EXCEPTION_REGISTRATION struct (that also pose as stack top right after this exception handler was set)
	 0x0C Pointer to CONTEXT struct
	 0x10 Pointer to DISPATCHER_CONTEXT struct
	We can MOV to EAX one of EXCEPTION_DISPOSITION values and RET from handler function (we will return to OS SEH handling code and not to GetSystemMetricsSehWrapper caller)
	Or we can forget about returning from handler function altogether and just get back to our current function
	*/
	movl 0x04(%esp), %eax             /* Move pointer to EXCEPTION_RECORD to EAX */
	cmpl $0, 0x04(%eax)               /* If EXCEPTION_RECORD.ExceptionFlags!=0 JMP not_handled */
	jne not_handled
	cmpl $0xC0000005, 0x00(%eax)      /* If EXCEPTION_RECORD.ExceptionCode!=EXCEPTION_ACCESS_VIOLATION JMP not_handled */
	jne not_handled
	movl 0x08(%esp), %eax             /* Move pointer to EXCEPTION_REGISTRATION to EAX */
	pushl $0                          /* Calling RtlUnwind(EXCEPTION_REGISTRATION*, OFFSET rtl_unwind_ret, NULL, NULL) */
	pushl $0                          /* Purpose of RtlUnwind is to call __finally blocks of all the non-triggered exception handlers and uninstall them (by setting [FS:0] to previous handler) one by one */
	pushl $rtl_unwind_ret             /* Our own exception handler doesn't have __finally-like statements so it jumps to not_handled if EXCEPTION_RECORD.ExceptionFlags contains EH_UNWINDING */
	pushl %eax
	call *__imp__RtlUnwind            /* It's STDCALL function */
rtl_unwind_ret:
	movl 0x08(%esp), %esp             /* Make EXCEPTION_REGISTRATION pointer current stack pointer (effectively continuing GetSystemMetricsSehWrapper execution) */
	popl %fs:0                        /* Restore old exception handler by restoring pointer to previous EXCEPTION_REGISTRATION struct */
	movl 0x04(%esp), %edi             /* Restoring EBX, ESI, EDI and EBP from SAVED_REGS sruct that is right behind EXCEPTION_REGISTRATION */
	movl 0x08(%esp), %esi
	movl 0x0C(%esp), %ebx
	movl 0x10(%esp), %ebp
	addl $20, %esp                    /* Dismissing EXCEPTION_REGISTRATION.handler and SAVED_REGS - now all what is left on the stack are original function arguments */
	movl $0, %eax                     /* Return false (from GetSystemMetricsSehWrapper) */
	ret $8                            /* Clean up call stack (8 bytes) conforming to STDCALL convention */    
not_handled:
	movl $1, %eax                     /* Return ExceptionContinueSearch (from seh_handler) */
	ret                               /* Conforming to CDECL caller is responsible for cleaning up the stack */
#endif
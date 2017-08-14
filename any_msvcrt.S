.file "any_msvcrt.S"
#ifndef _WIN64
.bss
.globl _msvcrt_name_in_use
.globl __imp____getmainargs
.globl __imp____p___mb_cur_max
.globl __imp____p__environ
.globl __imp____p__fmode
.globl __imp____p__iob
.globl __imp____p__pctype
.globl __imp___cexit
.globl __imp___errno
.globl __imp___filbuf
.globl __imp___flsbuf
.globl __imp___fullpath
.globl __imp___getch
.globl __imp___isctype
.globl __imp___setmode
.globl __imp__abort
.globl __imp__atoi
.globl __imp__calloc
.globl __imp__fclose
.globl __imp__fflush
.globl __imp__fopen
.globl __imp__fputc
.globl __imp__fputs
.globl __imp__fread
.globl __imp__free
.globl __imp__fseek
.globl __imp__ftell
.globl __imp__fwrite
.globl __imp__getenv
.globl __imp__getwc
.globl __imp__iswctype
.globl __imp__localeconv
.globl __imp__malloc
.globl __imp__mbstowcs
.globl __imp__memchr
.globl __imp__memcmp
.globl __imp__memcpy
.globl __imp__memmove
.globl __imp__memset
.globl __imp__putwc
.globl __imp__realloc
.globl __imp__setlocale
.globl __imp__setvbuf
.globl __imp__signal
.globl __imp__sprintf
.globl __imp__strchr
.globl __imp__strcmp
.globl __imp__strcoll
.globl __imp__strerror
.globl __imp__strftime
.globl __imp__strlen
.globl __imp__strtod
.globl __imp__strxfrm
.globl __imp__tolower
.globl __imp__towlower
.globl __imp__towupper
.globl __imp__ungetc
.globl __imp__ungetwc
.globl __imp__vfprintf
.globl __imp__wcscat
.globl __imp__wcscoll
.globl __imp__wcscpy
.globl __imp__wcsftime
.globl __imp__wcslen
.globl __imp__wcsncmp
.globl __imp__wcstombs
.globl __imp__wcsxfrm
.globl __imp__atexit
.globl __imp___onexit
.globl __imp__fdopen
.globl __imp__fstat
.globl __imp__lseek
.globl __imp__read
.globl __imp__strdup
.globl __imp__stricoll
.globl __imp__write

.align 4
_msvcrt_name_in_use:
	.space 4
MSVCRT_IAT_SUBST:
__imp____getmainargs:
	.space 4
__imp____p___mb_cur_max:
	.space 4
__imp____p__environ:
	.space 4
__imp____p__fmode:
	.space 4
__imp____p__iob:
	.space 4
__imp____p__pctype:
	.space 4
__imp___cexit:
	.space 4
__imp___errno:
	.space 4
__imp___filbuf:
	.space 4
__imp___flsbuf:
	.space 4
__imp___fullpath:
	.space 4
__imp___getch:
	.space 4
__imp___isctype:
	.space 4
__imp___setmode:
	.space 4
__imp__abort:
	.space 4
__imp__atoi:
	.space 4
__imp__calloc:
	.space 4
__imp__fclose:
	.space 4
__imp__fflush:
	.space 4
__imp__fopen:
	.space 4
__imp__fputc:
	.space 4
__imp__fputs:
	.space 4
__imp__fread:
	.space 4
__imp__free:
	.space 4
__imp__fseek:
	.space 4
__imp__ftell:
	.space 4
__imp__fwrite:
	.space 4
__imp__getenv:
	.space 4
__imp__getwc:
	.space 4
__imp__iswctype:
	.space 4
__imp__localeconv:
	.space 4
__imp__malloc:
	.space 4
__imp__mbstowcs:
	.space 4
__imp__memchr:
	.space 4
__imp__memcmp:
	.space 4
__imp__memcpy:
	.space 4
__imp__memmove:
	.space 4
__imp__memset:
	.space 4
__imp__putwc:
	.space 4
__imp__realloc:
	.space 4
__imp__setlocale:
	.space 4
__imp__setvbuf:
	.space 4
__imp__signal:
	.space 4
__imp__sprintf:
	.space 4
__imp__strchr:
	.space 4
__imp__strcmp:
	.space 4
__imp__strcoll:
	.space 4
__imp__strerror:
	.space 4
__imp__strftime:
	.space 4
__imp__strlen:
	.space 4
__imp__strtod:
	.space 4
__imp__strxfrm:
	.space 4
__imp__tolower:
	.space 4
__imp__towlower:
	.space 4
__imp__towupper:
	.space 4
__imp__ungetc:
	.space 4
__imp__ungetwc:
	.space 4
__imp__vfprintf:
	.space 4
__imp__wcscat:
	.space 4
__imp__wcscoll:
	.space 4
__imp__wcscpy:
	.space 4
__imp__wcsftime:
	.space 4
__imp__wcslen:
	.space 4
__imp__wcsncmp:
	.space 4
__imp__wcstombs:
	.space 4
__imp__wcsxfrm:
	.space 4
__imp__atexit:
	.space 4
__imp___onexit:
	.space 4
__imp__fdopen:
	.space 4
__imp__fstat:
	.space 4
__imp__lseek:
	.space 4
__imp__read:
	.space 4
__imp__strdup:
	.space 4
__imp__stricoll:
	.space 4
__imp__write:
	.space 4
	
.section .rdata,"dr"
MSVCRT_FN_NAME_TABLE:
	.long _NAME___getmainargs
	.long _NAME___p___mb_cur_max
	.long _NAME___p__environ
	.long _NAME___p__fmode
	.long _NAME___p__iob
	.long _NAME___p__pctype
	.long _NAME__cexit
	.long _NAME__errno
	.long _NAME__filbuf
	.long _NAME__flsbuf
	.long _NAME__fullpath
	.long _NAME__getch
	.long _NAME__isctype
	.long _NAME__setmode
	.long _NAME_abort
	.long _NAME_atoi
	.long _NAME_calloc
	.long _NAME_fclose
	.long _NAME_fflush
	.long _NAME_fopen
	.long _NAME_fputc
	.long _NAME_fputs
	.long _NAME_fread
	.long _NAME_free
	.long _NAME_fseek
	.long _NAME_ftell
	.long _NAME_fwrite
	.long _NAME_getenv
	.long _NAME_getwc
	.long _NAME_iswctype
	.long _NAME_localeconv
	.long _NAME_malloc
	.long _NAME_mbstowcs
	.long _NAME_memchr
	.long _NAME_memcmp
	.long _NAME_memcpy
	.long _NAME_memmove
	.long _NAME_memset
	.long _NAME_putwc
	.long _NAME_realloc
	.long _NAME_setlocale
	.long _NAME_setvbuf
	.long _NAME_signal
	.long _NAME_sprintf
	.long _NAME_strchr
	.long _NAME_strcmp
	.long _NAME_strcoll
	.long _NAME_strerror
	.long _NAME_strftime
	.long _NAME_strlen
	.long _NAME_strtod
	.long _NAME_strxfrm
	.long _NAME_tolower
	.long _NAME_towlower
	.long _NAME_towupper
	.long _NAME_ungetc
	.long _NAME_ungetwc
	.long _NAME_vfprintf
	.long _NAME_wcscat
	.long _NAME_wcscoll
	.long _NAME_wcscpy
	.long _NAME_wcsftime
	.long _NAME_wcslen
	.long _NAME_wcsncmp
	.long _NAME_wcstombs
	.long _NAME_wcsxfrm
	.long _NAME_atexit
	.long _NAME__onexit
	.long _NAME_fdopen
	.long _NAME_fstat
	.long _NAME_lseek
	.long _NAME_read
	.long _NAME_strdup
	.long _NAME_stricoll
	.long _NAME_write

_NAME___getmainargs:
	.ascii "__getmainargs\0"
_NAME___p___mb_cur_max:
	.ascii "__p___mb_cur_max\0"
_NAME___p__environ:
	.ascii "__p__environ\0"
_NAME___p__fmode:
	.ascii "__p__fmode\0"
_NAME___p__iob:
	.ascii "__p__iob\0"
_NAME___p__pctype:
	.ascii "__p__pctype\0"
_NAME__cexit:
	.ascii "_cexit\0"
_NAME__errno:
	.ascii "_errno\0"
_NAME__filbuf:
	.ascii "_filbuf\0"
_NAME__flsbuf:
	.ascii "_flsbuf\0"
_NAME__fullpath:
	.ascii "_fullpath\0"
_NAME__getch:
	.ascii "_getch\0"
_NAME__isctype:
	.ascii "_isctype\0"
_NAME__setmode:
	.ascii "_setmode\0"
_NAME_abort:
	.ascii "abort\0"
_NAME_atoi:
	.ascii "atoi\0"
_NAME_calloc:
	.ascii "calloc\0"
_NAME_fclose:
	.ascii "fclose\0"
_NAME_fflush:
	.ascii "fflush\0"
_NAME_fopen:
	.ascii "fopen\0"
_NAME_fputc:
	.ascii "fputc\0"
_NAME_fputs:
	.ascii "fputs\0"
_NAME_fread:
	.ascii "fread\0"
_NAME_free:
	.ascii "free\0"
_NAME_fseek:
	.ascii "fseek\0"
_NAME_ftell:
	.ascii "ftell\0"
_NAME_fwrite:
	.ascii "fwrite\0"
_NAME_getenv:
	.ascii "getenv\0"
_NAME_getwc:
	.ascii "getwc\0"
_NAME_iswctype:
	.ascii "iswctype\0"
_NAME_localeconv:
	.ascii "localeconv\0"
_NAME_malloc:
	.ascii "malloc\0"
_NAME_mbstowcs:
	.ascii "mbstowcs\0"
_NAME_memchr:
	.ascii "memchr\0"
_NAME_memcmp:
	.ascii "memcmp\0"
_NAME_memcpy:
	.ascii "memcpy\0"
_NAME_memmove:
	.ascii "memmove\0"
_NAME_memset:
	.ascii "memset\0"
_NAME_putwc:
	.ascii "putwc\0"
_NAME_realloc:
	.ascii "realloc\0"
_NAME_setlocale:
	.ascii "setlocale\0"
_NAME_setvbuf:
	.ascii "setvbuf\0"
_NAME_signal:
	.ascii "signal\0"
_NAME_sprintf:
	.ascii "sprintf\0"
_NAME_strchr:
	.ascii "strchr\0"
_NAME_strcmp:
	.ascii "strcmp\0"
_NAME_strcoll:
	.ascii "strcoll\0"
_NAME_strerror:
	.ascii "strerror\0"
_NAME_strftime:
	.ascii "strftime\0"
_NAME_strlen:
	.ascii "strlen\0"
_NAME_strtod:
	.ascii "strtod\0"
_NAME_strxfrm:
	.ascii "strxfrm\0"
_NAME_tolower:
	.ascii "tolower\0"
_NAME_towlower:
	.ascii "towlower\0"
_NAME_towupper:
	.ascii "towupper\0"
_NAME_ungetc:
	.ascii "ungetc\0"
_NAME_ungetwc:
	.ascii "ungetwc\0"
_NAME_vfprintf:
	.ascii "vfprintf\0"
_NAME_wcscat:
	.ascii "wcscat\0"
_NAME_wcscoll:
	.ascii "wcscoll\0"
_NAME_wcscpy:
	.ascii "wcscpy\0"
_NAME_wcsftime:
	.ascii "wcsftime\0"
_NAME_wcslen:
	.ascii "wcslen\0"
_NAME_wcsncmp:
	.ascii "wcsncmp\0"
_NAME_wcstombs:
	.ascii "wcstombs\0"
_NAME_wcsxfrm:
	.ascii "wcsxfrm\0"
_NAME_atexit:
	.ascii "atexit\0"
_NAME__onexit:
	.ascii "_onexit\0"
_NAME_fdopen:
	.ascii "_fdopen\0"
_NAME_fstat:
	.ascii "_fstat\0"
_NAME_lseek:
	.ascii "_lseek\0"
_NAME_read:
	.ascii "_read\0"
_NAME_strdup:
	.ascii "_strdup\0"
_NAME_stricoll:
	.ascii "_stricoll\0"
_NAME_write:
	.ascii "_write\0"
	
_DLL_msvcrt:
	.ascii "MSVCRT.DLL\0"
_DLL_msvcrt40:
	.ascii "MSVCRT40.DLL\0"
_DLL_msvcrt20:
	.ascii "MSVCRT20.DLL\0"

.text                           /* Code section */
/* Make functions visible to linker, CDECL name mangling */
.globl _PreLoadMsvcrt
.globl ___getmainargs
.globl ___p___mb_cur_max
.globl ___p__environ
.globl ___p__fmode
.globl ___p__iob
.globl ___p__pctype
.globl __cexit
.globl __errno
.globl __filbuf
.globl __flsbuf
.globl __fullpath
.globl __getch
.globl __isctype
.globl __setmode
.globl _abort
.globl _atoi
.globl _calloc
.globl _fclose
.globl _fflush
.globl _fopen
.globl _fputc
.globl _fputs
.globl _fread
.globl _free
.globl _fseek
.globl _ftell
.globl _fwrite
.globl _getenv
.globl _getwc
.globl _iswctype
.globl _localeconv
.globl _malloc
.globl _mbstowcs
.globl _memchr
.globl _memcmp
.globl _memcpy
.globl _memmove
.globl _memset
.globl _putwc
.globl _realloc
.globl _setlocale
.globl _setvbuf
.globl _signal
.globl _sprintf
.globl _strchr
.globl _strcmp
.globl _strcoll
.globl _strerror
.globl _strftime
.globl _strlen
.globl _strtod
.globl _strxfrm
.globl _tolower
.globl _towlower
.globl _towupper
.globl _ungetc
.globl _ungetwc
.globl _vfprintf
.globl _wcscat
.globl _wcscoll
.globl _wcscpy
.globl _wcsftime
.globl _wcslen
.globl _wcsncmp
.globl _wcstombs
.globl _wcsxfrm
.globl _atexit
.globl __onexit
.globl _fdopen
.globl _fstat
.globl _lseek
.globl _read
.globl _strdup
.globl _stricoll
.globl _write
/* Defining symbols, COFF format, storage-class 2 (external), attribute 32 (function) */
.def _PreLoadMsvcrt;           .scl 2; .type 32; .endef
.def __imp__GetProcAddress@8;  .scl 2; .type 32; .endef
.def __imp__LoadLibraryA@4;    .scl 2; .type 32; .endef
.def ___getmainargs;           .scl 2; .type 32; .endef
.def ___p___mb_cur_max;        .scl 2; .type 32; .endef
.def ___p__environ;            .scl 2; .type 32; .endef
.def ___p__fmode;              .scl 2; .type 32; .endef
.def ___p__iob;                .scl 2; .type 32; .endef
.def ___p__pctype;             .scl 2; .type 32; .endef
.def __cexit;                  .scl 2; .type 32; .endef
.def __errno;                  .scl 2; .type 32; .endef
.def __filbuf;                 .scl 2; .type 32; .endef
.def __flsbuf;                 .scl 2; .type 32; .endef
.def __fullpath;               .scl 2; .type 32; .endef
.def __getch;                  .scl 2; .type 32; .endef
.def __isctype;                .scl 2; .type 32; .endef
.def __setmode;                .scl 2; .type 32; .endef
.def _abort;                   .scl 2; .type 32; .endef
.def _atoi;                    .scl 2; .type 32; .endef
.def _calloc;                  .scl 2; .type 32; .endef
.def _fclose;                  .scl 2; .type 32; .endef
.def _fflush;                  .scl 2; .type 32; .endef
.def _fopen;                   .scl 2; .type 32; .endef
.def _fputc;                   .scl 2; .type 32; .endef
.def _fputs;                   .scl 2; .type 32; .endef
.def _fread;                   .scl 2; .type 32; .endef
.def _free;                    .scl 2; .type 32; .endef
.def _fseek;                   .scl 2; .type 32; .endef
.def _ftell;                   .scl 2; .type 32; .endef
.def _fwrite;                  .scl 2; .type 32; .endef
.def _getenv;                  .scl 2; .type 32; .endef
.def _getwc;                   .scl 2; .type 32; .endef
.def _iswctype;                .scl 2; .type 32; .endef
.def _localeconv;              .scl 2; .type 32; .endef
.def _malloc;                  .scl 2; .type 32; .endef
.def _mbstowcs;                .scl 2; .type 32; .endef
.def _memchr;                  .scl 2; .type 32; .endef
.def _memcmp;                  .scl 2; .type 32; .endef
.def _memcpy;                  .scl 2; .type 32; .endef
.def _memmove;                 .scl 2; .type 32; .endef
.def _memset;                  .scl 2; .type 32; .endef
.def _putwc;                   .scl 2; .type 32; .endef
.def _realloc;                 .scl 2; .type 32; .endef
.def _setlocale;               .scl 2; .type 32; .endef
.def _setvbuf;                 .scl 2; .type 32; .endef
.def _signal;                  .scl 2; .type 32; .endef
.def _sprintf;                 .scl 2; .type 32; .endef
.def _strchr;                  .scl 2; .type 32; .endef
.def _strcmp;                  .scl 2; .type 32; .endef
.def _strcoll;                 .scl 2; .type 32; .endef
.def _strerror;                .scl 2; .type 32; .endef
.def _strftime;                .scl 2; .type 32; .endef
.def _strlen;                  .scl 2; .type 32; .endef
.def _strtod;                  .scl 2; .type 32; .endef
.def _strxfrm;                 .scl 2; .type 32; .endef
.def _tolower;                 .scl 2; .type 32; .endef
.def _towlower;                .scl 2; .type 32; .endef
.def _towupper;                .scl 2; .type 32; .endef
.def _ungetc;                  .scl 2; .type 32; .endef
.def _ungetwc;                 .scl 2; .type 32; .endef
.def _vfprintf;                .scl 2; .type 32; .endef
.def _wcscat;                  .scl 2; .type 32; .endef
.def _wcscoll;                 .scl 2; .type 32; .endef
.def _wcscpy;                  .scl 2; .type 32; .endef
.def _wcsftime;                .scl 2; .type 32; .endef
.def _wcslen;                  .scl 2; .type 32; .endef
.def _wcsncmp;                 .scl 2; .type 32; .endef
.def _wcstombs;                .scl 2; .type 32; .endef
.def _wcsxfrm;                 .scl 2; .type 32; .endef
.def _atexit;                  .scl 2; .type 32; .endef
.def _onexit;                  .scl 2; .type 32; .endef
.def _fdopen;                  .scl 2; .type 32; .endef
.def _fstat;                   .scl 2; .type 32; .endef
.def _lseek;                   .scl 2; .type 32; .endef
.def _read;                    .scl 2; .type 32; .endef
.def _strdup;                  .scl 2; .type 32; .endef
.def _stricoll;                .scl 2; .type 32; .endef
.def _write;                   .scl 2; .type 32; .endef

_PreLoadMsvcrt:
	/* 
	Function prototype is HANDLE __cdecl PreLoadMsvcrt()
	1 DWORD is passed on the stack (ESP offsets): 
	 0x00 Return address
	*/
	pushl %ebx                  /* Saving EBX */
	pushl %esi                  /* Saving ESI */
	pushl %ebp                  /* Creating stack frame */
	movl %esp, %ebp
	pushl $_DLL_msvcrt20
	pushl $_DLL_msvcrt40
	pushl $_DLL_msvcrt
msvcrt_next:
	movl 0x0(%esp), %ebx        /* Saving queried MSVCRT name to EBX */
	call *__imp__LoadLibraryA
	testl %eax, %eax
	jz wrong_msvcrt
	movl %ebx, _msvcrt_name_in_use
	movl %eax, %ebx             /* Saving MSVCRT module handle to EBX */
	movl $296, %esi             /* Offset to the last function in MSVCRT IAT substitute: 4 * (number_of_fns - 1) */
next_fn:
	pushl MSVCRT_FN_NAME_TABLE(%esi)
	pushl %ebx
	call *__imp__GetProcAddress
	testl %eax, %eax
	jz exit_fn                  /* Function not found - exit w/ EAX=0 */
	movl %eax, MSVCRT_IAT_SUBST(%esi)
	subl $4, %esi
	jnc next_fn
	movl %ebx, %eax
exit_fn:
	leave                       /* Leaving stack frame */
	popl %esi                   /* Restoring ESI */
	popl %ebx                   /* Restoring EBX */
	ret
wrong_msvcrt:
	cmpl %esp, %ebp             /* Test if no MSVCRT variants remain */
	jne msvcrt_next
	jmp exit_fn                 /* MSVCRT not found - exit w/ EAX=0 */
	
___getmainargs:
	jmp *__imp____getmainargs

___p___mb_cur_max:
	jmp *__imp____p___mb_cur_max

___p__environ:
	jmp *__imp____p__environ

___p__fmode:
	jmp *__imp____p__fmode

___p__iob:
	jmp *__imp____p__iob

___p__pctype:
	jmp *__imp____p__pctype

__cexit:
	jmp *__imp___cexit

__errno:
	jmp *__imp___errno

__filbuf:
	jmp *__imp___filbuf

__flsbuf:
	jmp *__imp___flsbuf

__fullpath:
	jmp *__imp___fullpath

__getch:
	jmp *__imp___getch

__isctype:
	jmp *__imp___isctype

__setmode:
	jmp *__imp___setmode

_abort:
	jmp *__imp__abort

_atoi:
	jmp *__imp__atoi

_calloc:
	jmp *__imp__calloc

_fclose:
	jmp *__imp__fclose

_fflush:
	jmp *__imp__fflush

_fopen:
	jmp *__imp__fopen

_fputc:
	jmp *__imp__fputc

_fputs:
	jmp *__imp__fputs

_fread:
	jmp *__imp__fread

_free:
	jmp *__imp__free

_fseek:
	jmp *__imp__fseek

_ftell:
	jmp *__imp__ftell

_fwrite:
	jmp *__imp__fwrite

_getenv:
	jmp *__imp__getenv

_getwc:
	jmp *__imp__getwc

_iswctype:
	jmp *__imp__iswctype

_localeconv:
	jmp *__imp__localeconv

_malloc:
	jmp *__imp__malloc

_mbstowcs:
	jmp *__imp__mbstowcs

_memchr:
	jmp *__imp__memchr

_memcmp:
	jmp *__imp__memcmp

_memcpy:
	jmp *__imp__memcpy

_memmove:
	jmp *__imp__memmove

_memset:
	jmp *__imp__memset

_putwc:
	jmp *__imp__putwc

_realloc:
	jmp *__imp__realloc

_setlocale:
	jmp *__imp__setlocale

_setvbuf:
	jmp *__imp__setvbuf

_signal:
	jmp *__imp__signal

_sprintf:
	jmp *__imp__sprintf

_strchr:
	jmp *__imp__strchr

_strcmp:
	jmp *__imp__strcmp

_strcoll:
	jmp *__imp__strcoll

_strerror:
	jmp *__imp__strerror

_strftime:
	jmp *__imp__strftime

_strlen:
	jmp *__imp__strlen

_strtod:
	jmp *__imp__strtod

_strxfrm:
	jmp *__imp__strxfrm

_tolower:
	jmp *__imp__tolower

_towlower:
	jmp *__imp__towlower

_towupper:
	jmp *__imp__towupper

_ungetc:
	jmp *__imp__ungetc

_ungetwc:
	jmp *__imp__ungetwc

_vfprintf:
	jmp *__imp__vfprintf

_wcscat:
	jmp *__imp__wcscat

_wcscoll:
	jmp *__imp__wcscoll

_wcscpy:
	jmp *__imp__wcscpy

_wcsftime:
	jmp *__imp__wcsftime

_wcslen:
	jmp *__imp__wcslen

_wcsncmp:
	jmp *__imp__wcsncmp

_wcstombs:
	jmp *__imp__wcstombs

_wcsxfrm:
	jmp *__imp__wcsxfrm
	
_fdopen:
	jmp *__imp__fdopen

_fstat:
	jmp *__imp__fstat

_lseek:
	jmp *__imp__lseek

_read:
	jmp *__imp__read

_strdup:
	jmp *__imp__strdup

_stricoll:
	jmp *__imp__stricoll

_write:
	jmp *__imp__write
#endif
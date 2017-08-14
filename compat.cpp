#ifdef _WIN64
#error Only for x86 builds!
#endif

#include <cstdio>
#include <windows.h>

extern "C" int* __cdecl __p___mb_cur_max();
int *_IAT___mb_cur_max asm("__imp____mb_cur_max")=NULL;

extern "C" FILE** __cdecl __p__iob();
FILE **_IAT__iob asm("__imp___iob")=NULL;

extern "C" unsigned short** __cdecl __p__pctype();
unsigned short **_IAT__pctype asm("__imp___pctype")=NULL;

static void __cdecl _COMPAT___set_app_type(int at);
void (__cdecl *_IAT___set_app_type)(int) asm("__imp____set_app_type")=NULL;
extern "C" void __cdecl __set_app_type(int at) { _IAT___set_app_type(at); }

static BOOL WINAPI _COMPAT_IsDBCSLeadByteEx(UINT CodePage, BYTE TestChar);
BOOL (WINAPI *_IAT_IsDBCSLeadByteEx)(UINT, BYTE) asm("__imp__IsDBCSLeadByteEx@8")=NULL;
extern "C" BOOL WINAPI IsDBCSLeadByteEx(UINT CodePage, BYTE TestChar) { return _IAT_IsDBCSLeadByteEx(CodePage, TestChar); }

extern "C" void mainCRTStartup();
#if X86_3X==2
extern "C" HMODULE PreLoadMsvcrt();
#endif

extern "C" void CompatCRTStartup()
{
#if X86_3X==1
	HMODULE hModule=GetModuleHandle("msvcrt.dll");
#else
	//Disable Windows error dialog popup for failed LoadLibrary attempts on NT3.x and Win32s
	//This is done so GetSystemFilePath can search for needed files using LoadLibrary
	//For X86_3XAM (X86_3X==2) this is done this early because of LoadLibrary calls in PreLoadMsvcrt
	SetErrorMode(SEM_NOOPENFILEERRORBOX|SEM_FAILCRITICALERRORS);
	HMODULE hModule=PreLoadMsvcrt();
	
	if (!hModule) {
		MessageBox(NULL, "Can't find MSVCRT.DLL.", "Unable To Locate Component", MB_ICONERROR|MB_OK);
		ExitProcess(0xC0000135);
		return;
	}
#endif
	
	if (!(_IAT___mb_cur_max=(decltype(_IAT___mb_cur_max))GetProcAddress(hModule, "__mb_cur_max"))) {
		_IAT___mb_cur_max=__p___mb_cur_max();
	}
	
	if (!(_IAT__iob=(decltype(_IAT__iob))GetProcAddress(hModule, "_iob"))) {
		_IAT__iob=__p__iob();
	}
	
	if (!(_IAT__pctype=(decltype(_IAT__pctype))GetProcAddress(hModule, "_pctype"))) {
		_IAT__pctype=__p__pctype();
	}
	
	if (!(_IAT___set_app_type=(decltype(_IAT___set_app_type))GetProcAddress(hModule, "__set_app_type"))) {
		_IAT___set_app_type=_COMPAT___set_app_type;
	}
	
	hModule=GetModuleHandle("kernel32.dll");
	
	if (!(_IAT_IsDBCSLeadByteEx=(decltype(_IAT_IsDBCSLeadByteEx))GetProcAddress(hModule, "IsDBCSLeadByteEx"))) {
		_IAT_IsDBCSLeadByteEx=_COMPAT_IsDBCSLeadByteEx;
	}
		
	mainCRTStartup();
}

static BOOL WINAPI _COMPAT_IsDBCSLeadByteEx(UINT CodePage, BYTE TestChar)
{
	CPINFO cpinfo;

	if (GetCPInfo(CodePage, &cpinfo)) {
		if (cpinfo.MaxCharSize==2) {
			for (int i=0; i<MAX_LEADBYTES&&cpinfo.LeadByte[i]; i+=2) {
				if (TestChar>=cpinfo.LeadByte[i]&&TestChar<=cpinfo.LeadByte[i+1])
					return TRUE;
			}
		}
	}
	
	return FALSE;
}

static void __cdecl _COMPAT___set_app_type(int at)
{}

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

#ifdef COMPAT_GUI
extern "C" void WinMainCRTStartup();
#define OriginalCRTStarup WinMainCRTStartup
#else
extern "C" void mainCRTStartup();
#define OriginalCRTStarup mainCRTStartup
#endif

extern "C" void CompatCRTStartup()
{
	if (HMODULE hMsvcrt=GetModuleHandle("msvcrt.dll")) {
		if (!(_IAT___mb_cur_max=(decltype(_IAT___mb_cur_max))GetProcAddress(hMsvcrt, "__mb_cur_max"))) {
			_IAT___mb_cur_max=__p___mb_cur_max();
		}
		
		if (!(_IAT__iob=(decltype(_IAT__iob))GetProcAddress(hMsvcrt, "_iob"))) {
			_IAT__iob=__p__iob();
		}
		
		if (!(_IAT__pctype=(decltype(_IAT__pctype))GetProcAddress(hMsvcrt, "_pctype"))) {
			_IAT__pctype=__p__pctype();
		}
		
		if (!(_IAT___set_app_type=(decltype(_IAT___set_app_type))GetProcAddress(hMsvcrt, "__set_app_type"))) {
			_IAT___set_app_type=_COMPAT___set_app_type;
		}
	}
	
	if (HMODULE hKernel32=GetModuleHandle("kernel32.dll")) {
		if (!(_IAT_IsDBCSLeadByteEx=(decltype(_IAT_IsDBCSLeadByteEx))GetProcAddress(hKernel32, "IsDBCSLeadByteEx"))) {
			_IAT_IsDBCSLeadByteEx=_COMPAT_IsDBCSLeadByteEx;
		}
	}
		
	OriginalCRTStarup();
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

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

//Defines not found in MinGW's windows.h:
#define SM_FUNDAMENTALS		0x2004
#define VER_SERVER_NT 		0x80000000
#define VER_WORKSTATION_NT 	0x40000000
#define VER_SUITE_WH_SERVER	0x00008000
#define VER_PLATFORM_WIN32S	0

typedef void (WINAPI *pGetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);

void GuessOS(DWORD PlatformId, DWORD MajorVersion, DWORD MinorVersion, DWORD BuildNumber, char* CSDVersion, WORD ProductType, WORD SuiteMask, bool SERVERR2, bool MEDIACENTER, bool FUNDAMENTALS, bool AMD64);

int main(int argc, char* argv[])
{
	OSVERSIONINFO VerInf={sizeof(OSVERSIONINFO)};
	OSVERSIONINFOEX VerInfEx={sizeof(OSVERSIONINFOEX)};
	SYSTEM_INFO SysInf={};
	char* CSDVersion=NULL;
	
	if (GetVersionEx((LPOSVERSIONINFO)&VerInfEx)) {
		CSDVersion=VerInfEx.szCSDVersion;
	} else if (GetVersionEx(&VerInf)) {
		VerInfEx.dwPlatformId=VerInf.dwPlatformId;
		VerInfEx.dwMajorVersion=VerInf.dwMajorVersion;
		VerInfEx.dwMinorVersion=VerInfEx.dwMinorVersion;
		VerInfEx.dwBuildNumber=VerInf.dwBuildNumber;
		CSDVersion=VerInf.szCSDVersion;
		VerInfEx.wProductType=0;
		VerInfEx.wSuiteMask=0;
	}
	
	pGetNativeSystemInfo fnGetNativeSystemInfo;
	if (fnGetNativeSystemInfo=(pGetNativeSystemInfo)GetProcAddress(GetModuleHandle("kernel32.dll"), "GetNativeSystemInfo")) {
		fnGetNativeSystemInfo(&SysInf);
	} else {
		GetSystemInfo(&SysInf);
	}
	
	GuessOS(VerInfEx.dwPlatformId, VerInfEx.dwMajorVersion, VerInfEx.dwMinorVersion, VerInfEx.dwBuildNumber&0xFFFF, CSDVersion, VerInfEx.wProductType, VerInfEx.wSuiteMask, GetSystemMetrics(SM_SERVERR2), GetSystemMetrics(SM_MEDIACENTER), GetSystemMetrics(SM_FUNDAMENTALS), SysInf.wProcessorArchitecture&PROCESSOR_ARCHITECTURE_AMD64); 
	
	printf("\n");
    
	printf("Press ANY KEY to continue...\n");
	int c;
	c=getch();
	if (c==0||c==224) getch();
	return 0;
}

void GuessOS(DWORD PlatformId, DWORD MajorVersion, DWORD MinorVersion, DWORD BuildNumber, char* CSDVersion, WORD ProductType, WORD SuiteMask, bool SERVERR2, bool MEDIACENTER, bool FUNDAMENTALS, bool AMD64)
{
	switch (PlatformId) {
		case VER_PLATFORM_WIN32s: 		
			printf("Windows %u.%u", MajorVersion, MinorVersion);
			break;
		case VER_PLATFORM_WIN32_WINDOWS:
			switch (MinorVersion) {
				case 0:
					if (!strcmp(CSDVersion, " A"))
						printf("Windows 95 OSR1");
					else if (!strcmp(CSDVersion, " B")||!strcmp(CSDVersion, " C"))
						printf("Windows 95 OSR2");
					else
						printf("Windows 95");
					break;
				case 10:
					if (BuildNumber>1998)
						printf("Windows 98 SE");
					else
						printf("Windows 98");
					break;
				case 90:
					printf("Windows Me");
					break;
			}
			break;
		case VER_PLATFORM_WIN32_NT:
			switch (MajorVersion) {
				case 5:
					switch (MinorVersion) {
						case 0:
							printf("Windows 2000 %s", CSDVersion);
							break;
						case 1:
							if (MEDIACENTER)
								printf("Windows XP MCE %s", CSDVersion);
							else if (FUNDAMENTALS)
								printf("Windows FLP %s", CSDVersion);
							else
								printf("Windows XP %s", CSDVersion);
							break;
						case 2:
							if ((ProductType==VER_NT_WORKSTATION)&&AMD64) {
								printf("Windows XP x64 %s", CSDVersion);
							} else if (SuiteMask&VER_SUITE_WH_SERVER) {
								printf("Windows Home Server %s %s", CSDVersion, AMD64?"(64-bit)":"");
							} else if (SERVERR2) {
								printf("Windows Server 2003 R2 %s %s", CSDVersion, AMD64?"(64-bit)":"");
							} else {
								printf("Windows Server 2003 %s %s", CSDVersion, AMD64?"(64-bit)":"");
							}
							break;
					}
					break;
				case 6:
					switch (MinorVersion) {
						case 0:
							if (ProductType==VER_NT_WORKSTATION) {
								printf("Windows Vista %s %s", CSDVersion, AMD64?"(64-bit)":"");
							} else {
								printf("Windows Server 2008 %s %s", CSDVersion, AMD64?"(64-bit)":"");
							}
							break;
						case 1:
							//All Windows Servers based on 2008 R2 (and higher) are 64-bit only
							if (ProductType==VER_NT_WORKSTATION) {
								printf("Windows 7 %s %s", CSDVersion, AMD64?"(64-bit)":"");
							} else if (SuiteMask&VER_SUITE_WH_SERVER) {
								printf("Windows Home Server 2011 %s (64-bit)", CSDVersion);
							} else {
								printf("Windows Server 2008 R2 %s (64-bit)", CSDVersion);
							}
							break;
						case 2:
							if (ProductType==VER_NT_WORKSTATION) {
								printf("Windows 8 %s %s", CSDVersion, AMD64?"(64-bit)":"");
							} else {
								printf("Windows Server 2012 %s (64-bit)", CSDVersion);
							}
							break;
						case 3:
							//Proper manifest should be present for Windows 8.1 (and higher) checks to work
							if (ProductType==VER_NT_WORKSTATION) {
								printf("Windows 8.1 %s %s", CSDVersion, AMD64?"(64-bit)":"");
							} else {
								printf("Windows Server 2012 R2 %s (64-bit)", CSDVersion);
							}
							break;
					}
					break;
				case 10:
					switch (MinorVersion) {
						case 0:
							if (ProductType==VER_NT_WORKSTATION) {
								printf("Windows 10 %s %s", CSDVersion, AMD64?"(64-bit)":"");
							} else {
								printf("Windows Server 2016 Technical Preview %s (64-bit)", CSDVersion);
							}
							break;
					}
					break;
				default:
					if (MajorVersion<=4)
						printf("Windows NT %u.%u %s", MajorVersion, MinorVersion, CSDVersion);
			}
   }
}

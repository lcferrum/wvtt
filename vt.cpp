#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>
#include <initializer_list>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>

#define LABELED_VALUES_ARG(...)	false, TEXT(#__VA_ARGS__), {__VA_ARGS__}
#define UNIQUE_LABELED_VALUES_ARG(...)	true, TEXT(#__VA_ARGS__), {__VA_ARGS__}
template <typename ValT>
class BasicLabeledValues {
#ifdef UNICODE
	typedef std::wstring TSTRING;
	typedef std::wstringstream TSTRINGSTREAM;
#else
	typedef std::string TSTRING;
	typedef std::stringstream TSTRINGSTREAM;
#endif
private:
	std::vector<std::pair<TSTRING, ValT>> ValList;
public:
	BasicLabeledValues(bool unique_vals, const TCHAR* vals_str, const std::initializer_list<ValT> &vals): ValList() {
		TSTRINGSTREAM vals_ss(vals_str);
		TSTRING value;
		typename std::initializer_list<ValT>::iterator vals_it=vals.begin();
		while (std::getline(vals_ss, value, TEXT(','))&&vals_it!=vals.end()) {
			value.erase(std::remove_if(value.begin(), value.end(), [](TCHAR ch){ return std::isspace<TCHAR>(ch, std::locale::classic()); }), value.end());
			ValList.push_back(std::make_pair(value, *vals_it));
			vals_it++;
		}
		if (unique_vals)
			ValList.erase(std::unique(ValList.begin(), ValList.end(), [](const std::pair<TSTRING, ValT> &L, const std::pair<TSTRING, ValT> &R){
				return L.second==R.second;
			}), ValList.end());
	}
	void operator()(std::function<void(const TSTRING&, ValT)> enum_function) {
		for (std::pair<TSTRING, ValT> &val_pair: ValList)
			enum_function(val_pair.first, val_pair.second);
	}
	void Enums(ValT enums, std::function<void(const TSTRING&, ValT, bool)> enum_function) {
		bool found=false;
		for (std::pair<TSTRING, ValT> &val_pair: ValList) {
			if (enums==val_pair.second) {
				found=true;
				enum_function(val_pair.first, val_pair.second, false);
			}
		}
		if (!found)
			enum_function(TEXT(""), enums, true);
	}
	void Flags(ValT flags, std::function<void(const TSTRING&, ValT, bool)> enum_function) {
		ValT checked_flags=0;
		for (std::pair<TSTRING, ValT> &val_pair: ValList) {
			if ((flags&val_pair.second)==val_pair.second) {
				checked_flags|=val_pair.second;
				enum_function(val_pair.first, val_pair.second, false);
			}
		}
		if (checked_flags!=flags) {
			flags&=~checked_flags;
			enum_function(TEXT(""), flags, true);
		}
	}
};
typedef BasicLabeledValues<DWORD> LabeledValues;

//Defines not found in MinGW's windows.h/winnt.h:
#define SM_WEPOS			0x2003
#define SM_FUNDAMENTALS		0x2004

#define VER_MINORVERSION	0x0000001
#define VER_MAJORVERSION	0x0000002
#define VER_PLATFORMID		0x0000008
#define VER_EQUAL			1
#define VER_GREATER_EQUAL	3

#define VER_WORKSTATION_NT                  0x40000000
#define VER_SERVER_NT                       0x80000000
#define VER_SUITE_COMMUNICATIONS            0x00000008
#define VER_SUITE_EMBEDDED_RESTRICTED       0x00000800
#define VER_SUITE_SECURITY_APPLIANCE        0x00001000
#define VER_SUITE_WH_SERVER                 0x00008000

#define PROCESSOR_ARCHITECTURE_NEUTRAL	11

#define HKEY_PERFORMANCE_NLSTEXT    (( HKEY ) (ULONG_PTR)((LONG)0x80000060) )
#define HKEY_PERFORMANCE_TEXT       (( HKEY ) (ULONG_PTR)((LONG)0x80000050) )

#define PRODUCT_UNDEFINED                       0x00000000
#define PRODUCT_ULTIMATE                        0x00000001
#define PRODUCT_HOME_BASIC                      0x00000002
#define PRODUCT_HOME_PREMIUM                    0x00000003
#define PRODUCT_ENTERPRISE                      0x00000004
#define PRODUCT_HOME_BASIC_N                    0x00000005
#define PRODUCT_BUSINESS                        0x00000006
#define PRODUCT_STANDARD_SERVER                 0x00000007
#define PRODUCT_DATACENTER_SERVER               0x00000008
#define PRODUCT_SMALLBUSINESS_SERVER            0x00000009
#define PRODUCT_ENTERPRISE_SERVER               0x0000000A
#define PRODUCT_STARTER                         0x0000000B
#define PRODUCT_DATACENTER_SERVER_CORE          0x0000000C
#define PRODUCT_STANDARD_SERVER_CORE            0x0000000D
#define PRODUCT_ENTERPRISE_SERVER_CORE          0x0000000E
#define PRODUCT_ENTERPRISE_SERVER_IA64          0x0000000F
#define PRODUCT_BUSINESS_N                      0x00000010
#define PRODUCT_WEB_SERVER                      0x00000011
#define PRODUCT_CLUSTER_SERVER                  0x00000012
#define PRODUCT_HOME_SERVER                     0x00000013
#define PRODUCT_STORAGE_EXPRESS_SERVER          0x00000014
#define PRODUCT_STORAGE_STANDARD_SERVER         0x00000015
#define PRODUCT_STORAGE_WORKGROUP_SERVER        0x00000016
#define PRODUCT_STORAGE_ENTERPRISE_SERVER       0x00000017
#define PRODUCT_SERVER_FOR_SMALLBUSINESS        0x00000018
#define PRODUCT_SMALLBUSINESS_SERVER_PREMIUM    0x00000019
#define PRODUCT_UNLICENSED                      0xABCDABCD
#define PRODUCT_HOME_PREMIUM_N                      0x0000001A
#define PRODUCT_ENTERPRISE_N                        0x0000001B
#define PRODUCT_ULTIMATE_N                          0x0000001C
#define PRODUCT_WEB_SERVER_CORE                     0x0000001D
#define PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT    0x0000001E
#define PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY      0x0000001F
#define PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING     0x00000020
#define PRODUCT_SERVER_FOUNDATION                   0x00000021
#define PRODUCT_HOME_PREMIUM_SERVER                 0x00000022
#define PRODUCT_SERVER_FOR_SMALLBUSINESS_V          0x00000023
#define PRODUCT_STANDARD_SERVER_V                   0x00000024
#define PRODUCT_DATACENTER_SERVER_V                 0x00000025
#define PRODUCT_ENTERPRISE_SERVER_V                 0x00000026
#define PRODUCT_DATACENTER_SERVER_CORE_V            0x00000027
#define PRODUCT_STANDARD_SERVER_CORE_V              0x00000028
#define PRODUCT_ENTERPRISE_SERVER_CORE_V            0x00000029
#define PRODUCT_HYPERV                              0x0000002A
#define PRODUCT_STORAGE_EXPRESS_SERVER_CORE         0x0000002B
#define PRODUCT_STORAGE_STANDARD_SERVER_CORE        0x0000002C
#define PRODUCT_STORAGE_WORKGROUP_SERVER_CORE       0x0000002D
#define PRODUCT_STORAGE_ENTERPRISE_SERVER_CORE      0x0000002E
#define PRODUCT_STARTER_N                           0x0000002F
#define PRODUCT_PROFESSIONAL                        0x00000030
#define PRODUCT_PROFESSIONAL_N                      0x00000031
#define PRODUCT_SB_SOLUTION_SERVER                  0x00000032
#define PRODUCT_SERVER_FOR_SB_SOLUTIONS             0x00000033
#define PRODUCT_STANDARD_SERVER_SOLUTIONS           0x00000034
#define PRODUCT_STANDARD_SERVER_SOLUTIONS_CORE      0x00000035
#define PRODUCT_SB_SOLUTION_SERVER_EM               0x00000036
#define PRODUCT_SERVER_FOR_SB_SOLUTIONS_EM          0x00000037
#define PRODUCT_SOLUTION_EMBEDDEDSERVER             0x00000038
#define PRODUCT_SOLUTION_EMBEDDEDSERVER_CORE        0x00000039
#define PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE   0x0000003F
#define PRODUCT_ESSENTIALBUSINESS_SERVER_MGMT       0x0000003B
#define PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL       0x0000003C
#define PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC    0x0000003D
#define PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC    0x0000003E
#define PRODUCT_CLUSTER_SERVER_V                    0x00000040
#define PRODUCT_EMBEDDED                            0x00000041
#define PRODUCT_STARTER_E                           0x00000042
#define PRODUCT_HOME_BASIC_E                        0x00000043
#define PRODUCT_HOME_PREMIUM_E                      0x00000044
#define PRODUCT_PROFESSIONAL_E                      0x00000045
#define PRODUCT_ENTERPRISE_E                        0x00000046
#define PRODUCT_ULTIMATE_E                          0x00000047

typedef void (WINAPI *pGetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);
typedef ULONGLONG (WINAPI *pVerSetConditionMask)(ULONGLONG dwlConditionMask, DWORD dwTypeBitMask, BYTE dwConditionMask);
typedef BOOL (WINAPI *pVerifyVersionInfo)(LPOSVERSIONINFOEX lpVersionInfo, DWORD dwTypeMask, DWORDLONG dwlConditionMask);
typedef BOOL (WINAPI *pIsWow64Process)(HANDLE hProcess, PBOOL Wow64Process);
typedef BOOL (WINAPI *pGetProductInfo)(DWORD dwOSMajorVersion, DWORD dwOSMinorVersion, DWORD dwSpMajorVersion, DWORD dwSpMinorVersion, PDWORD pdwReturnedProductType);
typedef const char* (CDECL *pwine_get_version)(void);

bool PrintRegistryKey(HKEY hive, const char* keypath, const char* value);
bool PrintFileVersion(const char* fpath);

int main(int argc, char* argv[])
{
	OSVERSIONINFO VerInf={sizeof(OSVERSIONINFO)};
	OSVERSIONINFOEX VerInfEx={sizeof(OSVERSIONINFOEX)};
	SYSTEM_INFO SysInf={};
	
	LabeledValues PlatfromIds(LABELED_VALUES_ARG(VER_PLATFORM_WIN32s, VER_PLATFORM_WIN32_WINDOWS, VER_PLATFORM_WIN32_NT));
	LabeledValues SuiteMasks(LABELED_VALUES_ARG(VER_WORKSTATION_NT, VER_SERVER_NT, VER_SUITE_SMALLBUSINESS, VER_SUITE_ENTERPRISE, VER_SUITE_BACKOFFICE, VER_SUITE_COMMUNICATIONS, VER_SUITE_TERMINAL, VER_SUITE_SMALLBUSINESS_RESTRICTED, VER_SUITE_EMBEDDEDNT, VER_SUITE_DATACENTER, VER_SUITE_SINGLEUSERTS, VER_SUITE_PERSONAL, VER_SUITE_BLADE, VER_SUITE_EMBEDDED_RESTRICTED, VER_SUITE_SECURITY_APPLIANCE, VER_SUITE_STORAGE_SERVER, VER_SUITE_COMPUTE_SERVER, VER_SUITE_WH_SERVER));
	LabeledValues ProductTypes(LABELED_VALUES_ARG(VER_NT_WORKSTATION, VER_NT_DOMAIN_CONTROLLER, VER_NT_SERVER));
	LabeledValues SystemMetrics(LABELED_VALUES_ARG(SM_FUNDAMENTALS, SM_WEPOS, SM_DEBUG, SM_PENWINDOWS, SM_DBCSENABLED, SM_IMMENABLED, SM_SLOWMACHINE, SM_TABLETPC, SM_MEDIACENTER, SM_STARTER, SM_SERVERR2));
	LabeledValues ProcessorArchitectures(LABELED_VALUES_ARG(PROCESSOR_ARCHITECTURE_INTEL, PROCESSOR_ARCHITECTURE_MIPS, PROCESSOR_ARCHITECTURE_ALPHA, PROCESSOR_ARCHITECTURE_PPC, PROCESSOR_ARCHITECTURE_SHX, PROCESSOR_ARCHITECTURE_ARM, PROCESSOR_ARCHITECTURE_IA64, PROCESSOR_ARCHITECTURE_ALPHA64, PROCESSOR_ARCHITECTURE_MSIL, PROCESSOR_ARCHITECTURE_AMD64, PROCESSOR_ARCHITECTURE_IA32_ON_WIN64, PROCESSOR_ARCHITECTURE_NEUTRAL, PROCESSOR_ARCHITECTURE_UNKNOWN));
	LabeledValues ProductInfoTypes(LABELED_VALUES_ARG(PRODUCT_UNDEFINED, PRODUCT_ULTIMATE, PRODUCT_HOME_BASIC, PRODUCT_HOME_PREMIUM, PRODUCT_ENTERPRISE, PRODUCT_HOME_BASIC_N, PRODUCT_BUSINESS, PRODUCT_STANDARD_SERVER, PRODUCT_DATACENTER_SERVER, PRODUCT_SMALLBUSINESS_SERVER, PRODUCT_ENTERPRISE_SERVER, PRODUCT_STARTER, PRODUCT_DATACENTER_SERVER_CORE, PRODUCT_STANDARD_SERVER_CORE, PRODUCT_ENTERPRISE_SERVER_CORE, PRODUCT_ENTERPRISE_SERVER_IA64, PRODUCT_BUSINESS_N, PRODUCT_WEB_SERVER, PRODUCT_CLUSTER_SERVER, PRODUCT_HOME_SERVER, PRODUCT_STORAGE_EXPRESS_SERVER, PRODUCT_STORAGE_STANDARD_SERVER, PRODUCT_STORAGE_WORKGROUP_SERVER, PRODUCT_STORAGE_ENTERPRISE_SERVER, PRODUCT_SERVER_FOR_SMALLBUSINESS, PRODUCT_SMALLBUSINESS_SERVER_PREMIUM, PRODUCT_UNLICENSED, PRODUCT_HOME_PREMIUM_N, PRODUCT_ENTERPRISE_N, PRODUCT_ULTIMATE_N, PRODUCT_WEB_SERVER_CORE, PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT, PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY, PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING, PRODUCT_SERVER_FOUNDATION, PRODUCT_HOME_PREMIUM_SERVER, PRODUCT_SERVER_FOR_SMALLBUSINESS_V, PRODUCT_STANDARD_SERVER_V, PRODUCT_DATACENTER_SERVER_V, PRODUCT_ENTERPRISE_SERVER_V, PRODUCT_DATACENTER_SERVER_CORE_V, PRODUCT_STANDARD_SERVER_CORE_V, PRODUCT_ENTERPRISE_SERVER_CORE_V, PRODUCT_HYPERV, PRODUCT_STORAGE_EXPRESS_SERVER_CORE, PRODUCT_STORAGE_STANDARD_SERVER_CORE, PRODUCT_STORAGE_WORKGROUP_SERVER_CORE, PRODUCT_STORAGE_ENTERPRISE_SERVER_CORE, PRODUCT_STARTER_N, PRODUCT_PROFESSIONAL, PRODUCT_PROFESSIONAL_N, PRODUCT_SB_SOLUTION_SERVER, PRODUCT_SERVER_FOR_SB_SOLUTIONS, PRODUCT_STANDARD_SERVER_SOLUTIONS, PRODUCT_STANDARD_SERVER_SOLUTIONS_CORE, PRODUCT_SB_SOLUTION_SERVER_EM, PRODUCT_SERVER_FOR_SB_SOLUTIONS_EM, PRODUCT_SOLUTION_EMBEDDEDSERVER, PRODUCT_SOLUTION_EMBEDDEDSERVER_CORE, PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE, PRODUCT_ESSENTIALBUSINESS_SERVER_MGMT, PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL, PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC, PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC, PRODUCT_CLUSTER_SERVER_V, PRODUCT_EMBEDDED, PRODUCT_STARTER_E, PRODUCT_HOME_BASIC_E, PRODUCT_HOME_PREMIUM_E, PRODUCT_PROFESSIONAL_E, PRODUCT_ENTERPRISE_E, PRODUCT_ULTIMATE_E));
	
	if (GetVersionEx((LPOSVERSIONINFO)&VerInfEx)) {
		printf("OSVERSIONINFOEX:\n");
		printf("dwPlatformId = 0x%08x\n", VerInfEx.dwPlatformId);
		PlatfromIds.Enums(VerInfEx.dwPlatformId, [](const std::string& label, DWORD value, bool unknown){
			if (unknown)
				printf("\tUNKNOWN VALUE (0x%08x)\n", value);
			else
				printf("\t%s\n", label.c_str());
		});
		printf("dwMajorVersion = %u\n", VerInfEx.dwMajorVersion);
		printf("dwMinorVersion = %u\n", VerInfEx.dwMinorVersion);
		printf("dwBuildNumber = %u = %u.%u.%u\n", VerInfEx.dwBuildNumber, HIBYTE(HIWORD(VerInfEx.dwBuildNumber)), LOBYTE(HIWORD(VerInfEx.dwBuildNumber)), LOWORD(VerInfEx.dwBuildNumber));
		printf("szCSDVersion = \"%s\"\n", VerInfEx.szCSDVersion);
		printf("wServicePackMajor = %u\n", VerInfEx.wServicePackMajor);
		printf("wServicePackMinor = %u\n", VerInfEx.wServicePackMinor);
		printf("wSuiteMask = 0x%04x\n", VerInfEx.wSuiteMask);
		SuiteMasks.Flags(VerInfEx.wSuiteMask, [](const std::string& label, DWORD value, bool unknown){
			if (unknown)
				printf("\tUNKNOWN FLAG (0x%04x)\n", value);
			else
				printf("\t%s\n", label.c_str());
		});
		printf("wProductType = 0x%04x\n", VerInfEx.wProductType);
		ProductTypes.Enums(VerInfEx.wProductType, [](const std::string& label, DWORD value, bool unknown){
			if (unknown)
				printf("\tUNKNOWN VALUE (0x%04x)\n", value);
			else
				printf("\t%s\n", label.c_str());
		});
		printf("wReserved = 0x%04x\n", VerInfEx.wReserved);
	} else if (GetVersionEx(&VerInf)) {
		printf("OSVERSIONINFO:\n");
		printf("dwPlatformId = 0x%08x\n", VerInf.dwPlatformId);
		PlatfromIds.Enums(VerInf.dwPlatformId, [](const std::string& label, DWORD value, bool unknown){
			if (unknown)
				printf("\tUNKNOWN VALUE (0x%08x)\n", value);
			else
				printf("\t%s\n", label.c_str());
		});
		printf("dwMajorVersion = %u\n", VerInf.dwMajorVersion);
		printf("dwMinorVersion = %u\n", VerInf.dwMinorVersion);
		printf("dwBuildNumber = %u = %u.%u.%u\n", VerInf.dwBuildNumber, HIBYTE(HIWORD(VerInf.dwBuildNumber)), LOBYTE(HIWORD(VerInf.dwBuildNumber)), LOWORD(VerInf.dwBuildNumber));
		printf("szCSDVersion = \"%s\"\n", VerInf.szCSDVersion);
	} else {
		printf("GetVersionEx FAILED!\n");
	}
	
	printf("\n");
	
	printf("GetSystemMetrics:\n");
	SystemMetrics([](const std::string& label, DWORD value){
		if (GetSystemMetrics(value))
			printf("\t%s\n", label.c_str());
	});
	
	printf("\n");
	
	pGetNativeSystemInfo fnGetNativeSystemInfo;
	if ((fnGetNativeSystemInfo=(pGetNativeSystemInfo)GetProcAddress(GetModuleHandle("kernel32.dll"), "GetNativeSystemInfo"))) {
		fnGetNativeSystemInfo(&SysInf);
		printf("GetNativeSystemInfo.wProcessorArchitecture = 0x%04x\n", SysInf.wProcessorArchitecture);
	} else {
		GetSystemInfo(&SysInf);
		printf("GetSystemInfo.wProcessorArchitecture = 0x%04x\n", SysInf.wProcessorArchitecture);
	}
	ProcessorArchitectures.Enums(SysInf.wProcessorArchitecture, [](const std::string& label, DWORD value, bool unknown){
		if (unknown)
			printf("\tUNKNOWN VALUE (0x%04x)\n", value);
		else
			printf("\t%s\n", label.c_str());
	});
	
	printf("\n");
	
	pGetProductInfo fnGetProductInfo;
	if ((fnGetProductInfo=(pGetProductInfo)GetProcAddress(GetModuleHandle("kernel32.dll"), "GetProductInfo"))) {
		DWORD dwProdType;
		if (fnGetProductInfo(VerInfEx.dwMajorVersion, VerInfEx.dwMinorVersion, VerInfEx.wServicePackMajor, VerInfEx.wServicePackMinor, &dwProdType)) {
			printf("GetProductInfo = 0x%08x\n", dwProdType);
			ProductInfoTypes.Enums(dwProdType, [](const std::string& label, DWORD value, bool unknown){
				if (unknown)
					printf("\tUNKNOWN VALUE (0x%08x)\n", value);
				else
					printf("\t%s\n", label.c_str());
			});
		} else
			printf("GetProductInfo failed!\n");
	} else
		printf("Can't load GetProductInfo from kernel32.dll!\n");
	
	printf("\n");
	
	pVerSetConditionMask fnVerSetConditionMask;
	pVerifyVersionInfo fnVerifyVersionInfo;
	if ((fnVerSetConditionMask=(pVerSetConditionMask)GetProcAddress(GetModuleHandle("kernel32.dll"), "VerSetConditionMask"))&&
		(fnVerifyVersionInfo=(pVerifyVersionInfo)GetProcAddress(GetModuleHandle("kernel32.dll"), "VerifyVersionInfoA"))) {
		
		//Test for Windows 8.1 (and higher) that works without manifest
		OSVERSIONINFOEX VerTest={sizeof(OSVERSIONINFOEX)};
		VerTest.dwMajorVersion=6;
		VerTest.dwMinorVersion=3;
		VerTest.dwPlatformId=VER_PLATFORM_WIN32_NT;
		printf("VerifyVersionInfo(WIN32_NT >=6.3) = %s\n", 
			fnVerifyVersionInfo(&VerTest, VER_MAJORVERSION|VER_MINORVERSION|VER_PLATFORMID,	fnVerSetConditionMask(fnVerSetConditionMask(fnVerSetConditionMask(0, VER_PLATFORMID, VER_EQUAL), VER_MAJORVERSION, VER_GREATER_EQUAL), VER_MINORVERSION, VER_GREATER_EQUAL))
			?"TRUE":"FALSE");
	} else
		printf("Can't load VerSetConditionMask and VerifyVersionInfo from kernel32.dll!\n");
	
	pIsWow64Process fnIsWow64Process;
	if ((fnIsWow64Process=(pIsWow64Process)GetProcAddress(GetModuleHandle("kernel32.dll"), "IsWow64Process"))) {	
		//Test if current process is running under WOW64
		BOOL wow64=FALSE;
		fnIsWow64Process(GetCurrentProcess(), &wow64);
		printf("IsWow64Process() = %s\n", wow64?"TRUE":"FALSE");
	} else
		printf("Can't load IsWow64Process from kernel32.dll!\n");
	
	pwine_get_version fnwine_get_version;
	if ((fnwine_get_version=(pwine_get_version)GetProcAddress(GetModuleHandle("ntdll.dll"), "wine_get_version"))) {
		//Test for Wine
		printf("wine_get_version() = %s\n", fnwine_get_version());
	} else {
		printf("Can't load wine_get_version from ntdll.dll!\n");
	}
	
	printf("\n");
	
	PrintRegistryKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ProductName");
	PrintRegistryKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "BuildLab");
	PrintRegistryKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "BuildLabEx");
	PrintRegistryKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "CSDBuildNumber");
	PrintRegistryKey(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\ProductOptions", "ProductType");
	PrintRegistryKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion", "ProductName");
	
	printf("\n");
	
	PrintFileVersion("USER.EXE");
	PrintFileVersion("NTKERN.VXD");
	
	printf("\n");
    
	printf("Press ANY KEY to continue...\n");
	int c;
	c=getch();
	if (c==0||c==224) getch();
	return 0;
}

bool PrintRegistryKey(HKEY hive, const char* keypath, const char* value) {
	HKEY RegKey;
	DWORD buflen;
	bool success=false;
	
	char disp_path[strlen(keypath)+strlen(value)+7];
	switch ((DWORD)hive) {
		case (DWORD)HKEY_CLASSES_ROOT:
			strcpy(disp_path, "HKCR\\");
			break;
		case (DWORD)HKEY_CURRENT_CONFIG:
			strcpy(disp_path, "HKCC\\");
			break;
		case (DWORD)HKEY_CURRENT_USER:
			strcpy(disp_path, "HKCU\\");
			break;
		case (DWORD)HKEY_LOCAL_MACHINE:
			strcpy(disp_path, "HKLM\\");
			break;
		case (DWORD)HKEY_PERFORMANCE_DATA:
			strcpy(disp_path, "HKPD\\");
			break;
		case (DWORD)HKEY_PERFORMANCE_NLSTEXT:
			strcpy(disp_path, "HKPN\\");
			break;
		case (DWORD)HKEY_PERFORMANCE_TEXT:
			strcpy(disp_path, "HKPT\\");
			break;
		case (DWORD)HKEY_USERS:
			strcpy(disp_path, "HKU\\");
			break;
		default:
			strcpy(disp_path, "?\\");
	}
	
	strcat(disp_path, keypath);
	strcat(disp_path, "\\");
	strcat(disp_path, value);
	
	if (RegOpenKeyEx(hive, keypath, 0, KEY_READ, &RegKey)==ERROR_SUCCESS) {
		if (RegQueryValueEx(RegKey, value, 0, NULL, NULL, &buflen)==ERROR_SUCCESS) {
			char retbuf[buflen];
			if (RegQueryValueEx(RegKey, value, 0, NULL, (LPBYTE)retbuf, &buflen)==ERROR_SUCCESS) {
				printf("%s =\n\t\"%s\"\n", disp_path, retbuf);
				success=true;
			}
		}
		RegCloseKey(RegKey);
	}
	
	if (!success) printf("%s - error while opening!\n", disp_path);
	return success;
}

bool PrintFileVersion(const char* fpath) {
	DWORD buflen;
	char crdate[11]="?";
	
	if ((buflen=SearchPath(NULL, fpath, NULL, 0, NULL, NULL))) {
		char fullpth[buflen];
		if (SearchPath(NULL, fpath, NULL, buflen, fullpth, NULL)) {
			HANDLE hFile=CreateFile(fullpth, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			if (hFile!=INVALID_HANDLE_VALUE) {
				FILETIME ftModified;
				SYSTEMTIME stUTC;
				if (GetFileTime(hFile, NULL, NULL, &ftModified)&&FileTimeToSystemTime(&ftModified, &stUTC)) {
					sprintf(crdate, "%02d/%02d/%04d", stUTC.wDay, stUTC.wMonth, stUTC.wYear);
				}
				CloseHandle(hFile); 
			}

			if ((buflen=GetFileVersionInfoSize(fullpth, NULL))) {
				BYTE retbuf[buflen];
				if (GetFileVersionInfo(fullpth, 0, buflen, (LPVOID)retbuf)) {
					VS_FIXEDFILEINFO *pfinfo;
					UINT finfolen;
					if (VerQueryValue((LPVOID)retbuf, "\\", (LPVOID*)&pfinfo, &finfolen)) {
						printf("%s (%s) VS_FIXEDFILEINFO.FileVersion = %d.%d.%d.%d\n", fpath, crdate, HIWORD(pfinfo->dwFileVersionMS), LOWORD(pfinfo->dwFileVersionMS), HIWORD(pfinfo->dwFileVersionLS), LOWORD(pfinfo->dwFileVersionLS));
						return true;
					}
				}
			}
		}
	}		
	
	printf("%s - error while opening!\n", fpath);
	return false;
}
#include <iostream>
#include <iomanip>
#include <limits>
#include <cstddef>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <ntdef.h>
#include <windows.h>
#include "externs.h"
#include "labeled_values.hpp"

extern pRtlGetVersion fnRtlGetVersion;
extern pGetNativeSystemInfo fnGetNativeSystemInfo;
extern pIsWow64Process fnIsWow64Process;
extern pGetProductInfo fnGetProductInfo;
extern pwine_get_version fnwine_get_version;

//Defines not found in MinGW's windows.h/winnt.h:
#define SM_WEPOS			0x2003
#define SM_FUNDAMENTALS		0x2004

//std::cout treats char as, obvously, char and prints it as such - not it's numerical value
//If numerical value is needed, instead of static cast to ULONG_PTR unary addition operator can be used to force printing numerical value
//Also, these defines only work correctly with specific std::ios::fmtflags and std::ios::fill (see below in main)
#define COUT_DEC(dec_int)				+dec_int
#define COUT_HEX(hex_int, hex_width)	"0x"<<std::hex<<std::setw(hex_width)<<+hex_int<<std::dec
#define COUT_BOOL(bool_val)				(bool_val?"TRUE":"FALSE")

LabeledValues PlatfromIds(LABELED_VALUES_ARG(VER_PLATFORM_WIN32s, VER_PLATFORM_WIN32_WINDOWS, VER_PLATFORM_WIN32_NT));
LabeledValues SuiteMasks(LABELED_VALUES_ARG(VER_WORKSTATION_NT, VER_SERVER_NT, VER_SUITE_SMALLBUSINESS, VER_SUITE_ENTERPRISE, VER_SUITE_BACKOFFICE, VER_SUITE_COMMUNICATIONS, VER_SUITE_TERMINAL, VER_SUITE_SMALLBUSINESS_RESTRICTED, VER_SUITE_EMBEDDEDNT, VER_SUITE_DATACENTER, VER_SUITE_SINGLEUSERTS, VER_SUITE_PERSONAL, VER_SUITE_BLADE, VER_SUITE_EMBEDDED_RESTRICTED, VER_SUITE_SECURITY_APPLIANCE, VER_SUITE_STORAGE_SERVER, VER_SUITE_COMPUTE_SERVER, VER_SUITE_WH_SERVER));
LabeledValues ProductTypes(LABELED_VALUES_ARG(VER_NT_WORKSTATION, VER_NT_DOMAIN_CONTROLLER, VER_NT_SERVER));
LabeledValues SystemMetrics(LABELED_VALUES_ARG(SM_FUNDAMENTALS, SM_WEPOS, SM_DEBUG, SM_PENWINDOWS, SM_DBCSENABLED, SM_IMMENABLED, SM_SLOWMACHINE, SM_TABLETPC, SM_MEDIACENTER, SM_STARTER, SM_SERVERR2));
LabeledValues ProcessorArchitectures(LABELED_VALUES_ARG(PROCESSOR_ARCHITECTURE_INTEL, PROCESSOR_ARCHITECTURE_MIPS, PROCESSOR_ARCHITECTURE_ALPHA, PROCESSOR_ARCHITECTURE_PPC, PROCESSOR_ARCHITECTURE_SHX, PROCESSOR_ARCHITECTURE_ARM, PROCESSOR_ARCHITECTURE_IA64, PROCESSOR_ARCHITECTURE_ALPHA64, PROCESSOR_ARCHITECTURE_MSIL, PROCESSOR_ARCHITECTURE_AMD64, PROCESSOR_ARCHITECTURE_IA32_ON_WIN64, PROCESSOR_ARCHITECTURE_NEUTRAL, PROCESSOR_ARCHITECTURE_UNKNOWN));
LabeledValues ProductInfoTypes(LABELED_VALUES_ARG(PRODUCT_UNDEFINED, PRODUCT_ULTIMATE, PRODUCT_HOME_BASIC, PRODUCT_HOME_PREMIUM, PRODUCT_ENTERPRISE, PRODUCT_HOME_BASIC_N, PRODUCT_BUSINESS, PRODUCT_STANDARD_SERVER, PRODUCT_DATACENTER_SERVER, PRODUCT_SMALLBUSINESS_SERVER, PRODUCT_ENTERPRISE_SERVER, PRODUCT_STARTER, PRODUCT_DATACENTER_SERVER_CORE, PRODUCT_STANDARD_SERVER_CORE, PRODUCT_ENTERPRISE_SERVER_CORE, PRODUCT_ENTERPRISE_SERVER_IA64, PRODUCT_BUSINESS_N, PRODUCT_WEB_SERVER, PRODUCT_CLUSTER_SERVER, PRODUCT_HOME_SERVER, PRODUCT_STORAGE_EXPRESS_SERVER, PRODUCT_STORAGE_STANDARD_SERVER, PRODUCT_STORAGE_WORKGROUP_SERVER, PRODUCT_STORAGE_ENTERPRISE_SERVER, PRODUCT_SERVER_FOR_SMALLBUSINESS, PRODUCT_SMALLBUSINESS_SERVER_PREMIUM, PRODUCT_UNLICENSED, PRODUCT_HOME_PREMIUM_N, PRODUCT_ENTERPRISE_N, PRODUCT_ULTIMATE_N, PRODUCT_WEB_SERVER_CORE, PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT, PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY, PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING, PRODUCT_SERVER_FOUNDATION, PRODUCT_HOME_PREMIUM_SERVER, PRODUCT_SERVER_FOR_SMALLBUSINESS_V, PRODUCT_STANDARD_SERVER_V, PRODUCT_DATACENTER_SERVER_V, PRODUCT_ENTERPRISE_SERVER_V, PRODUCT_DATACENTER_SERVER_CORE_V, PRODUCT_STANDARD_SERVER_CORE_V, PRODUCT_ENTERPRISE_SERVER_CORE_V, PRODUCT_HYPERV, PRODUCT_STORAGE_EXPRESS_SERVER_CORE, PRODUCT_STORAGE_STANDARD_SERVER_CORE, PRODUCT_STORAGE_WORKGROUP_SERVER_CORE, PRODUCT_STORAGE_ENTERPRISE_SERVER_CORE, PRODUCT_STARTER_N, PRODUCT_PROFESSIONAL, PRODUCT_PROFESSIONAL_N, PRODUCT_SB_SOLUTION_SERVER, PRODUCT_SERVER_FOR_SB_SOLUTIONS, PRODUCT_STANDARD_SERVER_SOLUTIONS, PRODUCT_STANDARD_SERVER_SOLUTIONS_CORE, PRODUCT_SB_SOLUTION_SERVER_EM, PRODUCT_SERVER_FOR_SB_SOLUTIONS_EM, PRODUCT_SOLUTION_EMBEDDEDSERVER, PRODUCT_SOLUTION_EMBEDDEDSERVER_CORE, PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE, PRODUCT_ESSENTIALBUSINESS_SERVER_MGMT, PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL, PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC, PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC, PRODUCT_CLUSTER_SERVER_V, PRODUCT_EMBEDDED, PRODUCT_STARTER_E, PRODUCT_HOME_BASIC_E, PRODUCT_HOME_PREMIUM_E, PRODUCT_PROFESSIONAL_E, PRODUCT_ENTERPRISE_E, PRODUCT_ULTIMATE_E));

bool PrintRegistryKey(HKEY hive, const char* keypath, const char* value);
bool PrintFileVersion(const char* fpath);
bool GetVersionWrapper(OSVERSIONINFOEX &osvi_ex);

int main(int argc, char* argv[])
{
	Externs::MakeInstance();
	
	//This sets fill character to '0' for all subsequent cout outputs 
	//It only matters when setw in non-zero (and it is reset to zero after every output) so it won't corrupt ordinary integer output
	std::cout.fill('0');

	//This prevents base from showing in hex/oct output and hex/oct output becomes uppercase
	//N.B.: Uppercase also makes base to be displayed in uppercase (that's why showing base is disabled) but doesn't affect boolalpha flag (bool values still displayed in lowercase)
	//Applied for all subsequent cout outputs 
	std::cout.unsetf(std::ios::showbase);
	std::cout.setf(std::ios::uppercase);   
	
	OSVERSIONINFOEX osvi_ex;
	if (GetVersionWrapper(osvi_ex)) {
		std::cout<<"dwPlatformId = "<<COUT_HEX(osvi_ex.dwPlatformId, 8)<<std::endl;
		PlatfromIds.Enums(osvi_ex.dwPlatformId, [](const std::string& label, DWORD value, bool unknown, size_t idx){
			if (unknown)
				std::cout<<"\tUNKNOWN VALUE ("<<COUT_HEX(value, 8)<<")"<<std::endl;
			else
				std::cout<<"\t"<<label<<std::endl;
			return false;
		});
		std::cout<<"dwMajorVersion = "<<COUT_DEC(osvi_ex.dwMajorVersion)<<std::endl;
		std::cout<<"dwMinorVersion = "<<COUT_DEC(osvi_ex.dwMinorVersion)<<std::endl;
		std::cout<<"dwBuildNumber = "<<COUT_DEC(osvi_ex.dwBuildNumber)<<" = "<<COUT_DEC(HIBYTE(HIWORD(osvi_ex.dwBuildNumber)))<<"."<<COUT_DEC(LOBYTE(HIWORD(osvi_ex.dwBuildNumber)))<<"."<<COUT_DEC((DWORD)LOWORD(osvi_ex.dwBuildNumber))<<std::endl;
		std::cout<<"szCSDVersion = \""<<COUT_DEC(osvi_ex.szCSDVersion)<<"\""<<std::endl;
		if (osvi_ex.dwOSVersionInfoSize==sizeof(OSVERSIONINFOEX)) {
			std::cout<<"wServicePackMajor = "<<COUT_DEC(osvi_ex.wServicePackMajor)<<std::endl;
			std::cout<<"wServicePackMinor = "<<COUT_DEC(osvi_ex.wServicePackMinor)<<std::endl;
			std::cout<<"wSuiteMask = "<<COUT_HEX(osvi_ex.wSuiteMask, 4)<<std::endl;
			SuiteMasks.Flags(osvi_ex.wSuiteMask, [](const std::string& label, DWORD value, bool unknown, size_t idx){
				if (unknown)
					std::cout<<"\tUNKNOWN FLAG ("<<COUT_HEX(value, 4)<<")"<<std::endl;
				else
					std::cout<<"\t"<<label<<std::endl;
				return false;
			});
			std::cout<<"wProductType = "<<COUT_HEX(osvi_ex.wProductType, 2)<<std::endl;
			ProductTypes.Enums(osvi_ex.wProductType, [](const std::string& label, DWORD value, bool unknown, size_t idx){
				if (unknown)
					std::cout<<"\tUNKNOWN VALUE ("<<COUT_HEX(value, 2)<<")"<<std::endl;
				else
					std::cout<<"\t"<<label<<std::endl;
				return false;
			});
			std::cout<<"wReserved = "<<COUT_HEX(osvi_ex.wReserved, 2)<<std::endl;
		}
	} else {
		std::cout<<"RtlGetVersion and GetVersionEx failed!"<<std::endl;
	}
	
	std::cout<<std::endl;
	
	std::cout<<"GetSystemMetrics:"<<std::endl;
	SystemMetrics.Values([](const std::string& label, DWORD value, size_t idx){
		if (GetSystemMetrics(value))
			std::cout<<"\t"<<label<<std::endl;
		return false;
	});
	
	std::cout<<std::endl;
	
	SYSTEM_INFO sys_inf={};
	if (fnGetNativeSystemInfo) {
		fnGetNativeSystemInfo(&sys_inf);
		std::cout<<"GetNativeSystemInfo";
	} else {
		GetSystemInfo(&sys_inf);
		std::cout<<"GetSystemInfo";
	}
	std::cout<<".wProcessorArchitecture = "<<COUT_HEX(sys_inf.wProcessorArchitecture, 4)<<std::endl;
	ProcessorArchitectures.Enums(sys_inf.wProcessorArchitecture, [](const std::string& label, DWORD value, bool unknown, size_t idx){
		if (unknown)
			std::cout<<"\tUNKNOWN VALUE ("<<COUT_HEX(value, 4)<<")"<<std::endl;
		else
			std::cout<<"\t"<<label<<std::endl;
		return false;
	});
	
	std::cout<<std::endl;
	
	if (fnGetProductInfo) {
		DWORD dwProdType;
		if (fnGetProductInfo(osvi_ex.dwMajorVersion, osvi_ex.dwMinorVersion, osvi_ex.wServicePackMajor, osvi_ex.wServicePackMinor, &dwProdType)) {
			std::cout<<"GetProductInfo = "<<COUT_HEX(dwProdType, 8)<<std::endl;
			ProductInfoTypes.Enums(dwProdType, [](const std::string& label, DWORD value, bool unknown, size_t idx){
				if (unknown)
					std::cout<<"\tUNKNOWN VALUE ("<<COUT_HEX(value, 8)<<")"<<std::endl;
				else
					std::cout<<"\t"<<label<<std::endl;
				return false;
			});
		} else
			std::cout<<"GetProductInfo failed!"<<std::endl;
	} else
		std::cout<<"Can't load GetProductInfo from kernel32.dll!"<<std::endl;
	
	std::cout<<std::endl;
	
#ifdef _WIN64
	std::cout<<"Current EXE build is x86-64"<<std::endl;
#else
	std::cout<<"Current EXE build is x86"<<std::endl;
#endif
	
	if (fnIsWow64Process) {	
		//Test if current process is running under WOW64
		BOOL wow64=FALSE;
		fnIsWow64Process(GetCurrentProcess(), &wow64);
		std::cout<<"IsWow64Process() = "<<COUT_BOOL(wow64)<<std::endl;
	} else
		std::cout<<"Can't load IsWow64Process from kernel32.dll!"<<std::endl;
	
	if (fnwine_get_version) {
		//Test for Wine
		std::cout<<"wine_get_version() = "<<fnwine_get_version()<<std::endl;
	} else {
		std::cout<<"Can't load wine_get_version from ntdll.dll!"<<std::endl;
	}
	
	std::cout<<std::endl;
	
	PrintRegistryKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ProductName");
	PrintRegistryKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "BuildLab");
	PrintRegistryKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "BuildLabEx");
	PrintRegistryKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "CSDBuildNumber");
	PrintRegistryKey(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\ProductOptions", "ProductType");
	PrintRegistryKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion", "ProductName");
	
	std::cout<<std::endl;
	
	PrintFileVersion("USER.EXE");
	PrintFileVersion("KERNEL32.DLL");
	PrintFileVersion("NTKERN.VXD");
	
	std::cout<<std::endl;
    
	std::cout<<"Press ENTER to continue..."<<std::flush;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');	//Needs defined NOMINMAX
	return 0;
}

bool GetVersionWrapper(OSVERSIONINFOEX &osvi_ex)
{
	if (fnRtlGetVersion) {
		RTL_OSVERSIONINFOEXW rtl_osvi_ex={sizeof(RTL_OSVERSIONINFOEXW)};
		if (NT_SUCCESS(fnRtlGetVersion((PRTL_OSVERSIONINFOW)&rtl_osvi_ex))||(rtl_osvi_ex.dwOSVersionInfoSize=sizeof(RTL_OSVERSIONINFOW), NT_SUCCESS(fnRtlGetVersion((PRTL_OSVERSIONINFOW)&rtl_osvi_ex)))) {
			if (WideCharToMultiByte(CP_ACP, 0, rtl_osvi_ex.szCSDVersion, sizeof(((RTL_OSVERSIONINFOEXW*)NULL)->szCSDVersion)/sizeof(wchar_t), osvi_ex.szCSDVersion, sizeof(((RTL_OSVERSIONINFOEXW*)NULL)->szCSDVersion), NULL, NULL)) {
				osvi_ex.dwMajorVersion=rtl_osvi_ex.dwMajorVersion;
				osvi_ex.dwMinorVersion=rtl_osvi_ex.dwMinorVersion;
				osvi_ex.dwBuildNumber=rtl_osvi_ex.dwBuildNumber;
				osvi_ex.dwPlatformId=rtl_osvi_ex.dwPlatformId;
				if (rtl_osvi_ex.dwOSVersionInfoSize==sizeof(RTL_OSVERSIONINFOEXW)) {
					std::cout<<"RTL_OSVERSIONINFOEXW:"<<std::endl;
					osvi_ex.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
					osvi_ex.wServicePackMajor=rtl_osvi_ex.wServicePackMajor;
					osvi_ex.wServicePackMinor=rtl_osvi_ex.wServicePackMinor;
					osvi_ex.wSuiteMask=rtl_osvi_ex.wSuiteMask;
					osvi_ex.wProductType=rtl_osvi_ex.wProductType;
					osvi_ex.wReserved=rtl_osvi_ex.wReserved;
				} else {
					std::cout<<"RTL_OSVERSIONINFOW:"<<std::endl;
					osvi_ex.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
				}
				return true;
			}
		}
	}
	osvi_ex.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
	if (GetVersionEx((LPOSVERSIONINFO)&osvi_ex)) {
		std::cout<<"OSVERSIONINFOEX:"<<std::endl;
		return true;
	} else if ((osvi_ex.dwOSVersionInfoSize=sizeof(OSVERSIONINFO), GetVersionEx((LPOSVERSIONINFO)&osvi_ex))) {
		std::cout<<"OSVERSIONINFO:"<<std::endl;
		return true;
	} else
		return false;
}

bool PrintRegistryKey(HKEY hive, const char* keypath, const char* value) {
	HKEY RegKey;
	DWORD buflen;
	bool success=false;
	
	char disp_path[strlen(keypath)+strlen(value)+7];
	switch ((ULONG_PTR)hive) {
		case (ULONG_PTR)HKEY_CLASSES_ROOT:
			strcpy(disp_path, "HKCR\\");
			break;
		case (ULONG_PTR)HKEY_CURRENT_CONFIG:
			strcpy(disp_path, "HKCC\\");
			break;
		case (ULONG_PTR)HKEY_CURRENT_USER:
			strcpy(disp_path, "HKCU\\");
			break;
		case (ULONG_PTR)HKEY_LOCAL_MACHINE:
			strcpy(disp_path, "HKLM\\");
			break;
		case (ULONG_PTR)HKEY_PERFORMANCE_DATA:
			strcpy(disp_path, "HKPD\\");
			break;
		case (ULONG_PTR)HKEY_PERFORMANCE_NLSTEXT:
			strcpy(disp_path, "HKPN\\");
			break;
		case (ULONG_PTR)HKEY_PERFORMANCE_TEXT:
			strcpy(disp_path, "HKPT\\");
			break;
		case (ULONG_PTR)HKEY_USERS:
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
				printf("%s:\n\t\"%s\"\n", disp_path, retbuf);
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
	char mddate[11]="?";
	char crdate[11]="?";
	
	if ((buflen=SearchPath(NULL, fpath, NULL, 0, NULL, NULL))) {
		char fullpth[buflen];
		if (SearchPath(NULL, fpath, NULL, buflen, fullpth, NULL)) {
			HANDLE hFile=CreateFile(fullpth, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile!=INVALID_HANDLE_VALUE) {
				FILETIME ft;
				SYSTEMTIME st_utc;
				if (GetFileTime(hFile, NULL, NULL, &ft)&&FileTimeToSystemTime(&ft, &st_utc))
					sprintf(mddate, "%02d/%02d/%04d", st_utc.wDay, st_utc.wMonth, st_utc.wYear);
				
				if (HANDLE hFileMapping=CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL)) {
					if (LPVOID lpFileBase=MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0)) {
						IMAGE_DOS_HEADER *p_dos_hdr=(IMAGE_DOS_HEADER*)lpFileBase;
						
						if (p_dos_hdr->e_magic==IMAGE_DOS_SIGNATURE) {
							IMAGE_NT_HEADERS *p_nt_hdr=(IMAGE_NT_HEADERS*)((ULONG_PTR)lpFileBase+p_dos_hdr->e_lfanew);
							if (p_nt_hdr->Signature==IMAGE_NT_SIGNATURE) {
								//Algorithm to convert UNIX time (time_t) to FILETIME taken from here: https://support.microsoft.com/en-us/kb/167296
								//Even though FILETIME and LONGLONG returned by Int32x32To64 are both 64-bit numbers, MS suggests no to perform 64-bit arithmetics directly on FILETIME
								//Instead it suggests using intermediate union LARGE_INTEGER to transfer 64-bit arithmetic result to FILETIME
								LARGE_INTEGER nix_to_w32;
								nix_to_w32.QuadPart=Int32x32To64(p_nt_hdr->FileHeader.TimeDateStamp, 10000000)+116444736000000000;
								ft.dwLowDateTime=nix_to_w32.LowPart;
								ft.dwHighDateTime=nix_to_w32.HighPart;
								if (FileTimeToSystemTime(&ft, &st_utc))
									sprintf(crdate, "%02d/%02d/%04d", st_utc.wDay, st_utc.wMonth, st_utc.wYear);
							}
						}
						
						UnmapViewOfFile(lpFileBase);
					}
					CloseHandle(hFileMapping);
				}
				
				CloseHandle(hFile); 
			}

			if ((buflen=GetFileVersionInfoSize(fullpth, NULL))) {
				BYTE retbuf[buflen];
				if (GetFileVersionInfo(fullpth, 0, buflen, (LPVOID)retbuf)) {
					VS_FIXEDFILEINFO *pfinfo;
					UINT finfolen;
					if (VerQueryValue((LPVOID)retbuf, "\\", (LPVOID*)&pfinfo, &finfolen)) {
						printf("%s:\n\tGetFileTime.lpLastWriteTime = %s\n\tIMAGE_FILE_HEADER.TimeDateStamp = %s\n\tVS_FIXEDFILEINFO.FileVersion = %d.%d.%d.%d\n", fpath, mddate, crdate, HIWORD(pfinfo->dwFileVersionMS), LOWORD(pfinfo->dwFileVersionMS), HIWORD(pfinfo->dwFileVersionLS), LOWORD(pfinfo->dwFileVersionLS));
						return true;
					}
				}
			}
		}
	}		
	
	printf("%s - error while opening!\n", fpath);
	return false;
}
#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <ntdef.h>
#include <windows.h>
#include "externs.h"
#include "labeled_values.hpp"

extern pRtlGetVersion fnRtlGetVersion;
extern pGetNativeSystemInfo fnGetNativeSystemInfo;
extern pIsWow64Process fnIsWow64Process;
extern pGetProductInfo fnGetProductInfo;
extern pWow64DisableWow64FsRedirection fnWow64DisableWow64FsRedirection;
extern pWow64RevertWow64FsRedirection fnWow64RevertWow64FsRedirection;
extern pGetMappedFileName fnGetMappedFileName;
extern pNtCreateFile fnNtCreateFile;
extern pNtQueryObject fnNtQueryObject;
extern pGetSystemWow64DirectoryW fnGetSystemWow64DirectoryW;
extern pSetSearchPathMode fnSetSearchPathMode;
extern pwine_get_version fnwine_get_version;

typedef struct _LANGANDCODEPAGE {
	WORD wLanguage;
	WORD wCodePage;
} LANGANDCODEPAGE;

//Defines not found in MinGW's windows.h/winnt.h:
#define SM_WEPOS			0x2003
#define SM_FUNDAMENTALS		0x2004

//std::cout treats char as, obvously, char and prints it as such - not it's numerical value
//If numerical value is needed, instead of static cast to ULONG_PTR unary addition operator can be used to force printing numerical value
//Also, these defines only work correctly with specific std::ios::fmtflags and std::ios::fill (see below in main)
#define COUT_DEC(dec_int)				+dec_int
#define COUT_FIX(dec_int, dec_width)	std::setw(dec_width)<<+dec_int
#define COUT_HEX(hex_int, hex_width)	"0x"<<std::hex<<std::setw(hex_width)<<+hex_int<<std::dec
#define COUT_BOOL(bool_val)				(bool_val?"TRUE":"FALSE")

LabeledValues PlatfromIds(LABELED_VALUES_ARG(VER_PLATFORM_WIN32s, VER_PLATFORM_WIN32_WINDOWS, VER_PLATFORM_WIN32_NT));
LabeledValues SuiteMasks(LABELED_VALUES_ARG(VER_WORKSTATION_NT, VER_SERVER_NT, VER_SUITE_SMALLBUSINESS, VER_SUITE_ENTERPRISE, VER_SUITE_BACKOFFICE, VER_SUITE_COMMUNICATIONS, VER_SUITE_TERMINAL, VER_SUITE_SMALLBUSINESS_RESTRICTED, VER_SUITE_EMBEDDEDNT, VER_SUITE_DATACENTER, VER_SUITE_SINGLEUSERTS, VER_SUITE_PERSONAL, VER_SUITE_BLADE, VER_SUITE_EMBEDDED_RESTRICTED, VER_SUITE_SECURITY_APPLIANCE, VER_SUITE_STORAGE_SERVER, VER_SUITE_COMPUTE_SERVER, VER_SUITE_WH_SERVER));
LabeledValues ProductTypes(LABELED_VALUES_ARG(VER_NT_WORKSTATION, VER_NT_DOMAIN_CONTROLLER, VER_NT_SERVER));
LabeledValues SystemMetrics(LABELED_VALUES_ARG(SM_FUNDAMENTALS, SM_WEPOS, SM_DEBUG, SM_PENWINDOWS, SM_DBCSENABLED, SM_IMMENABLED, SM_SLOWMACHINE, SM_TABLETPC, SM_MEDIACENTER, SM_STARTER, SM_SERVERR2));
LabeledValues ProcessorArchitectures(LABELED_VALUES_ARG(PROCESSOR_ARCHITECTURE_INTEL, PROCESSOR_ARCHITECTURE_MIPS, PROCESSOR_ARCHITECTURE_ALPHA, PROCESSOR_ARCHITECTURE_PPC, PROCESSOR_ARCHITECTURE_SHX, PROCESSOR_ARCHITECTURE_ARM, PROCESSOR_ARCHITECTURE_IA64, PROCESSOR_ARCHITECTURE_ALPHA64, PROCESSOR_ARCHITECTURE_MSIL, PROCESSOR_ARCHITECTURE_AMD64, PROCESSOR_ARCHITECTURE_IA32_ON_WIN64, PROCESSOR_ARCHITECTURE_NEUTRAL, PROCESSOR_ARCHITECTURE_UNKNOWN));
LabeledValues ProductInfoTypes(LABELED_VALUES_ARG(PRODUCT_UNDEFINED, PRODUCT_ULTIMATE, PRODUCT_HOME_BASIC, PRODUCT_HOME_PREMIUM, PRODUCT_ENTERPRISE, PRODUCT_HOME_BASIC_N, PRODUCT_BUSINESS, PRODUCT_STANDARD_SERVER, PRODUCT_DATACENTER_SERVER, PRODUCT_SMALLBUSINESS_SERVER, PRODUCT_ENTERPRISE_SERVER, PRODUCT_STARTER, PRODUCT_DATACENTER_SERVER_CORE, PRODUCT_STANDARD_SERVER_CORE, PRODUCT_ENTERPRISE_SERVER_CORE, PRODUCT_ENTERPRISE_SERVER_IA64, PRODUCT_BUSINESS_N, PRODUCT_WEB_SERVER, PRODUCT_CLUSTER_SERVER, PRODUCT_HOME_SERVER, PRODUCT_STORAGE_EXPRESS_SERVER, PRODUCT_STORAGE_STANDARD_SERVER, PRODUCT_STORAGE_WORKGROUP_SERVER, PRODUCT_STORAGE_ENTERPRISE_SERVER, PRODUCT_SERVER_FOR_SMALLBUSINESS, PRODUCT_SMALLBUSINESS_SERVER_PREMIUM, PRODUCT_UNLICENSED, PRODUCT_HOME_PREMIUM_N, PRODUCT_ENTERPRISE_N, PRODUCT_ULTIMATE_N, PRODUCT_WEB_SERVER_CORE, PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT, PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY, PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING, PRODUCT_SERVER_FOUNDATION, PRODUCT_HOME_PREMIUM_SERVER, PRODUCT_SERVER_FOR_SMALLBUSINESS_V, PRODUCT_STANDARD_SERVER_V, PRODUCT_DATACENTER_SERVER_V, PRODUCT_ENTERPRISE_SERVER_V, PRODUCT_DATACENTER_SERVER_CORE_V, PRODUCT_STANDARD_SERVER_CORE_V, PRODUCT_ENTERPRISE_SERVER_CORE_V, PRODUCT_HYPERV, PRODUCT_STORAGE_EXPRESS_SERVER_CORE, PRODUCT_STORAGE_STANDARD_SERVER_CORE, PRODUCT_STORAGE_WORKGROUP_SERVER_CORE, PRODUCT_STORAGE_ENTERPRISE_SERVER_CORE, PRODUCT_STARTER_N, PRODUCT_PROFESSIONAL, PRODUCT_PROFESSIONAL_N, PRODUCT_SB_SOLUTION_SERVER, PRODUCT_SERVER_FOR_SB_SOLUTIONS, PRODUCT_STANDARD_SERVER_SOLUTIONS, PRODUCT_STANDARD_SERVER_SOLUTIONS_CORE, PRODUCT_SB_SOLUTION_SERVER_EM, PRODUCT_SERVER_FOR_SB_SOLUTIONS_EM, PRODUCT_SOLUTION_EMBEDDEDSERVER, PRODUCT_SOLUTION_EMBEDDEDSERVER_CORE, PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE, PRODUCT_ESSENTIALBUSINESS_SERVER_MGMT, PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL, PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC, PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC, PRODUCT_CLUSTER_SERVER_V, PRODUCT_EMBEDDED, PRODUCT_STARTER_E, PRODUCT_HOME_BASIC_E, PRODUCT_HOME_PREMIUM_E, PRODUCT_PROFESSIONAL_E, PRODUCT_ENTERPRISE_E, PRODUCT_ULTIMATE_E));
BasicLabeledValues<HKEY> RegistryHives(LABELED_VALUES_ARG(HKEY_CLASSES_ROOT, HKEY_CURRENT_CONFIG, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE, HKEY_PERFORMANCE_DATA, HKEY_PERFORMANCE_TEXT, HKEY_USERS));

void PrintRegistryKey(HKEY hive, const char* keypath, const char* value);
void PrintFileInformation(const char* fpath, BOOL wow64);
bool GetVersionWrapper(OSVERSIONINFOEX &osvi_ex);

int main(int argc, char* argv[])
{
	Externs::MakeInstance();
	
	//This sets fill character to '0' for all subsequent cout outputs 
	//It only matters when setw in non-zero and it is reset to zero after every output
	std::cout.fill('0');

	//This prevents base from showing in hex/oct output and hex/oct output becomes uppercase
	//N.B.: Uppercase also makes base to be displayed in uppercase (that's why showing base is disabled) but doesn't affect boolalpha flag (bool values still displayed in lowercase)
	//Applied for all subsequent cout outputs 
	std::cout.unsetf(std::ios::showbase);
	std::cout.setf(std::ios::uppercase); 

	//Disable Windows error dialog popup for failed LoadLibrary attempts
	//This is done so PrintFileInformation can search for needed files using LoadLibrary
	SetErrorMode(SEM_FAILCRITICALERRORS);	

	OSVERSIONINFOEX osvi_ex;
	if (GetVersionWrapper(osvi_ex)) {
		std::cout<<"\tdwMajorVersion = "<<COUT_DEC(osvi_ex.dwMajorVersion)<<std::endl;
		std::cout<<"\tdwMinorVersion = "<<COUT_DEC(osvi_ex.dwMinorVersion)<<std::endl;
		std::cout<<"\tdwBuildNumber = "<<COUT_DEC(osvi_ex.dwBuildNumber)<<" = "<<COUT_DEC(HIBYTE(HIWORD(osvi_ex.dwBuildNumber)))<<"."<<COUT_DEC(LOBYTE(HIWORD(osvi_ex.dwBuildNumber)))<<"."<<COUT_DEC((DWORD)LOWORD(osvi_ex.dwBuildNumber))<<std::endl;
		std::cout<<"\tdwPlatformId = "<<COUT_HEX(osvi_ex.dwPlatformId, 8)<<std::endl;
		PlatfromIds.Enums(osvi_ex.dwPlatformId, [](const std::string& label, DWORD value, bool unknown, size_t idx){
			if (unknown)
				std::cout<<"\t\tUNKNOWN VALUE ("<<COUT_HEX(value, 8)<<")"<<std::endl;
			else
				std::cout<<"\t\t"<<label<<std::endl;
			return false;
		});
		std::cout<<"\tszCSDVersion = \""<<COUT_DEC(osvi_ex.szCSDVersion)<<"\""<<std::endl;
		if (osvi_ex.dwOSVersionInfoSize==sizeof(OSVERSIONINFOEX)) {
			std::cout<<"\twServicePackMajor = "<<COUT_DEC(osvi_ex.wServicePackMajor)<<std::endl;
			std::cout<<"\twServicePackMinor = "<<COUT_DEC(osvi_ex.wServicePackMinor)<<std::endl;
			std::cout<<"\twSuiteMask = "<<COUT_HEX(osvi_ex.wSuiteMask, 4)<<std::endl;
			SuiteMasks.Flags(osvi_ex.wSuiteMask, [](const std::string& label, DWORD value, bool unknown, size_t idx){
				if (unknown)
					std::cout<<"\t\tUNKNOWN FLAG ("<<COUT_HEX(value, 4)<<")"<<std::endl;
				else
					std::cout<<"\t\t"<<label<<std::endl;
				return false;
			});
			std::cout<<"\twProductType = "<<COUT_HEX(osvi_ex.wProductType, 2)<<std::endl;
			ProductTypes.Enums(osvi_ex.wProductType, [](const std::string& label, DWORD value, bool unknown, size_t idx){
				if (unknown)
					std::cout<<"\t\tUNKNOWN VALUE ("<<COUT_HEX(value, 2)<<")"<<std::endl;
				else
					std::cout<<"\t\t"<<label<<std::endl;
				return false;
			});
			std::cout<<"\twReserved = "<<COUT_HEX(osvi_ex.wReserved, 2)<<std::endl;
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
	
	BOOL wow64=FALSE;
	if (fnIsWow64Process) {	
		//Test if current process is running under WOW64
		if (fnIsWow64Process(GetCurrentProcess(), &wow64))
			std::cout<<"IsWow64Process = "<<COUT_BOOL(wow64==TRUE)<<std::endl;
		else
			std::cout<<"IsWow64Process failed!"<<std::endl;
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
	
	PrintFileInformation("KERNEL32.DLL", wow64);
	PrintFileInformation("USER.EXE", wow64);
	PrintFileInformation("NTKERN.VXD", wow64);
	
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
					std::cout<<"RtlGetVersion.RTL_OSVERSIONINFOEXW:"<<std::endl;
					osvi_ex.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
					osvi_ex.wServicePackMajor=rtl_osvi_ex.wServicePackMajor;
					osvi_ex.wServicePackMinor=rtl_osvi_ex.wServicePackMinor;
					osvi_ex.wSuiteMask=rtl_osvi_ex.wSuiteMask;
					osvi_ex.wProductType=rtl_osvi_ex.wProductType;
					osvi_ex.wReserved=rtl_osvi_ex.wReserved;
				} else {
					std::cout<<"RtlGetVersion.RTL_OSVERSIONINFOW:"<<std::endl;
					osvi_ex.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
				}
				return true;
			}
		}
	}
	osvi_ex.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
	if (GetVersionEx((LPOSVERSIONINFO)&osvi_ex)) {
		std::cout<<"GetVersionEx.OSVERSIONINFOEX:"<<std::endl;
		return true;
	} else if ((osvi_ex.dwOSVersionInfoSize=sizeof(OSVERSIONINFO), GetVersionEx((LPOSVERSIONINFO)&osvi_ex))) {
		std::cout<<"GetVersionEx.OSVERSIONINFO:"<<std::endl;
		return true;
	} else
		return false;
}

void PrintRegistryKey(HKEY hive, const char* keypath, const char* value) 
{
	HKEY reg_key;
	DWORD buflen;
	DWORD key_type;
	bool success=false;
	
	if (RegOpenKeyEx(hive, keypath, 0, KEY_READ, &reg_key)==ERROR_SUCCESS) {
		if (RegQueryValueEx(reg_key, value, NULL, &key_type, NULL, &buflen)==ERROR_SUCCESS&&(key_type==REG_SZ||key_type==REG_EXPAND_SZ)) {
			char retbuf[buflen];
			if (RegQueryValueEx(reg_key, value, NULL, &key_type, (LPBYTE)retbuf, &buflen)==ERROR_SUCCESS&&(key_type==REG_SZ||key_type==REG_EXPAND_SZ)&&retbuf[buflen-1]=='\0') {
				std::cout<<RegistryHives(hive)<<"\\"<<keypath<<"\\"<<value<<":\n\t\""<<retbuf<<"\""<<std::endl;
				success=true;
			}
		}
		RegCloseKey(reg_key);
	}
	
	if (!success) 
		std::cout<<RegistryHives(hive)<<"\\"<<keypath<<"\\"<<value<<" - error while opening!"<<std::endl;
}

std::string UnredirectWow64FsPath(const char* query_path, BOOL wow64)
{
#ifdef _WIN64
	//Useless for 64-bit binary - no redirection is done for it
	return query_path;
#else
	//How does it work
	//We are supplying original path (prefixed with \??\ so to make NT path from it) to NtCreateFile
	//WoW64 redirection expands to NtCreateFile for \??\ paths so when NtCreateFile opens this file - it opens the right (intended) one
	//Now we use NtQueryObject to get canonical NT path (\Device\...) from opened handle
	//These paths are not affected by WoW64 redirection so returned path is the real one
	//After that we get WoW64 directory (with GetSystemWow64Directory) and convert it to NT path the same way as original path
	//Find out if original path has something to do with WoW64 redirection by checing if it is prefixed by WoW64 directory
	//If not - just return original path because it is not actually redirected
	//Now all is left is to convert original path in it's NT canonical form back to it's Win32 equivalent
	//In this case it is simple because we already know Win32 equivalent of it's device prefix which is WoW64 directory
	//Just swap this prefix with it's Win32 equivalent and we are good to go
	
	//Some caveats
	//Original path should be valid file path - file must exist and everything
	//Algorithm can be mofified to also allow directory as original path - but it still must exist

	//If not on WoW64 - obviously return query_path (aka original string)
	//If WoW64 is TRUE, all functions below in theory should be available
	//But because calling unavailable function potentially causes access violation it's a good thing to check them anyway (NT functions are subject to change according to MS)
	//(And treat unavailability of functions as unavailability of WoW64)
	if (wow64!=TRUE||!fnGetSystemWow64DirectoryW||!fnNtCreateFile||!fnNtQueryObject)
		return query_path;
	
	UINT chars_num=fnGetSystemWow64DirectoryW(NULL, 0);
	//GetSystemWow64Directory can fail with GetLastError()==ERROR_CALL_NOT_IMPLEMENTED indicating that we should return query_path because there are no WoW64 obviously
	//But it is impossible case because wow64 should be FALSE for that to work and we have already checked for it to be TRUE
	//So if GetSystemWow64Directory failed: it failed for some other nasty reason - treat as conversion error
	if (!chars_num) 
		return "";
	
	wchar_t wow64_path[chars_num+4]=L"\\??\\";	//4 is length of "\??\" in characters not including '\0'
	if (!fnGetSystemWow64DirectoryW(wow64_path+4, chars_num))
		return "";
	
	chars_num=MultiByteToWideChar(CP_ACP, 0, query_path, -1, NULL, 0);
	if (!chars_num)
		return "";
	
	wchar_t wq_path[chars_num+4]=L"\\??\\";	//4 is length of "\??\" in characters not including '\0'
	if (!MultiByteToWideChar(CP_ACP, 0, query_path, -1, wq_path+4, chars_num))
		return "";
	
	HANDLE hFile;
	OBJECT_ATTRIBUTES objAttribs;	
	UNICODE_STRING ustr_fpath={(USHORT)(wcslen(wq_path)*sizeof(wchar_t)), (USHORT)((wcslen(wq_path)+1)*sizeof(wchar_t)), wq_path};
	IO_STATUS_BLOCK ioStatusBlock;
	
	InitializeObjectAttributes(&objAttribs, &ustr_fpath, OBJ_CASE_INSENSITIVE, NULL, NULL);	
	if (!NT_SUCCESS(fnNtCreateFile(&hFile, FILE_READ_ATTRIBUTES, &objAttribs, &ioStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, FILE_OPEN, FILE_NON_DIRECTORY_FILE, NULL, 0)))
		return "";
	
	DWORD buf_len=wcslen(wq_path)*sizeof(wchar_t)+1024;
	BYTE oni_buf_wqpath[buf_len];
	if (!NT_SUCCESS(fnNtQueryObject(hFile, ObjectNameInformation, (OBJECT_NAME_INFORMATION*)oni_buf_wqpath, buf_len, NULL))) {
		CloseHandle(hFile);
		//NtQueryObject can fail with STATUS_INVALID_INFO_CLASS indicating that ObjectNameInformation class is not supported
		//This could in theory happen on some damn old NT which, because of it's age, shouldn't also support WoW64
		//But we have already checked WoW64 presence and it should be TRUE if we are here
		//Or MS decided (again) to change NtQueryObject behaviour for one of the future releases and remove STATUS_INVALID_INFO_CLASS
		//All in all, if NtQueryObject failed for any of the reasons - it's conversion error
		return "";
	}	
	CloseHandle(hFile);
	
	ustr_fpath.Length=(USHORT)(wcslen(wow64_path)*sizeof(wchar_t));
	ustr_fpath.MaximumLength=(USHORT)((wcslen(wow64_path)+1)*sizeof(wchar_t));
	ustr_fpath.Buffer=wow64_path;
	
	InitializeObjectAttributes(&objAttribs, &ustr_fpath, OBJ_CASE_INSENSITIVE, NULL, NULL);	
	if (!NT_SUCCESS(fnNtCreateFile(&hFile, FILE_READ_ATTRIBUTES, &objAttribs, &ioStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, FILE_OPEN, FILE_DIRECTORY_FILE, NULL, 0)))
		return "";
	
	buf_len=wcslen(wow64_path)*sizeof(wchar_t)+1024;
	BYTE oni_buf_wow64path[buf_len];
	if (!NT_SUCCESS(fnNtQueryObject(hFile, ObjectNameInformation, (OBJECT_NAME_INFORMATION*)oni_buf_wow64path, buf_len, NULL))) {
		CloseHandle(hFile);
		return "";
	}	
	CloseHandle(hFile);
	
	wchar_t* wow64_krn_path=((OBJECT_NAME_INFORMATION*)oni_buf_wow64path)->Name.Buffer;
	wchar_t* wq_krn_path=((OBJECT_NAME_INFORMATION*)oni_buf_wqpath)->Name.Buffer;
	chars_num=((OBJECT_NAME_INFORMATION*)oni_buf_wow64path)->Name.Length/sizeof(wchar_t);
	//First check that queried path is prefixed by WoW64 path
	//Length of wow64_krn_path (chars_num) is guaranteed to be >0 because otherwise NtCreateFile or NtQueryObject should have failed
	if (wcsncmp(wow64_krn_path, wq_krn_path, chars_num))
		return query_path;
	
	//If it is prefixed and prefix doesn't end with backslash - check that character next to prefix is backslash
	//We don't check wq_krn_path length because wq_krn_path can only be file path and if it passed wcsncmp, then it should be longer than it's directory prefix
	if (wow64_krn_path[chars_num-1]!=L'\\'&&wq_krn_path[chars_num]!=L'\\')
		return query_path;
	
	//Now everything is alright and we can swap prefix with it's Win32 equivalent
	wchar_t wunrd_path[wcslen(wow64_path)+((OBJECT_NAME_INFORMATION*)oni_buf_wqpath)->Name.Length/sizeof(wchar_t)-chars_num+1];
	wcscpy(wunrd_path, wow64_path);
	wcscat(wunrd_path, wq_krn_path+chars_num);
	
	chars_num=WideCharToMultiByte(CP_ACP, 0, wunrd_path+4, -1, NULL, 0, NULL, NULL);	//4 is length of "\??\" in characters not including '\0'
	if (!chars_num)
		return "";
	
	char unredirected_path[chars_num];
	if (!WideCharToMultiByte(CP_ACP, 0, wunrd_path+4, -1, unredirected_path, chars_num, NULL, NULL))	//4 is length of "\??\" in characters not including '\0'
		return "";
	
	return unredirected_path;
#endif
}

std::string PathViaLoadLibrary(const char* query_path, BOOL wow64) 
{
	//PrintFileInformation expects that just file name will be supplied, not a full path
	//Because most Windows files that are relevant to OS detection are system DLLs - to find full path some LoadLibrary-like search algorithm should be used for best results
	//It turns out that more straightforward approach is the most efffective here - just let LoadRibrary load file in question and then get loaded module path
	//It is somehow redundand but it safer than implementing own LoadLibrary-like search algorithm (and MSDN specifically states that SearchPath shouldn't be used as substitute)
	//Also GetFileVersionInfo behaves similar - instead of parsing file resources on it's own, it relies on LoadLibrary to do it
	
	//LoadLibrary can't load libraries if it's path exceeds MAX_PATH so it's safe to assume that MAX_PATH is enough to hold full library path
	char full_path[MAX_PATH];
	DWORD retlen=0;
	//First see if this library was already loaded by current process
	if (HMODULE hMod=GetModuleHandle(query_path)) {
		retlen=GetModuleFileName(hMod, full_path, MAX_PATH);
	
	//That way we don't execute DLLMain but we have to convert NT path to Win32
	} else if (HMODULE hMod=LoadLibraryEx(query_path, NULL, LOAD_LIBRARY_AS_DATAFILE)) {
		char pszFilename[MAX_PATH]="";
		fnGetMappedFileName(GetCurrentProcess(), hMod, pszFilename, MAX_PATH);
		FreeLibrary(hMod);
		return pszFilename;
		//balh, blah, retlen...
	}
	
	//If not - load it and get path
	/*} else if (HMODULE hMod=LoadLibrary(query_path)) {
		retlen=GetModuleFileName(hMod, full_path, MAX_PATH);
		FreeLibrary(hMod);
	}*/
	
	//GetModuleFileName returns 0 if everything is bad and nSize (which is MAX_PATH) if buffer size is insufficient and returned path truncated
	//In case of MAX_PATH, buffer size really should be enough to hold any library path but someday MS may decide to lift LoadLibrary limitation on path size
	//It's better to be safe than sorry
	if (retlen&&retlen<MAX_PATH)
		//It is observed that some file paths returned by GetModuleFileName are not affected by WoW64 redirection
		//But it's not always the case so it's better to convert anyway
		return UnredirectWow64FsPath(full_path, wow64);
	else
		return "";
}

std::string PathViaSearchPath(const char* query_path, BOOL wow64) 
{
	//Fallback variant if PathViaLoadLibrary failed
	//In this case file doesn't seem to be valid library (otherwise PathViaLoadLibrary would have worked) so go on and use SearchPath

	if (DWORD buflen=SearchPath(NULL, query_path, NULL, 0, NULL, NULL)) {
		char full_path[buflen];
		if (SearchPath(NULL, query_path, NULL, buflen, full_path, NULL)) {
			return UnredirectWow64FsPath(full_path, wow64);
		}
	}
	
	return "";
}

void PrintFileInformation(const char* query_path, BOOL wow64) 
{
	//Some Windows system files bear information that can help in detecting Windows version
	//Most of these files are PE DLLs but some are also LE VXDs
	//Microsoft vaguely suggests checking VS_FIXEDFILEINFO.FILEVERSION of VERSIONINFO resource and "date" of system files to help detect OS version
	//In reality it's better to check StringFileInfo.ProductVersion of VERSIONINFO resource and file's modified date/time
	//ProductVersion holds version info more related to OS version than FILEVERSION that holds actual file version (TODO: example for Win ME and NT4)
	//N.B.: 
	//File properties dialog prior to Vista showed StringFileInfo.FileVersion in it's header (not actual VS_FIXEDFILEINFO.FILEVERSION)
	//Also this version of dialog allowed to view any other (even custom) StringFileInfo item
	//Starting from Vista only limited number of VERSIONINFO items is shown
	//And only two of them is about versioning: VS_FIXEDFILEINFO.FILEVERSION (not StringFileInfo.FileVersion) and StringFileInfo.ProductVersion
	//None of file properties dialog versions ever showed VS_FIXEDFILEINFO.PRODUCTVERSION
	
	//PE files have TimeDateStamp field in the header that contains actual build date
	//But binary can be shipped modified by Microsoft and on Win 9x HH:MM:SS portion of modified date actually contains build number instead of real modified time
	//LE files have ModuleVersion field in the headder but it's not used in any of system VXDs
	
	//"File not found" is a result - absence/presence of specific files also helps in detecting OS version (ex: USB supplement)
	//Also see comments for PathViaLoadLibrary to find out how queried file is searched
	
	std::string full_path=PathViaLoadLibrary(query_path, wow64);
	if (full_path.empty()) full_path=PathViaSearchPath(query_path, wow64);
	
	if (full_path.empty()) {
		std::cout<<query_path<<" - file not found!"<<std::endl;
	} else {			
		bool got_info=false;
		std::cout<<query_path<<" ("<<full_path<<"):"<<std::endl;
		
		HANDLE hFile=CreateFile(full_path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile!=INVALID_HANDLE_VALUE) {
			FILETIME ft_utc, ft_local;
			SYSTEMTIME st_local;
			
			if (GetFileTime(hFile, NULL, NULL, &ft_utc)&&FileTimeToLocalFileTime(&ft_utc, &ft_local)&&FileTimeToSystemTime(&ft_local, &st_local)) {
				std::cout<<"\tGetFileTime.lpLastWriteTime = "<<COUT_FIX(st_local.wDay, 2)<<"/"<<COUT_FIX(st_local.wMonth, 2)<<"/"<<COUT_FIX(st_local.wYear, 2)<<" "<<COUT_FIX(st_local.wHour, 2)<<":"<<COUT_FIX(st_local.wMinute, 2)<<":"<<COUT_FIX(st_local.wSecond, 2)<<std::endl;
				got_info=true;
			}
			
			CloseHandle(hFile); 
		}
		
		if (DWORD buflen=GetFileVersionInfoSize(full_path.c_str(), NULL)) {	
			BYTE retbuf[buflen];
			if (GetFileVersionInfo(full_path.c_str(), 0, buflen, (LPVOID)retbuf)) {	
				//If GetFileVersionInfo finds a MUI file for the file it is currently querying, it will use VERSIONINFO from this file instead of original one
				//So information can differ between what Explorer show (actual file VERSIONINFO) and what GetFileVersionInfo retreives
				LANGANDCODEPAGE *plcp;
				UINT lcplen;
				if (VerQueryValue((LPVOID)retbuf, "\\VarFileInfo\\Translation", (LPVOID*)&plcp, &lcplen)&&lcplen>=sizeof(LANGANDCODEPAGE)) {
					//We are interested only in first translation - most system files have only one VERSIONINFO translation anyway
					char *value;
					UINT valuelen;
					std::stringstream qstr;
					qstr<<std::nouppercase<<std::noshowbase<<std::hex<<std::setfill('0')<<"\\StringFileInfo\\"<<std::setw(4)<<plcp->wLanguage<<std::setw(4)<<plcp->wCodePage<<"\\ProductVersion";
					if (VerQueryValue((LPVOID)retbuf, qstr.str().c_str(), (LPVOID*)&value, &valuelen)) {
						std::cout<<"\tVERSIONINFO"<<qstr.str()<<" = \""<<value<<"\""<<std::endl;
						got_info=true;
					}
				}
			}
		}
		
		if (!got_info)
			std::cout<<"\tNO INFORMATION FOUND"<<std::endl;	
	}
}
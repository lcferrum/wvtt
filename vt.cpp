#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <fstream>
#include <conio.h>
#include <windows.h>
#include "externs.h"
#include "tee_buf.hpp"
#include "fp_routines.h"
#include "labeled_values.hpp"

extern pRtlGetVersion fnRtlGetVersion;
extern pGetNativeSystemInfo fnGetNativeSystemInfo;
extern pIsWow64Process fnIsWow64Process;
extern pGetProductInfo fnGetProductInfo;
extern pGetVersionExA fnGetVersionExA;
extern pwine_get_version fnwine_get_version;

typedef struct _LANGANDCODEPAGE {
	WORD wLanguage;
	WORD wCodePage;
} LANGANDCODEPAGE;

//Defines not found in MinGW's windows.h/winnt.h:
#define SM_WEPOS			0x2003
#define SM_FUNDAMENTALS		0x2004
#define VER_PLATFORM_WIN32_CE	3

//std::cout treats char as, obviously, char and prints it as such - not it's numerical value
//If numerical value is needed, instead of static cast to ULONG_PTR unary addition operator can be used to force printing numerical value
//Also, these defines only work correctly with specific std::ios::fmtflags and std::ios::fill (see below in main)
//Dec_width and hex_width is in chars
#define COUT_DEC(dec_int)				+(dec_int)
#define COUT_FIX(dec_int, dec_width)	std::setw(dec_width)<<+(dec_int)
#define COUT_HEX(hex_int, hex_width)	"0x"<<std::hex<<std::setw(hex_width)<<+(hex_int)<<std::dec
#define COUT_BOOL(bool_val)				((bool_val)?"TRUE":"FALSE")

LabeledValues PlatfromIds(LABELED_VALUES_ARG(VER_PLATFORM_WIN32s, VER_PLATFORM_WIN32_WINDOWS, VER_PLATFORM_WIN32_NT, VER_PLATFORM_WIN32_CE));
LabeledValues SuiteMasks(LABELED_VALUES_ARG(VER_WORKSTATION_NT, VER_SERVER_NT, VER_SUITE_SMALLBUSINESS, VER_SUITE_ENTERPRISE, VER_SUITE_BACKOFFICE, VER_SUITE_COMMUNICATIONS, VER_SUITE_TERMINAL, VER_SUITE_SMALLBUSINESS_RESTRICTED, VER_SUITE_EMBEDDEDNT, VER_SUITE_DATACENTER, VER_SUITE_SINGLEUSERTS, VER_SUITE_PERSONAL, VER_SUITE_BLADE, VER_SUITE_EMBEDDED_RESTRICTED, VER_SUITE_SECURITY_APPLIANCE, VER_SUITE_STORAGE_SERVER, VER_SUITE_COMPUTE_SERVER, VER_SUITE_WH_SERVER));
LabeledValues ProductTypes(LABELED_VALUES_ARG(VER_NT_WORKSTATION, VER_NT_DOMAIN_CONTROLLER, VER_NT_SERVER));
LabeledValues SystemMetrics(LABELED_VALUES_ARG(SM_FUNDAMENTALS, SM_WEPOS, SM_DEBUG, SM_PENWINDOWS, SM_DBCSENABLED, SM_IMMENABLED, SM_SLOWMACHINE, SM_TABLETPC, SM_MEDIACENTER, SM_STARTER, SM_SERVERR2));
LabeledValues ProcessorArchitectures(LABELED_VALUES_ARG(PROCESSOR_ARCHITECTURE_INTEL, PROCESSOR_ARCHITECTURE_MIPS, PROCESSOR_ARCHITECTURE_ALPHA, PROCESSOR_ARCHITECTURE_PPC, PROCESSOR_ARCHITECTURE_SHX, PROCESSOR_ARCHITECTURE_ARM, PROCESSOR_ARCHITECTURE_IA64, PROCESSOR_ARCHITECTURE_ALPHA64, PROCESSOR_ARCHITECTURE_MSIL, PROCESSOR_ARCHITECTURE_AMD64, PROCESSOR_ARCHITECTURE_IA32_ON_WIN64, PROCESSOR_ARCHITECTURE_NEUTRAL, PROCESSOR_ARCHITECTURE_UNKNOWN));
LabeledValues ProductInfoTypes(LABELED_VALUES_ARG(PRODUCT_UNDEFINED, PRODUCT_ULTIMATE, PRODUCT_HOME_BASIC, PRODUCT_HOME_PREMIUM, PRODUCT_ENTERPRISE, PRODUCT_HOME_BASIC_N, PRODUCT_BUSINESS, PRODUCT_STANDARD_SERVER, PRODUCT_DATACENTER_SERVER, PRODUCT_SMALLBUSINESS_SERVER, PRODUCT_ENTERPRISE_SERVER, PRODUCT_STARTER, PRODUCT_DATACENTER_SERVER_CORE, PRODUCT_STANDARD_SERVER_CORE, PRODUCT_ENTERPRISE_SERVER_CORE, PRODUCT_ENTERPRISE_SERVER_IA64, PRODUCT_BUSINESS_N, PRODUCT_WEB_SERVER, PRODUCT_CLUSTER_SERVER, PRODUCT_HOME_SERVER, PRODUCT_STORAGE_EXPRESS_SERVER, PRODUCT_STORAGE_STANDARD_SERVER, PRODUCT_STORAGE_WORKGROUP_SERVER, PRODUCT_STORAGE_ENTERPRISE_SERVER, PRODUCT_SERVER_FOR_SMALLBUSINESS, PRODUCT_SMALLBUSINESS_SERVER_PREMIUM, PRODUCT_UNLICENSED, PRODUCT_HOME_PREMIUM_N, PRODUCT_ENTERPRISE_N, PRODUCT_ULTIMATE_N, PRODUCT_WEB_SERVER_CORE, PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT, PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY, PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING, PRODUCT_SERVER_FOUNDATION, PRODUCT_HOME_PREMIUM_SERVER, PRODUCT_SERVER_FOR_SMALLBUSINESS_V, PRODUCT_STANDARD_SERVER_V, PRODUCT_DATACENTER_SERVER_V, PRODUCT_ENTERPRISE_SERVER_V, PRODUCT_DATACENTER_SERVER_CORE_V, PRODUCT_STANDARD_SERVER_CORE_V, PRODUCT_ENTERPRISE_SERVER_CORE_V, PRODUCT_HYPERV, PRODUCT_STORAGE_EXPRESS_SERVER_CORE, PRODUCT_STORAGE_STANDARD_SERVER_CORE, PRODUCT_STORAGE_WORKGROUP_SERVER_CORE, PRODUCT_STORAGE_ENTERPRISE_SERVER_CORE, PRODUCT_STARTER_N, PRODUCT_PROFESSIONAL, PRODUCT_PROFESSIONAL_N, PRODUCT_SB_SOLUTION_SERVER, PRODUCT_SERVER_FOR_SB_SOLUTIONS, PRODUCT_STANDARD_SERVER_SOLUTIONS, PRODUCT_STANDARD_SERVER_SOLUTIONS_CORE, PRODUCT_SB_SOLUTION_SERVER_EM, PRODUCT_SERVER_FOR_SB_SOLUTIONS_EM, PRODUCT_SOLUTION_EMBEDDEDSERVER, PRODUCT_SOLUTION_EMBEDDEDSERVER_CORE, PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE, PRODUCT_ESSENTIALBUSINESS_SERVER_MGMT, PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL, PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC, PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC, PRODUCT_CLUSTER_SERVER_V, PRODUCT_EMBEDDED, PRODUCT_STARTER_E, PRODUCT_HOME_BASIC_E, PRODUCT_HOME_PREMIUM_E, PRODUCT_PROFESSIONAL_E, PRODUCT_ENTERPRISE_E, PRODUCT_ULTIMATE_E));
BasicLabeledValues<HKEY> RegistryHives(LABELED_VALUES_ARG(HKEY_CLASSES_ROOT, HKEY_CURRENT_CONFIG, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE, HKEY_PERFORMANCE_DATA, HKEY_PERFORMANCE_TEXT, HKEY_USERS));

void PrintRegistryKey(HKEY hive, const char* keypath, const char* value);
void PrintFileInformation(const char* query_path, const char* sfi_item);
bool GetVersionWrapper(OSVERSIONINFOEX &osvi_ex);

#ifdef __clang__
//Obscure clang++ bug - it reports "multiple definition" of std::setfill() and __gnu_cxx::operator==() when statically linking with libstdc++
//Observed on LLVM 3.6.2 with MinGW 4.7.2
//This is a fix for the bug
extern template std::_Setfill<char> std::setfill(char);													//caused by use of std::setfill(char)
extern template bool __gnu_cxx::operator==(const std::string::iterator&, const std::string::iterator&);	//caused by use of std::remove_if(std::string::iterator, std::string::iterator, bool (*)(char))
#endif

#if defined(NT3)&&!defined(_WIN64)
extern "C" bool __stdcall GetSystemMetricsSehWrapper(int nIndex, int* result);
#endif

int main(int argc, char* argv[])
{
	Externs::MakeInstance();
	
	std::string cout_copy;
	CreateOstreamTeeBuf(std::cout, cout_tee_buf, std::bind<std::string&(std::string::*)(const char*, size_t)>(&std::string::append, &cout_copy, std::placeholders::_1, std::placeholders::_2));
	HANDLE hstdstream=GetStdHandle(STD_OUTPUT_HANDLE);
	cout_tee_buf.IgnoreOutputErrors(hstdstream==INVALID_HANDLE_VALUE);
	
	//This sets fill character to '0' for all subsequent cout outputs 
	//It only matters when setw in non-zero and it is reset to zero after every output
	std::cout.fill('0');

	//This prevents base from showing in hex/oct output and hex/oct output becomes uppercase
	//N.B.: Uppercase also makes base to be displayed in uppercase (that's why showing base is disabled) but doesn't affect boolalpha flag (bool values still displayed in lowercase)
	//Applied for all subsequent cout outputs 
	std::cout.unsetf(std::ios::showbase);
	std::cout.setf(std::ios::uppercase); 

#ifdef NT3
	//Disable Windows error dialog popup for failed LoadLibrary attempts on NT3.x and Win32s
	//This is done so GetSystemFilePath can search for needed files using LoadLibrary
	SetErrorMode(SEM_NOOPENFILEERRORBOX|SEM_FAILCRITICALERRORS);
#endif
	
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
			if (!SuiteMasks.Flags(osvi_ex.wSuiteMask, [](const std::string& label, DWORD value, bool unknown, size_t idx){
				if (unknown)
					std::cout<<"\t\tUNKNOWN FLAG ("<<COUT_HEX(value, 4)<<")"<<std::endl;
				else
					std::cout<<"\t\t"<<label<<std::endl;
				return false;
			}))
				std::cout<<"\t\tNO FLAGS SET"<<std::endl;
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
	} else 
		std::cout<<"GetVersionEx/RtlGetVersion failed or not available!"<<std::endl;
	DWORD dwVersion=GetVersion();
	std::cout<<"GetVersion = "<<COUT_HEX(dwVersion, 8)<<std::endl;
	if (dwVersion) {
		std::cout<<"\tMajorVersion = "<<COUT_DEC(LOBYTE(LOWORD(dwVersion)))<<std::endl;
		std::cout<<"\tMinorVersion = "<<COUT_DEC(HIBYTE(LOWORD(dwVersion)))<<std::endl;
		//N.B.:
		//Code below is actual for Win32 API
		//On Win16 API HIWORD contains MS-DOS version, major number in LOBYTE and minor number in HIBYTE
		std::cout<<"\tIsNT = "<<COUT_BOOL((dwVersion&0x80000000)==0)<<std::endl;
		std::cout<<"\tBldNumOrRes = "<<COUT_DEC(HIWORD(dwVersion)&~0x8000)<<std::endl;	//This thing is reserved on Win 9x and build number on NT and Win32s
	}

	std::cout<<std::endl;

	std::cout<<"GetSystemMetrics:"<<std::endl;
	if (!SystemMetrics.Matches([](const std::string& label, DWORD value, size_t idx) {
		//All the system metrics reside in aiSysMet member of SERVERINFO struct (aka _gpsi)
		//SERVERINFO is a global internal system structure common for all the processes
		//aiSysMet is an int array of fixed length - nIndex passed to GetSystemMetrics actually is an index to this array
		//So passed nIndex can be potentially out of bounds and lead to memory access violation
		//NT 3.51's (and onwards) GetSystemMetrics actually checks that passed index is valid (i.e. within array)
		//But on NT 3.1 and 3.5 GetSystemMetrics just access the array with whatever index passed
		//Some small diviations from maximum index won't do many harm (aside from returning incorrect result) because they land somewhere within SERVERINFO struct
		//The struct is few kilobytes in size and aiSysMet starts somewhere within first kilobyte
		//But large deviations like 1000-th element behind real maximum will surely get you in trouble
		//Real GetSystemMetrics index range is 0-43 for NT 3.1 and 0-70 for NT 3.5 - everything else is out of bounds
#if !defined(_WIN64)&&defined(NT3)
		int result;
		if (GetSystemMetricsSehWrapper(value, &result)) {
			if (result) {
				std::cout<<"\t"<<label<<std::endl;
				return true;
			} else
				return false;
		} else {
			std::cout<<"\t"<<label<<" (ACCESS VIOLATION)"<<std::endl;
			return true;
		}
#else
		if (GetSystemMetrics(value)) {
			std::cout<<"\t"<<label<<std::endl;
			return true;
		} else
			return false;
#endif
	}))
		std::cout<<"\tREQUESTED METRICS NOT FOUND"<<std::endl;
	
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
	
	PrintFileInformation("KERNEL32.DLL", "ProductVersion");
	PrintFileInformation("USER.EXE", "ProductVersion");
	PrintFileInformation("NTKERN.VXD", "ProductVersion");
	
	cout_tee_buf.Deactivate();
	
	bool save_output=false;
	DWORD conmode;
	if (hstdstream==INVALID_HANDLE_VALUE) {
		save_output=MessageBox(NULL, "Output will be saved to VT_OUT.TXT", "vt.exe", MB_ICONINFORMATION|MB_OKCANCEL|MB_DEFBUTTON1)==IDOK;
	} else if (GetConsoleMode(hstdstream, &conmode)) {
		std::cout<<std::endl;
		std::cout<<"Press S to save output to VT_OUT.TXT or ENTER to continue..."<<std::flush;
		
		char command;
		do command=tolower(_getch()); while (command!='\r'&&command!='s'&&command!=EOF);
		
		std::cout<<std::endl;
		save_output=command=='s';
	}	
	if (save_output) {
		std::ofstream ofile("VT_OUT.TXT", std::ofstream::trunc);
		ofile<<cout_copy;
		ofile.close();
	}
	
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
	} else if (fnGetVersionExA) {
		osvi_ex.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
		if (fnGetVersionExA((LPOSVERSIONINFO)&osvi_ex)) {
			std::cout<<"GetVersionEx.OSVERSIONINFOEX:"<<std::endl;
			return true;
		} else if ((osvi_ex.dwOSVersionInfoSize=sizeof(OSVERSIONINFO), fnGetVersionExA((LPOSVERSIONINFO)&osvi_ex))) {
			std::cout<<"GetVersionEx.OSVERSIONINFO:"<<std::endl;
			return true;
		} else
			return false;
	}
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

void PrintFileInformation(const char* query_path, const char* sfi_item) 
{
	//Some Windows system files bear information that can help in detecting Windows version
	//Most of these files are PE DLLs but some are also LE VXDs
	//Microsoft vaguely suggests checking VS_FIXEDFILEINFO.FILEVERSION of VERSIONINFO resource and "date" of system files to help detect OS version
	//In reality it's better to check StringFileInfo.ProductVersion of VERSIONINFO resource and file's modified date/time
	//ProductVersion holds version info more related to OS version than FILEVERSION that holds actual file version (TODO: example for Win ME and NT4)
	//N.B.: 
	//File properties dialog on Win 9x, Win 3.x and Win NT3.x showed StringFileInfo.FileVersion in it's header (not actual VS_FIXEDFILEINFO.FILEVERSION)
	//On NT systems (starting from NT4, up to and including XP) header showed VS_FIXEDFILEINFO.FILEVERSION instead
	//Also these versions of dialog allowed to view any other (even custom) StringFileInfo items
	//Starting from Vista only limited number of VERSIONINFO items is shown
	//And only two of them is about versioning: VS_FIXEDFILEINFO.FILEVERSION (but not StringFileInfo.FileVersion) and StringFileInfo.ProductVersion
	//None of file properties dialog versions ever showed VS_FIXEDFILEINFO.PRODUCTVERSION or VS_FIXEDFILEINFO.FILEFLAGS
	
	//PE files have TimeDateStamp field in the header that contains actual build time that should match actual last modified time (if file wasn't modified)
	//But binary can be shipped modified by Microsoft and on Win 9x HH:MM:SS portion of modified time actually contains build number instead of real timestamp
	//Also PE header contains MajorImageVersion and MinorImageVersion fields
	//For system DLLs these fields frequently just duplicate OS major/minor version or have some internal version unhelpful for OS version detection
	//LE files have ModuleVersion field in the header but it's not used in any of system VXDs
	
	//"File not found" is also a result - absence/presence of specific files also helps in detecting OS version (ex: USB supplement)
	//Also see comments for FPRoutines::GetSystemFilePath to find out how queried file is searched
	
	//Most Windows files that are relevant to OS detection are system DLLs
	//GetSystemFilePath uses special set of routines to find full file path (from just relative file name) to such files
	//GetSystemFilePath also accepts absolute paths
	std::string full_path=FPRoutines::GetSystemFilePath(query_path);
	
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
				LANGANDCODEPAGE def_lcp={0x0409, 0x0};
				UINT vqvlen;
#ifdef NT3
				//On Win32s VerQueryValue is a thunk to Win16 API and this thunk doesn't work with string literals passed as LP(C)STR
				//LP(C)STR should be on the stack or heap for this thunk to work
				char vfi_translation[]="\\VarFileInfo\\Translation";
				if (!VerQueryValue((LPVOID)retbuf, vfi_translation, (LPVOID*)&plcp, &vqvlen)) {
#else
				if (!VerQueryValue((LPVOID)retbuf, "\\VarFileInfo\\Translation", (LPVOID*)&plcp, &vqvlen)) {
#endif
					//If no translations found - assume default translation
					plcp=&def_lcp;
				}
				//We are interested only in first translation - most system files have only one VERSIONINFO translation anyway
				char *value;
				std::stringstream qstr;
				qstr<<std::uppercase<<std::noshowbase<<std::hex<<std::setfill('0')<<"\\StringFileInfo\\"<<std::setw(4)<<plcp->wLanguage<<std::setw(4)<<plcp->wCodePage<<"\\"<<sfi_item;
				if (VerQueryValue((LPVOID)retbuf, qstr.str().c_str(), (LPVOID*)&value, &vqvlen)) {
					std::cout<<"\tVERSIONINFO"<<qstr.str()<<" = \""<<value<<"\""<<std::endl;
					got_info=true;
#ifdef NT3
				} else {
					//Wow, we got translation but failed at getting string for this translation?
					//And it was just simple ProductVersion
					//The thing is if we are on obsolete as hell NT 3.1 - LANGANDCODEPAGE's wLanguage and wCodePage fields may be swapped in here
					//So let's try the other way around
					qstr.str(std::string());
					qstr.clear();
					qstr<<std::uppercase<<std::noshowbase<<std::hex<<std::setfill('0')<<"\\StringFileInfo\\"<<std::setw(4)<<plcp->wCodePage<<std::setw(4)<<plcp->wLanguage<<"\\"<<sfi_item;
					if (VerQueryValue((LPVOID)retbuf, qstr.str().c_str(), (LPVOID*)&value, &vqvlen)) {
						std::cout<<"\tVERSIONINFO"<<qstr.str()<<" = \""<<value<<"\""<<std::endl;
						got_info=true;
					}
#endif
				}
			}
		}
		
		if (!got_info)
			std::cout<<"\tNO INFORMATION FOUND"<<std::endl;	
	}
}
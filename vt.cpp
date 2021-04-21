#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <fstream>
#include <conio.h>
#include <windows.h>
#include "externs.h"
#include "tee_buf.hpp"
#include "extra_defs.h"
#include "fp_routines.h"
#include "labeled_values.hpp"

extern pRtlGetVersion fnRtlGetVersion;
extern pGetNativeSystemInfo fnGetNativeSystemInfo;
extern pIsWow64Process fnIsWow64Process;
extern pGetFileVersionInfoExW fnGetFileVersionInfoExW;
extern pGetFileVersionInfoSizeExW fnGetFileVersionInfoSizeExW;
extern pGetProductInfo fnGetProductInfo;
extern pGetVersionExA fnGetVersionExA;
extern pQueryActCtxW fnQueryActCtxW;
extern pGetSecurityInfo fnGetSecurityInfo;
extern pGetKernelObjectSecurity fnGetKernelObjectSecurity;
extern pGetSecurityDescriptorOwner fnGetSecurityDescriptorOwner;
extern pLookupAccountSidA fnLookupAccountSidA;
extern pRtlGetDeviceFamilyInfoEnum fnRtlGetDeviceFamilyInfoEnum;
extern pwine_get_version fnwine_get_version;

typedef struct _LANGANDCODEPAGE {
	WORD wLanguage;
	WORD wCodePage;
} LANGANDCODEPAGE;

//std::cout treats char as, obviously, char and prints it as such - not it's numerical value
//If numerical value is needed, instead of static cast to ULONG_PTR, unary addition operator can be used to force printing numerical value
//Also, these defines only work correctly with specific std::ios::fmtflags and std::ios::fill (see below in main)
//Dec_width and hex_width is in chars
#define COUT_ADEC(dec_int)				+(dec_int)
#define COUT_FDEC(dec_int, dec_width)	std::setw(dec_width)<<+(dec_int)
#define COUT_FHEX(hex_int, hex_width)	"0x"<<std::hex<<std::setw(hex_width)<<+(hex_int)<<std::dec
#define COUT_GUID(guid)					std::hex<<std::setw(8)<<+(guid.Data1)<<"-"<<std::setw(4)<<+(guid.Data2)<<"-"<<std::setw(4)<<+(guid.Data3)<<"-"<<std::setw(2)<<+(guid.Data4[0])<<std::setw(2)<<+(guid.Data4[1])<<"-"<<std::setw(2)<<+(guid.Data4[2])<<std::setw(2)<<+(guid.Data4[3])<<std::setw(2)<<+(guid.Data4[4])<<std::setw(2)<<+(guid.Data4[5])<<std::setw(2)<<+(guid.Data4[6])<<std::setw(2)<<+(guid.Data4[7])<<std::dec
#define COUT_BOOL(bool_val)				((bool_val)?"TRUE":"FALSE")

LabeledValues PlatfromIds(LABELED_VALUES_ARG(VER_PLATFORM_WIN32s, VER_PLATFORM_WIN32_WINDOWS, VER_PLATFORM_WIN32_NT, VER_PLATFORM_WIN32_CE));
LabeledValues SuiteMasks(LABELED_VALUES_ARG(VER_WORKSTATION_NT, VER_SERVER_NT, VER_SUITE_SMALLBUSINESS, VER_SUITE_ENTERPRISE, VER_SUITE_BACKOFFICE, VER_SUITE_COMMUNICATIONS, VER_SUITE_TERMINAL, VER_SUITE_SMALLBUSINESS_RESTRICTED, VER_SUITE_EMBEDDEDNT, VER_SUITE_DATACENTER, VER_SUITE_SINGLEUSERTS, VER_SUITE_PERSONAL, VER_SUITE_BLADE, VER_SUITE_EMBEDDED_RESTRICTED, VER_SUITE_SECURITY_APPLIANCE, VER_SUITE_STORAGE_SERVER, VER_SUITE_COMPUTE_SERVER, VER_SUITE_WH_SERVER));
LabeledValues ProductTypes(LABELED_VALUES_ARG(VER_NT_WORKSTATION, VER_NT_DOMAIN_CONTROLLER, VER_NT_SERVER));
LabeledValues SystemMetrics(LABELED_VALUES_ARG(SM_FUNDAMENTALS, SM_WEPOS, SM_DEBUG, SM_PENWINDOWS, SM_DBCSENABLED, SM_IMMENABLED, SM_SLOWMACHINE, SM_TABLETPC, SM_MEDIACENTER, SM_STARTER, SM_SERVERR2));
LabeledValues ProcessorArchitectures(LABELED_VALUES_ARG(PROCESSOR_ARCHITECTURE_INTEL, PROCESSOR_ARCHITECTURE_MIPS, PROCESSOR_ARCHITECTURE_ALPHA, PROCESSOR_ARCHITECTURE_PPC, PROCESSOR_ARCHITECTURE_SHX, PROCESSOR_ARCHITECTURE_ARM, PROCESSOR_ARCHITECTURE_IA64, PROCESSOR_ARCHITECTURE_ALPHA64, PROCESSOR_ARCHITECTURE_MSIL, PROCESSOR_ARCHITECTURE_AMD64, PROCESSOR_ARCHITECTURE_IA32_ON_WIN64, PROCESSOR_ARCHITECTURE_NEUTRAL, PROCESSOR_ARCHITECTURE_UNKNOWN));
LabeledValues ProductInfoTypes(LABELED_VALUES_ARG(PRODUCT_UNDEFINED, PRODUCT_ULTIMATE, PRODUCT_HOME_BASIC, PRODUCT_HOME_PREMIUM, PRODUCT_ENTERPRISE, PRODUCT_HOME_BASIC_N, PRODUCT_BUSINESS, PRODUCT_STANDARD_SERVER, PRODUCT_DATACENTER_SERVER, PRODUCT_SMALLBUSINESS_SERVER, PRODUCT_ENTERPRISE_SERVER, PRODUCT_STARTER, PRODUCT_DATACENTER_SERVER_CORE, PRODUCT_STANDARD_SERVER_CORE, PRODUCT_ENTERPRISE_SERVER_CORE, PRODUCT_ENTERPRISE_SERVER_IA64, PRODUCT_BUSINESS_N, PRODUCT_WEB_SERVER, PRODUCT_CLUSTER_SERVER, PRODUCT_HOME_SERVER, PRODUCT_STORAGE_EXPRESS_SERVER, PRODUCT_STORAGE_STANDARD_SERVER, PRODUCT_STORAGE_WORKGROUP_SERVER, PRODUCT_STORAGE_ENTERPRISE_SERVER, PRODUCT_SERVER_FOR_SMALLBUSINESS, PRODUCT_SMALLBUSINESS_SERVER_PREMIUM, PRODUCT_UNLICENSED, PRODUCT_HOME_PREMIUM_N, PRODUCT_ENTERPRISE_N, PRODUCT_ULTIMATE_N, PRODUCT_WEB_SERVER_CORE, PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT, PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY, PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING, PRODUCT_SERVER_FOUNDATION, PRODUCT_HOME_PREMIUM_SERVER, PRODUCT_SERVER_FOR_SMALLBUSINESS_V, PRODUCT_STANDARD_SERVER_V, PRODUCT_DATACENTER_SERVER_V, PRODUCT_ENTERPRISE_SERVER_V, PRODUCT_DATACENTER_SERVER_CORE_V, PRODUCT_STANDARD_SERVER_CORE_V, PRODUCT_ENTERPRISE_SERVER_CORE_V, PRODUCT_HYPERV, PRODUCT_STORAGE_EXPRESS_SERVER_CORE, PRODUCT_STORAGE_STANDARD_SERVER_CORE, PRODUCT_STORAGE_WORKGROUP_SERVER_CORE, PRODUCT_STORAGE_ENTERPRISE_SERVER_CORE, PRODUCT_STARTER_N, PRODUCT_PROFESSIONAL, PRODUCT_PROFESSIONAL_N, PRODUCT_SB_SOLUTION_SERVER, PRODUCT_SERVER_FOR_SB_SOLUTIONS, PRODUCT_STANDARD_SERVER_SOLUTIONS, PRODUCT_STANDARD_SERVER_SOLUTIONS_CORE, PRODUCT_SB_SOLUTION_SERVER_EM, PRODUCT_SERVER_FOR_SB_SOLUTIONS_EM, PRODUCT_SOLUTION_EMBEDDEDSERVER, PRODUCT_SOLUTION_EMBEDDEDSERVER_CORE, PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE, PRODUCT_ESSENTIALBUSINESS_SERVER_MGMT, PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL, PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC, PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC, PRODUCT_CLUSTER_SERVER_V, PRODUCT_EMBEDDED, PRODUCT_STARTER_E, PRODUCT_HOME_BASIC_E, PRODUCT_HOME_PREMIUM_E, PRODUCT_PROFESSIONAL_E, PRODUCT_ENTERPRISE_E, PRODUCT_ULTIMATE_E, PRODUCT_ARM64_SERVER, PRODUCT_AZURE_NANO_SERVER, PRODUCT_AZURE_SERVER_CORE, PRODUCT_CLOUD, PRODUCT_CLOUD_HOST_INFRASTRUCTURE_SERVER, PRODUCT_CLOUD_STORAGE_SERVER, PRODUCT_CLOUDN, PRODUCT_CONNECTED_CAR, PRODUCT_CORE, PRODUCT_CORE_ARM, PRODUCT_CORE_CONNECTED, PRODUCT_CORE_CONNECTED_COUNTRYSPECIFIC, PRODUCT_CORE_CONNECTED_N, PRODUCT_CORE_CONNECTED_SINGLELANGUAGE, PRODUCT_CORE_COUNTRYSPECIFIC, PRODUCT_CORE_N, PRODUCT_CORE_SINGLELANGUAGE, PRODUCT_DATACENTER_A_SERVER_CORE, PRODUCT_DATACENTER_EVALUATION_SERVER, PRODUCT_DATACENTER_EVALUATION_SERVER_CORE, PRODUCT_DATACENTER_NANO_SERVER, PRODUCT_DATACENTER_WS_SERVER_CORE, PRODUCT_EDUCATION, PRODUCT_EDUCATION_N, PRODUCT_EMBEDDED_A, PRODUCT_EMBEDDED_AUTOMOTIVE, PRODUCT_EMBEDDED_E, PRODUCT_EMBEDDED_E_EVAL, PRODUCT_EMBEDDED_EVAL, PRODUCT_EMBEDDED_INDUSTRY, PRODUCT_EMBEDDED_INDUSTRY_A, PRODUCT_EMBEDDED_INDUSTRY_A_E, PRODUCT_EMBEDDED_INDUSTRY_E, PRODUCT_EMBEDDED_INDUSTRY_E_EVAL, PRODUCT_EMBEDDED_INDUSTRY_EVAL, PRODUCT_ENTERPRISE_EVALUATION, PRODUCT_ENTERPRISE_N_EVALUATION, PRODUCT_ENTERPRISE_S, PRODUCT_ENTERPRISE_S_EVALUATION, PRODUCT_ENTERPRISE_S_N, PRODUCT_ENTERPRISE_S_N_EVALUATION, PRODUCT_ENTERPRISE_SUBSCRIPTION, PRODUCT_ENTERPRISE_SUBSCRIPTION_N, PRODUCT_ENTERPRISEG, PRODUCT_ENTERPRISEGN, PRODUCT_HOLOGRAPHIC, PRODUCT_INDUSTRY_HANDHELD, PRODUCT_IOTUAP, PRODUCT_IOTUAPCOMMERCIAL, PRODUCT_MOBILE_CORE, PRODUCT_MOBILE_ENTERPRISE, PRODUCT_MULTIPOINT_PREMIUM_SERVER, PRODUCT_MULTIPOINT_STANDARD_SERVER, PRODUCT_NANO_SERVER, PRODUCT_PPI_PRO, PRODUCT_PRERELEASE, PRODUCT_PRERELEASE_ARM, PRODUCT_PRERELEASE_N, PRODUCT_PRO_CHINA, PRODUCT_PRO_FOR_EDUCATION, PRODUCT_PRO_FOR_EDUCATION_N, PRODUCT_PRO_SINGLE_LANGUAGE, PRODUCT_PRO_WORKSTATION, PRODUCT_PRO_WORKSTATION_N, PRODUCT_PROFESSIONAL_EMBEDDED, PRODUCT_PROFESSIONAL_PLUS, PRODUCT_PROFESSIONAL_S, PRODUCT_PROFESSIONAL_S_N, PRODUCT_PROFESSIONAL_STUDENT, PRODUCT_PROFESSIONAL_STUDENT_N, PRODUCT_PROFESSIONAL_WMC, PRODUCT_STANDARD_A_SERVER_CORE, PRODUCT_STANDARD_EVALUATION_SERVER, PRODUCT_STANDARD_EVALUATION_SERVER_CORE, PRODUCT_STANDARD_NANO_SERVER, PRODUCT_STANDARD_WS_SERVER_CORE, PRODUCT_STORAGE_STANDARD_EVALUATION_SERVER, PRODUCT_STORAGE_WORKGROUP_EVALUATION_SERVER, PRODUCT_THINPC, PRODUCT_UTILITY_VM));
LabeledValues CSDReleaseTypes(LABELED_VALUES_ARG(SP_RELEASE_TYPE_NONE, SP_RELEASE_TYPE_INTERNAL, SP_RELEASE_TYPE_RC, SP_RELEASE_TYPE_BETA));
BasicLabeledValues<HKEY> RegistryHives(LABELED_VALUES_ARG(HKEY_CLASSES_ROOT, HKEY_CURRENT_CONFIG, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE, HKEY_PERFORMANCE_DATA, HKEY_PERFORMANCE_TEXT, HKEY_USERS));
BasicLabeledValues<GUID, const GUID&> OsGuids(LABELED_VALUES_ARG(XP_CONTEXT_GUID, VISTA_CONTEXT_GUID, WIN7_CONTEXT_GUID, WIN8_CONTEXT_GUID, WIN81_CONTEXT_GUID, WIN10_CONTEXT_GUID));

void PrintRegistryKey(HKEY hive, const char* keypath, const char* value, std::function<bool(const std::string&, DWORD, BYTE*, DWORD)> custom_process=nullptr);
void PrintFileInformation(const char* query_path);
void PrintFileOwner(HANDLE hFile);
void GetSupportedOSInformationFromCompatibilityContext();
bool GetVersionWrapper(OSVERSIONINFOEX &osvi_ex);
BOOL GetFileVersionInfoWrapper(LPCSTR lpstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData);
DWORD GetFileVersionInfoSizeWrapper(LPCSTR lpstrFilename, LPDWORD lpdwHandle);

#ifdef __clang__
//Obscure clang++ bug - it reports "multiple definition" of std::setfill() and __gnu_cxx::operator==() when statically linking with libstdc++
//Observed on LLVM 3.6.2 with MinGW 4.7.2
//This is a fix for the bug
extern template std::_Setfill<char> std::setfill(char);													//caused by use of std::setfill(char)
extern template bool __gnu_cxx::operator==(const std::string::iterator&, const std::string::iterator&);	//caused by use of std::remove_if(std::string::iterator, std::string::iterator, bool (*)(char))
#endif

#if defined(X86_3X)&&!defined(_WIN64)
extern "C" bool __stdcall GetSystemMetricsSehWrapper(int nIndex, int* result);
#ifdef X86_3XAM
extern "C" char* msvcrt_name_in_use;
#endif
#endif

int main(int argc, char* argv[])
{
	Externs::MakeInstance();
	FPRoutines::FillDriveList();
	
	std::string cout_copy;
	CreateOstreamTeeBuf(std::cout, cout_tee_buf, BindStdStringAppend(cout_copy));
	HANDLE hstdstream=GetStdHandle(STD_OUTPUT_HANDLE);
	cout_tee_buf.IgnoreOutputErrors(hstdstream==INVALID_HANDLE_VALUE);
	
	//This sets fill character to '0' for all subsequent cout outputs 
	//It only matters when setw is non-zero and it is reset to zero after every output
	std::cout.fill('0');

	//This prevents base from showing in hex/oct output and hex/oct output becomes uppercase
	//N.B.: Uppercase also makes base to be displayed in uppercase (that's why showing base is disabled) but doesn't affect boolalpha flag (bool values still displayed in lowercase)
	//Applied for all subsequent cout outputs 
	std::cout.unsetf(std::ios::showbase);
	std::cout.setf(std::ios::uppercase); 
	
#ifndef X86_3XAM
	//Disable Windows error dialog popup for failed LoadLibrary attempts on NT3.x and Win32s
	//This is done so GetSystemFilePath can search for needed files using LoadLibrary
	//In case of X86_3XAM the same is done earlier in compat.cpp/CompatCRTStartup
	SetErrorMode(SEM_NOOPENFILEERRORBOX|SEM_FAILCRITICALERRORS);
#endif

	OSVERSIONINFOEX osvi_ex;
	if (GetVersionWrapper(osvi_ex)) {
		std::cout<<"\tdwMajorVersion = "<<COUT_ADEC(osvi_ex.dwMajorVersion)<<std::endl;
		std::cout<<"\tdwMinorVersion = "<<COUT_ADEC(osvi_ex.dwMinorVersion)<<std::endl;
		//On Win 9x dwBuildNumber contains not only build number, but also major and minor version
		std::cout<<"\tdwBuildNumber = "<<COUT_ADEC(osvi_ex.dwBuildNumber)<<" = "<<COUT_ADEC(HIBYTE(HIWORD(osvi_ex.dwBuildNumber)))<<"."<<COUT_ADEC(LOBYTE(HIWORD(osvi_ex.dwBuildNumber)))<<"."<<COUT_ADEC((DWORD)LOWORD(osvi_ex.dwBuildNumber))<<std::endl;
		std::cout<<"\tdwPlatformId = "<<COUT_FHEX(osvi_ex.dwPlatformId, 8)<<std::endl;
		PlatfromIds.Enums(osvi_ex.dwPlatformId, [](const std::string& label, DWORD value, bool unknown){
			if (unknown)
				std::cout<<"\t\tUNKNOWN VALUE ("<<COUT_FHEX(value, 8)<<")"<<std::endl;
			else
				std::cout<<"\t\t"<<label<<std::endl;
			return false;
		});
		//CSD version is an alias for Service Pack version
		//CSD actually stands for "Corrective Service Diskette" - a term coined for OS/2 method of distributing hotfixes
		//These CSDs were versioned accordingly and could contain not only hotfixes, but also what was later called "ServicePak" or "FixPak"
		//It seems that, because Microsoft was developing early versions of OS/2 in parallel with Windows, this term just stucked
		std::cout<<"\tszCSDVersion = \""<<COUT_ADEC(osvi_ex.szCSDVersion)<<"\""<<std::endl;
		if (osvi_ex.dwOSVersionInfoSize==sizeof(OSVERSIONINFOEX)) {
			std::cout<<"\twServicePackMajor = "<<COUT_ADEC(osvi_ex.wServicePackMajor)<<std::endl;
			std::cout<<"\twServicePackMinor = "<<COUT_ADEC(osvi_ex.wServicePackMinor)<<" = '";
			if (osvi_ex.wServicePackMinor) std::cout<<(char)('A'+osvi_ex.wServicePackMinor-1);
			std::cout<<"'"<<std::endl;
			std::cout<<"\twSuiteMask = "<<COUT_FHEX(osvi_ex.wSuiteMask, 4)<<std::endl;
			if (!SuiteMasks.Flags(osvi_ex.wSuiteMask, [](const std::string& label, DWORD value, bool unknown){
				if (unknown)
					std::cout<<"\t\tUNKNOWN FLAG ("<<COUT_FHEX(value, 4)<<")"<<std::endl;
				else
					std::cout<<"\t\t"<<label<<std::endl;
				return false;
			}))
				std::cout<<"\t\tNO FLAGS SET"<<std::endl;
			std::cout<<"\twProductType = "<<COUT_FHEX(osvi_ex.wProductType, 2)<<std::endl;
			ProductTypes.Enums(osvi_ex.wProductType, [](const std::string& label, DWORD value, bool unknown){
				if (unknown)
					std::cout<<"\t\tUNKNOWN VALUE ("<<COUT_FHEX(value, 2)<<")"<<std::endl;
				else
					std::cout<<"\t\t"<<label<<std::endl;
				return false;
			});
			std::cout<<"\twReserved = "<<COUT_FHEX(osvi_ex.wReserved, 2)<<std::endl;
		}
	} else 
		std::cout<<"GetVersionEx/RtlGetVersion failed or not available!"<<std::endl;
		
	std::cout<<std::endl;

	if (DWORD dwVersion=GetVersion()) {
		//This information is mostly taken from MSDN Library October 1999 and Win NT source code
		//Newer MSDN versions omit non-NT related info for GetVersion
		
		std::cout<<"GetVersion = "<<COUT_FHEX(dwVersion, 8)<<std::endl;
		std::cout<<"\tMajorVersion = "<<COUT_ADEC(LOBYTE(LOWORD(dwVersion)))<<std::endl;
		std::cout<<"\tMinorVersion = "<<COUT_ADEC(HIBYTE(LOWORD(dwVersion)))<<std::endl;
		//This is just OSVERSIONINFO(EX).dwPlatformId XORed with 0x2
		std::cout<<"\tBogusPlatformId = "<<COUT_FHEX((dwVersion>>30)&0x3, 1)<<std::endl;
		PlatfromIds.Enums((dwVersion>>30)^0x2, [](const std::string& label, DWORD value, bool unknown){
			if (unknown)
				std::cout<<"\t\tUNKNOWN VALUE ("<<COUT_FHEX(value, 8)<<")"<<std::endl;
			else
				std::cout<<"\t\t"<<label<<std::endl;
			return false;
		});
		//This thing is "reserved" on Win 9x and "build number" on NT and Win32s
		std::cout<<"\tBuildNumber = "<<COUT_ADEC(HIWORD(dwVersion)&0x3FFF)<<std::endl;
		
		//N.B.:
		//HIWORD(dwVersion) treatment above is actual for Win32 API
		//On Win16 API HIWORD(dwVersion) contains MS-DOS version with major number in LOBYTE and minor number in HIBYTE
	} else {
		std::cout<<"GetVersion failed!"<<std::endl;
	}

	std::cout<<std::endl;

	std::cout<<"GetSystemMetrics:"<<std::endl;
	if (!SystemMetrics.Matches([](const std::string& label, DWORD value) {
		//On NT all the system metrics reside in aiSysMet member of SERVERINFO struct (aka _gpsi)
		//SERVERINFO is a global internal system structure common for all the processes
		//aiSysMet is an int array of fixed length - nIndex passed to GetSystemMetrics actually is an index to this array
		//So passed nIndex can be potentially out of bounds and lead to memory access violation
		//NT 3.51's (and onwards) GetSystemMetrics actually checks that passed index is valid (i.e. within array)
		//But on NT 3.1 and 3.5 GetSystemMetrics just access the array with whatever index passed
		//Some small diviations from maximum index won't do much harm (aside from returning incorrect result) because they land somewhere within SERVERINFO struct
		//The struct is few kilobytes in size and aiSysMet starts somewhere within first kilobyte
		//But large deviations like 1000-th element behind real maximum will surely get you in trouble
		//Real GetSystemMetrics index range is 0-43 for NT 3.1 and 0-70 for NT 3.5 - everything else is out of bounds
		//On Win32s and 9x/Me system metrics also reside in system array but here GetSystemMetrics always checks index validness
#if !defined(_WIN64)&&defined(X86_3X)
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
	std::cout<<".wProcessorArchitecture = "<<COUT_FHEX(sys_inf.wProcessorArchitecture, 4)<<std::endl;
	ProcessorArchitectures.Enums(sys_inf.wProcessorArchitecture, [](const std::string& label, DWORD value, bool unknown){
		if (unknown)
			std::cout<<"\tUNKNOWN VALUE ("<<COUT_FHEX(value, 4)<<")"<<std::endl;
		else
			std::cout<<"\t"<<label<<std::endl;
		return false;
	});
	
	std::cout<<std::endl;
	
	if (fnGetProductInfo) {
		DWORD dwProdType;
		if (fnGetProductInfo(osvi_ex.dwMajorVersion, osvi_ex.dwMinorVersion, osvi_ex.wServicePackMajor, osvi_ex.wServicePackMinor, &dwProdType)) {
			std::cout<<"GetProductInfo = "<<COUT_FHEX(dwProdType, 8)<<std::endl;
			ProductInfoTypes.Enums(dwProdType, [](const std::string& label, DWORD value, bool unknown){
				if (unknown)
					std::cout<<"\tUNKNOWN VALUE ("<<COUT_FHEX(value, 8)<<")"<<std::endl;
				else
					std::cout<<"\t"<<label<<std::endl;
				return false;
			});
		} else {
			std::cout<<"GetProductInfo failed!"<<std::endl;
		}
	} else {
		std::cout<<"Can't load GetProductInfo from kernel32.dll!"<<std::endl;
	}
	
	std::cout<<std::endl;
	
	if (fnRtlGetDeviceFamilyInfoEnum) {
		ULONGLONG UAPInfo=0;
		ULONG DeviceFamily;
		ULONG DeviceForm;
		fnRtlGetDeviceFamilyInfoEnum(&UAPInfo, &DeviceFamily, &DeviceForm);
		if (UAPInfo) {
			std::cout<<"RtlGetDeviceFamilyInfoEnum.UAPInfo = "<<COUT_FHEX(UAPInfo, 16)<<std::endl;
			std::cout<<"\tMajorVersion = "<<COUT_ADEC((UAPInfo&0xFFFF000000000000)>>48)<<std::endl;
			std::cout<<"\tMinorVersion = "<<COUT_ADEC((UAPInfo&0x0000FFFF00000000)>>32)<<std::endl;
			std::cout<<"\tBuildNumber = "<<COUT_ADEC((UAPInfo&0x00000000FFFF0000)>>16)<<std::endl;
			//Update Build Revision
			//This, with BuildNumber, fully qualifies Windows 10 build which is BuildNumber.UBR
			std::cout<<"\tUBR = "<<COUT_ADEC(UAPInfo&0x000000000000FFFF)<<std::endl;
		} else {
			std::cout<<"RtlGetDeviceFamilyInfoEnum failed!"<<std::endl;
		}
	} else {
		std::cout<<"Can't load RtlGetDeviceFamilyInfoEnum from ntdll.dll!"<<std::endl;
	}
	
	std::cout<<std::endl;
	
	if (fnwine_get_version) {
		//Test for Wine
		std::cout<<"wine_get_version = \""<<fnwine_get_version()<<"\""<<std::endl;
	} else {
		std::cout<<"Can't load wine_get_version from ntdll.dll!"<<std::endl;
	}
	
	std::cout<<std::endl;
	
#if !defined(_WIN64)&&defined(X86_3XAM)
	std::cout<<"Microsoft C Runtime Library = \""<<msvcrt_name_in_use<<"\""<<std::endl;
#else
	std::cout<<"Microsoft C Runtime Library = \"MSVCRT.DLL\""<<std::endl;
#endif
	
#ifdef _WIN64
	std::cout<<"This binary is built for x86-64 arch"<<std::endl;
#else
	std::cout<<"This binary is built for x86 arch"<<std::endl;
#endif

	if (fnIsWow64Process) {	
		//Test if current process is running under WOW64
		BOOL wow64;
		if (fnIsWow64Process(GetCurrentProcess(), &wow64))
			std::cout<<"IsWow64Process = "<<COUT_BOOL(wow64)<<std::endl;
		else
			std::cout<<"IsWow64Process failed!"<<std::endl;
	} else
		std::cout<<"Can't load IsWow64Process from kernel32.dll!"<<std::endl;
	
	GetSupportedOSInformationFromCompatibilityContext();
	
	std::cout<<std::endl;
	
	//There are tons of registry entries that can be used to detect specific service pack, release, edition or suite variations not covered by functions above
	//They are often exclusive to some specific minor version and have no meaning or absent on other versions
	//Also note that some of the CurrentVersion register entries contain the same info that is returned by GetVersion variations
	//And remember that, unlike values returned by GetVersion variations, registry values are user editable
	//Below are most common values that will just give you a general idea of what OS version you are runnning
	PrintRegistryKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ProductName");
	PrintRegistryKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "BuildLab");
	PrintRegistryKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "BuildLabEx");
	PrintRegistryKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ReleaseId");
	PrintRegistryKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "UBR");
	//Service Pack version in fully qualified string form
	PrintRegistryKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "CSDVersion");
	//Service Pack release type
	PrintRegistryKey(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Windows", "CSDReleaseType", [](const std::string& rpath, DWORD key_type, BYTE *regbuf, DWORD buflen){
		if (key_type==REG_DWORD) {
			std::cout<<rpath<<":\n\t"<<COUT_FHEX(*(DWORD*)regbuf, 8)<<std::endl;
			CSDReleaseTypes.Enums(*(DWORD*)regbuf, [](const std::string& label, DWORD value, bool unknown){
				if (unknown)
					std::cout<<"\tUNKNOWN VALUE ("<<COUT_FHEX(value, 8)<<")"<<std::endl;
				else
					std::cout<<"\t"<<label<<std::endl;
				return false;
			});
			return true;
		} else {
			return false;
		}
	});
	//This is a Service Pack version in it's original DWORD form, direct copy of internal CmNtCSDVersion variable
	//OSVERSIONINFOEX.wServicePackMajor/wServicePackMinor values are populated using the same variable
	//Also, if you really need it, starting from NT4 it's copy is also available in PEB.OSCSDVersion
	PrintRegistryKey(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Windows", "CSDVersion", [](const std::string& rpath, DWORD key_type, BYTE *regbuf, DWORD buflen){
		if (key_type==REG_DWORD) {
			std::cout<<rpath<<":\n\t"<<COUT_FHEX(*(DWORD*)regbuf, 8)<<std::endl;
			//Lower-order 16-bits of CSDVersion is an actual Service Pack version
			//In it's string form, minor portion is appended to major as an uppercase letter (e.g. 3.1 becomes "3A")
			std::cout<<"\tMajor = "<<COUT_ADEC(HIBYTE(LOWORD(*(DWORD*)regbuf)))<<std::endl;
			std::cout<<"\tMinor = "<<COUT_ADEC(LOBYTE(LOWORD(*(DWORD*)regbuf)))<<" = '";
			if (LOBYTE(LOWORD(*(DWORD*)regbuf))) std::cout<<(char)('A'+LOBYTE(LOWORD(*(DWORD*)regbuf))-1);
			std::cout<<"'"<<std::endl;
			//High-order 16-bits of CSDVersion is a Service Pack build number (the same as in CSDBuildNumber), but actual interpretation depends on CSDReleaseType
			//It's not used (and should be empty) for SP_RELEASE_TYPE_NONE
			//For SP_RELEASE_TYPE_INTERNAL this is really a build number
			//For SP_RELEASE_TYPE_RC and SP_RELEASE_TYPE_BETA this is RC/BETA version string in the form of "major.minor"
			std::cout<<"\tBuild = "<<COUT_ADEC(HIWORD(*(DWORD*)regbuf))<<" = "<<COUT_ADEC(HIBYTE(HIWORD(*(DWORD*)regbuf)))<<"."<<COUT_ADEC(LOBYTE(HIWORD(*(DWORD*)regbuf)))<<std::endl;
			return true;
		} else {
			return false;
		}
	});
	PrintRegistryKey(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\ProductOptions", "ProductType");
	PrintRegistryKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion", "ProductName");
	
	std::cout<<std::endl;
	
	//As with registry entries there can be various files that can hint on specific OS version variations which are otherwise impossible to detect with functions above
	//Below are just most common ones used in version detection
	PrintFileInformation("KERNEL32.DLL");
	PrintFileInformation("USER.EXE");
#if !defined(_WIN64)&&defined(X86_3XAM)
	PrintFileInformation(msvcrt_name_in_use);
#else
	PrintFileInformation("MSVCRT.DLL");
#endif
	//NTKERN.VXD and VMM.VXD are installed by "USB Supplement for OSR2"
	//You can check either of them - both report identical file info
	PrintFileInformation("NTKERN.VXD");
	
	std::cout.flush();
	cout_tee_buf.Deactivate();
	
	bool save_output=false;
	DWORD conmode;
	if (hstdstream==INVALID_HANDLE_VALUE) {
		//If standard output stream handle is invalid, chances are input stream is also invalid, so obviously we can't get input from console
		save_output=MessageBox(NULL, "Output will be saved to VT_OUT.TXT.", argv[0], MB_ICONINFORMATION|MB_OKCANCEL|MB_DEFBUTTON1)==IDOK;
	} else if (GetConsoleMode(hstdstream, &conmode)&&!fnwine_get_version) {
		//Under Wine _getch() may not work properly and always return EOF (depends on Wine version)
		//Also, even when GetConsoleMode succeeds here, it doesn't mean that standard input stream is attached to actual console
		//So, for the consistency under Wine, do not ask to save output here
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
		//RTL_OSVERSIONINFOEXW and OSVERSIONINFOEXW are essentially identical structures
		//Internally non-kernel RtlGetVersion actually compares dwOSVersionInfoSize to OSVERSIONINFOEXW and not RTL_OSVERSIONINFOEXW 
		OSVERSIONINFOEXW rtl_osvi_ex={sizeof(OSVERSIONINFOEXW)};
		if (NT_SUCCESS(fnRtlGetVersion((PRTL_OSVERSIONINFOW)&rtl_osvi_ex))||(rtl_osvi_ex.dwOSVersionInfoSize=sizeof(OSVERSIONINFOW), NT_SUCCESS(fnRtlGetVersion((PRTL_OSVERSIONINFOW)&rtl_osvi_ex)))) {
			//Humorously enough, on Win 2k RtlGetVersion doesn't populate szCSDVersion, leaving this task to GetVersionEx
			if (*rtl_osvi_ex.szCSDVersion) {
				WideCharToMultiByte(CP_ACP, 0, rtl_osvi_ex.szCSDVersion, sizeof(((OSVERSIONINFOEXW*)NULL)->szCSDVersion)/sizeof(wchar_t), osvi_ex.szCSDVersion, sizeof(((OSVERSIONINFOEXW*)NULL)->szCSDVersion), NULL, NULL);
			} else if (fnGetVersionExA) {
				osvi_ex.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
				fnGetVersionExA((LPOSVERSIONINFO)&osvi_ex);
			}
			osvi_ex.dwMajorVersion=rtl_osvi_ex.dwMajorVersion;
			osvi_ex.dwMinorVersion=rtl_osvi_ex.dwMinorVersion;
			osvi_ex.dwBuildNumber=rtl_osvi_ex.dwBuildNumber;
			osvi_ex.dwPlatformId=rtl_osvi_ex.dwPlatformId;
			if (rtl_osvi_ex.dwOSVersionInfoSize==sizeof(OSVERSIONINFOEXW)) {
				std::cout<<"RtlGetVersion.OSVERSIONINFOEX:"<<std::endl;
				osvi_ex.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
				osvi_ex.wServicePackMajor=rtl_osvi_ex.wServicePackMajor;
				osvi_ex.wServicePackMinor=rtl_osvi_ex.wServicePackMinor;
				osvi_ex.wSuiteMask=rtl_osvi_ex.wSuiteMask;
				osvi_ex.wProductType=rtl_osvi_ex.wProductType;
				osvi_ex.wReserved=rtl_osvi_ex.wReserved;
			} else {
				std::cout<<"RtlGetVersion.OSVERSIONINFO:"<<std::endl;
				osvi_ex.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
			}
			return true;
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

void PrintRegistryKey(HKEY hive, const char* keypath, const char* value, std::function<bool(const std::string&, DWORD, BYTE*, DWORD)> custom_process) 
{
	HKEY reg_key;
	DWORD buflen;
	DWORD key_type;
	bool success=false;
	
	//KEY_WOW64_64KEY flag causes pre-XP NT versions to fail
	if (RegOpenKeyEx(hive, keypath, 0, KEY_READ|KEY_WOW64_64KEY, &reg_key)==ERROR_SUCCESS||RegOpenKeyEx(hive, keypath, 0, KEY_READ, &reg_key)==ERROR_SUCCESS) {
		if (RegQueryValueEx(reg_key, value, NULL, &key_type, NULL, &buflen)==ERROR_SUCCESS) {
			BYTE regbuf[buflen+sizeof(char)];	//+1 char for possible missing NULL terminator in case of REG_SZ and REG_EXPAND_SZ
			if (RegQueryValueEx(reg_key, value, NULL, &key_type, regbuf, &buflen)==ERROR_SUCCESS) {
				if (custom_process) {
					//Custom registry entry processing
					//E.g. you can add support for REG_MULTI_SZ if needed
					std::ostringstream rpath;
					rpath<<RegistryHives(hive)<<"\\"<<keypath<<"\\"<<value;
					success=custom_process(rpath.str(), key_type, regbuf, buflen);
				} else switch (key_type) {
					case REG_SZ:
					case REG_EXPAND_SZ:
						//It's not guaranteed that buffer returned by RegQueryValueEx is NULL-terminated
						//RegQueryValueEx fails if buffer is too small and it is already one byte bigger than needed, so NULL-terminating using buffer length as index won't cause access violation
						regbuf[buflen]='\0';
						std::cout<<RegistryHives(hive)<<"\\"<<keypath<<"\\"<<value<<":\n\t\""<<(char*)regbuf<<"\""<<std::endl;
						success=true;
						break;
					case REG_DWORD:
						std::cout<<RegistryHives(hive)<<"\\"<<keypath<<"\\"<<value<<":\n\t"<<COUT_ADEC(*(DWORD*)regbuf)<<" = "<<COUT_FHEX(*(DWORD*)regbuf, 8)<<std::endl;
						success=true;
						break;
					case REG_QWORD:
						std::cout<<RegistryHives(hive)<<"\\"<<keypath<<"\\"<<value<<":\n\t"<<COUT_ADEC(*(DWORD*)regbuf)<<" = "<<COUT_FHEX(*(ULONGLONG*)regbuf, 16)<<std::endl;
						success=true;
						break;
				}
			}
		}
		RegCloseKey(reg_key);
	}
	
	if (!success) 
		std::cout<<RegistryHives(hive)<<"\\"<<keypath<<"\\"<<value<<" - can't read registry value!"<<std::endl;
}

BOOL GetFileVersionInfoWrapper(LPCSTR lpstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData)
{
	//StringFileInfo is considered non-fixed part of VERSIONINFO
	//If GetFileVersionInfo finds a MUI file for the file it is currently querying, it will use StringFileInfo from this file instead of original one
	//So information can differ between what Explorer shows (actual file StringFileInfo) and what GetFileVersionInfo retreives
	//VS_FIXEDFILEINFO remains unaffected
	//The trick is to use GetFileVersionInfoEx with FILE_VER_GET_NEUTRAL flag to get StringFileInfo from actual file and not from MUI
	//GetFileVersionInfoEx is available since Vista and only in UNICODE version
	
	if (fnGetFileVersionInfoExW&&fnGetFileVersionInfoSizeExW) {
		if (int chars_num=MultiByteToWideChar(CP_ACP, 0, lpstrFilename, -1, NULL, 0)) {
			wchar_t w_fname[chars_num];
			if (MultiByteToWideChar(CP_ACP, 0, lpstrFilename, -1, w_fname, chars_num)) {
				return fnGetFileVersionInfoExW(FILE_VER_GET_NEUTRAL, w_fname, dwHandle, dwLen, lpData);
			}
		}
			
		return FALSE;
	} else
		return GetFileVersionInfo(lpstrFilename, dwHandle, dwLen, lpData);
}

DWORD GetFileVersionInfoSizeWrapper(LPCSTR lpstrFilename, LPDWORD lpdwHandle)
{
	//See comments on GetFileVersionInfoWrapper
	
	if (fnGetFileVersionInfoExW&&fnGetFileVersionInfoSizeExW) {
		if (int chars_num=MultiByteToWideChar(CP_ACP, 0, lpstrFilename, -1, NULL, 0)) {
			wchar_t w_fname[chars_num];
			if (MultiByteToWideChar(CP_ACP, 0, lpstrFilename, -1, w_fname, chars_num)) {
				return fnGetFileVersionInfoSizeExW(FILE_VER_GET_NEUTRAL, w_fname, lpdwHandle);
			}
		}
			
		return FALSE;
	} else
		return GetFileVersionInfoSize(lpstrFilename, lpdwHandle);
}

void PrintFileInformation(const char* query_path) 
{
	//Some Windows system files bear information that can help in detecting Windows version
	//Most of these files are PE DLLs but some are also LE VXDs
	//Microsoft vaguely suggests checking VS_FIXEDFILEINFO.FILEVERSION and StringFileInfo.ProductVersion of VERSIONINFO resource and "date" of system files to help detect OS version
	//In reality the best option here seems to check StringFileInfo.ProductVersion and file's modified date/time
	//ProductVersion holds version info more correlated with OS version than FILEVERSION that holds actual file version
	//For example, kernel32.dll VERSIONINFO: 
	//  NT4: FILEVERSION=4.0.1381.7095 ProductVersion="4.00"
	//  Win98: FILEVERSION=4.10.0.2222 ProductVersion="4.10.2222"
	//Though FILEVERSION is still useful because it doesn't require parsing
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
	
	//"File not found" is also a result - absence/presence of specific files also helps in detecting OS version
	//For example, NTKERN.VXD is only found on Win95 versions that have USB Supplement installed 
	//Also see comments for FPRoutines::GetSystemFilePath to find out how queried file is searched
	
	//Most Windows files that are relevant to OS detection are system DLLs
	//FPRoutines::GetSystemFilePath uses special set of routines to find full file path (from just relative file name) to such files
	//FPRoutines::GetSystemFilePath also accepts absolute paths
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
				std::cout<<"\tGetFileTime.lpLastWriteTime = "<<COUT_FDEC(st_local.wDay, 2)<<"/"<<COUT_FDEC(st_local.wMonth, 2)<<"/"<<COUT_FDEC(st_local.wYear, 2)<<" "<<COUT_FDEC(st_local.wHour, 2)<<":"<<COUT_FDEC(st_local.wMinute, 2)<<":"<<COUT_FDEC(st_local.wSecond, 2)<<std::endl;
				got_info=true;
			}
			
			PrintFileOwner(hFile);
			
			CloseHandle(hFile); 
		}
		
		//Interesting thing is that GetFileVersionInfo family of functions uses LoadLibraryEx(LOAD_LIBRARY_AS_DATAFILE) on NT and SearchPath on 9x to get absolute path from relative one
		//Not far from what FPRoutines::GetSystemFilePath does internally
		//But we need to show user actual absolute path for supplied file name and also make sure that the same file will be used with CreateFile/GetFileTime
		//That's why we use FPRoutines::GetSystemFilePath
		if (DWORD buflen=GetFileVersionInfoSizeWrapper(full_path.c_str(), NULL)) {
			BYTE vibuf[buflen];
			if (GetFileVersionInfoWrapper(full_path.c_str(), 0, buflen, (LPVOID)vibuf)) {
				UINT vqvlen;
				VS_FIXEDFILEINFO *pffi;
#ifdef X86_3X
				//On Win32s VerQueryValue is a thunk to Win16 API and this thunk doesn't work with string literals passed as LPCSTR
				//LPCSTR should be on the stack or heap for this thunk to work
				char vi_root[]="\\";
				if (VerQueryValue((LPVOID)vibuf, vi_root, (LPVOID*)&pffi, &vqvlen)) {
#else
				if (VerQueryValue((LPVOID)vibuf, "\\", (LPVOID*)&pffi, &vqvlen)) {
#endif
					//VERSIONINFO.FILEVERSION consists of four parts: Major.Minor.Build.Private
					//On Win 10 if file is owned by TrustedInstaller and FILEVERSION=10.0.*.*, same manifest OS compatibility rules apply as for GetVersion/GetVersionEx
					//E.g. if maximum supported OS, declared in manifest compatibility section, is Win 8.1, then FILEVERSION major and minor part will be set to 6 and 3 accordingly
					//VERSIONINFO.PRODUCTVERSION is not affected by such behaviour
					std::cout<<"\tVERSIONINFO\\FILEVERSION = "<<COUT_ADEC(HIWORD(pffi->dwFileVersionMS))<<"."<<COUT_ADEC(LOWORD(pffi->dwFileVersionMS))<<"."<<COUT_ADEC(HIWORD(pffi->dwFileVersionLS))<<"."<<COUT_ADEC(LOWORD(pffi->dwFileVersionLS))<<std::endl;
					got_info=true;
				}

				LANGANDCODEPAGE *plcp;
#ifdef X86_3X
				char vfi_translation[]="\\VarFileInfo\\Translation";
				if (VerQueryValue((LPVOID)vibuf, vfi_translation, (LPVOID*)&plcp, &vqvlen)) {
#else
				if (VerQueryValue((LPVOID)vibuf, "\\VarFileInfo\\Translation", (LPVOID*)&plcp, &vqvlen)) {
#endif
					//We are interested only in first translation - most system files have only one VERSIONINFO translation anyway (yeah, even on non-English releases)
					//For applications that query StringFileInfo structs from wider variety of files it's probably better to select translation based on some priority list (or just show user all the available translations)
					//But again here we are delaing specifically with system files to get OS version related information

					char *value;
					std::ostringstream qstr;
					qstr<<std::uppercase<<std::noshowbase<<std::hex<<std::setfill('0')<<"\\StringFileInfo\\"<<std::setw(4)<<plcp->wLanguage<<std::setw(4)<<plcp->wCodePage<<"\\ProductVersion";
					if (VerQueryValue((LPVOID)vibuf, qstr.str().c_str(), (LPVOID*)&value, &vqvlen)) {
						std::cout<<"\tVERSIONINFO"<<qstr.str()<<" = \""<<value<<"\""<<std::endl;
						got_info=true;
#ifdef X86_3X
					} else {
						//Wow, we got translation but failed at getting string for this translation?
						//And it was just simple ProductVersion
						//The thing is if we are on obsolete as hell NT 3.1 - LANGANDCODEPAGE's wLanguage and wCodePage fields may be swapped in here
						//So let's try the other way around
						qstr.str(std::string());
						qstr.clear();
						qstr<<"\\StringFileInfo\\"<<std::setw(4)<<plcp->wCodePage<<std::setw(4)<<plcp->wLanguage<<"\\ProductVersion";
						if (VerQueryValue((LPVOID)vibuf, qstr.str().c_str(), (LPVOID*)&value, &vqvlen)) {
							std::cout<<"\tVERSIONINFO"<<qstr.str()<<" = \""<<value<<"\""<<std::endl;
							got_info=true;
						}
#endif
					}
				}
			}
		}
		
		if (!got_info)
			std::cout<<"\tNO INFORMATION FOUND"<<std::endl;	
	}
}

void GetSupportedOSInformationFromCompatibilityContext() {
	if (fnQueryActCtxW) {
		HANDLE act_ctx_handle=NULL;
		SIZE_T ret_size=0;
		if (fnQueryActCtxW(0, act_ctx_handle, NULL, CompatibilityInformationInActivationContext, NULL, 0, &ret_size)||GetLastError()==ERROR_INSUFFICIENT_BUFFER) {
			bool found_guid=false;			
			
			std::cout<<"QueryActCtxW.ACTIVATION_CONTEXT_COMPATIBILITY_INFORMATION:"<<std::endl;
					
			if (ret_size) {
				ACTIVATION_CONTEXT_COMPATIBILITY_INFORMATION *ctx_compat_info=(ACTIVATION_CONTEXT_COMPATIBILITY_INFORMATION*)new BYTE[ret_size];			
				if (fnQueryActCtxW(0, act_ctx_handle, NULL, CompatibilityInformationInActivationContext, ctx_compat_info, ret_size, &ret_size)) {
					for (DWORD idx=0; idx<ctx_compat_info->ElementCount; idx++) {
						if (ctx_compat_info->Elements[idx].Type==ACTCTX_COMPATIBILITY_ELEMENT_TYPE_OS) {
							found_guid=true;
							OsGuids.Enums(ctx_compat_info->Elements[idx].Id, [](const std::string& label, const GUID& value, bool unknown){
								if (unknown)
									std::cout<<"\tUNKNOWN OS GUID ("<<COUT_GUID(value)<<")"<<std::endl;
								else
									std::cout<<"\t"<<label<<std::endl;
								return false;
							});
						}
					}
				}
				delete[] (BYTE*)ctx_compat_info;
			}
			
			if (!found_guid)
				std::cout<<"\tOS CONTEXT GUIDS NOT FOUND"<<std::endl;
		} else
			std::cout<<"QueryActCtxW failed!"<<std::endl;
	} else
		std::cout<<"Can't load QueryActCtxW from kernel32.dll!"<<std::endl;		
}

void PrintFileOwner(HANDLE hFile)
{
	if (fnLookupAccountSidA) {
		PSID pSidOwner;
		auto fnLookupAccountSidOwner=[&pSidOwner](){
			DWORD account_len=0;
			DWORD domain_len=0;
			SID_NAME_USE sid_type;
			//When LookupAccountSid fails because buffer is not large enough - returned buffer length includes NULL-terminator
			if (fnLookupAccountSidA(NULL, pSidOwner, NULL, &account_len, NULL, &domain_len, &sid_type)==FALSE&&account_len&&domain_len) {
				char account[account_len];
				char domain[domain_len];
				//When LookupAccountSid succeeds - returned buffer length doesn't include NULL-terminator
				if (fnLookupAccountSidA(NULL, pSidOwner, account, &account_len, domain, &domain_len, &sid_type)) {
#ifdef X86_3X
					//Early versions of NT may return empty domain name in case of BUILTIN domain
					//If everything went ok LookupAccountSid returns actual number of written charactes (w/o NULL-terminator) in cchName and cchReferencedDomainName
					if (domain_len)
						std::cout<<"\tOWNER_SECURITY_INFORMATION = \""<<domain<<"\\"<<account<<"\""<<std::endl;
					else
						std::cout<<"\tOWNER_SECURITY_INFORMATION = \""<<account<<"\""<<std::endl;
#else
					std::cout<<"\tOWNER_SECURITY_INFORMATION = \""<<domain<<"\\"<<account<<"\""<<std::endl;
#endif
				}
			}
		};
		
		//GetSecurityInfo method works only on WinNT4+
		if (fnGetSecurityInfo) {
			PSECURITY_DESCRIPTOR pSD;
			if (fnGetSecurityInfo(hFile, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, &pSidOwner, NULL, NULL, NULL, &pSD)==ERROR_SUCCESS) {
				fnLookupAccountSidOwner();
				LocalFree(pSD);
			}
		//GetKernelObjectSecurity/GetSecurityDescriptorOwner method works on Win NT31+ but is potentially unsafe on Win 2000+ - fortunately, code above will work on Win 2000+ instead
		} else if (fnGetKernelObjectSecurity&&fnGetSecurityDescriptorOwner) {
			DWORD sz=0;
			if (fnGetKernelObjectSecurity(hFile, OWNER_SECURITY_INFORMATION, NULL, 0, &sz)==FALSE&&sz) {
				BYTE sd[sz];
				if (fnGetKernelObjectSecurity(hFile, OWNER_SECURITY_INFORMATION, (PSECURITY_DESCRIPTOR)&sd, sz, &sz)) {
					BOOL def;
					if (fnGetSecurityDescriptorOwner((PSECURITY_DESCRIPTOR)&sd, &pSidOwner, &def))
						fnLookupAccountSidOwner();
				}
			}
		}
		//No way to get file owner for Win 9x or Win32s
	}
}

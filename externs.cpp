#include "externs.h"

pRtlGetVersion fnRtlGetVersion=NULL;
pGetNativeSystemInfo fnGetNativeSystemInfo=NULL;
pIsWow64Process fnIsWow64Process=NULL;
pGetFileVersionInfoExW fnGetFileVersionInfoExW=NULL;
pGetFileVersionInfoSizeExW fnGetFileVersionInfoSizeExW=NULL;
pGetProductInfo fnGetProductInfo=NULL;
pGetVersionExA fnGetVersionExA=NULL;
pNtCreateFile fnNtCreateFile=NULL;
pNtQueryObject fnNtQueryObject=NULL;
pGetSystemWow64DirectoryW fnGetSystemWow64DirectoryW=NULL;
pNtOpenSymbolicLinkObject fnNtOpenSymbolicLinkObject=NULL;
pNtQuerySymbolicLinkObject fnNtQuerySymbolicLinkObject=NULL;
pNtQueryInformationFile fnNtQueryInformationFile=NULL;
pNtQueryVirtualMemory fnNtQueryVirtualMemory=NULL;
pGetFileAttributesW fnGetFileAttributesW=NULL;
pQueryActCtxW fnQueryActCtxW=NULL;
pGetSecurityInfo fnGetSecurityInfo=NULL;
pGetKernelObjectSecurity fnGetKernelObjectSecurity=NULL;
pGetSecurityDescriptorOwner fnGetSecurityDescriptorOwner=NULL;
pLookupAccountSidA fnLookupAccountSidA=NULL;
pwine_get_version fnwine_get_version=NULL;

std::unique_ptr<Externs> Externs::instance;

Externs::Externs(): 
	hNtDll(NULL), hKernel32(NULL), hVersion(NULL), hAdvapi32(NULL)
{
	LoadFunctions();
}

Externs::~Externs() {
	UnloadFunctions();
}

bool Externs::MakeInstance() 
{
	if (instance)
		return false;
	
	instance.reset(new Externs());
	return true;
}

//Checking if DLLs are alredy loaded before LoadLibrary is cool but redundant
//This method is private and called (and designed to be called) only once - in constructor before everything else
void Externs::LoadFunctions() 
{
	hNtDll=LoadLibrary("ntdll.dll");
	hKernel32=LoadLibrary("kernel32.dll");
	hVersion=LoadLibrary("version.dll");
	hAdvapi32=LoadLibrary("advapi32.dll");

	if (hNtDll) {
		fnRtlGetVersion=(pRtlGetVersion)GetProcAddress(hNtDll, "RtlGetVersion");
		fnNtCreateFile=(pNtCreateFile)GetProcAddress(hNtDll, "NtCreateFile");
		fnNtQueryObject=(pNtQueryObject)GetProcAddress(hNtDll, "NtQueryObject");
		fnNtQueryInformationFile=(pNtQueryInformationFile)GetProcAddress(hNtDll, "NtQueryInformationFile");
		fnNtOpenSymbolicLinkObject=(pNtOpenSymbolicLinkObject)GetProcAddress(hNtDll, "NtOpenSymbolicLinkObject");
		fnNtQuerySymbolicLinkObject=(pNtQuerySymbolicLinkObject)GetProcAddress(hNtDll, "NtQuerySymbolicLinkObject");
		fnNtQueryVirtualMemory=(pNtQueryVirtualMemory)GetProcAddress(hNtDll, "NtQueryVirtualMemory");
		fnwine_get_version=(pwine_get_version)GetProcAddress(hNtDll, "wine_get_version");
	}
	
	if (hKernel32) {
		fnIsWow64Process=(pIsWow64Process)GetProcAddress(hKernel32, "IsWow64Process");
		fnGetSystemWow64DirectoryW=(pGetSystemWow64DirectoryW)GetProcAddress(hKernel32, "GetSystemWow64DirectoryW");
		fnGetNativeSystemInfo=(pGetNativeSystemInfo)GetProcAddress(hKernel32, "GetNativeSystemInfo");
		fnGetProductInfo=(pGetProductInfo)GetProcAddress(hKernel32, "GetProductInfo");
		fnGetVersionExA=(pGetVersionExA)GetProcAddress(hKernel32, "GetVersionExA");
		fnGetFileAttributesW=(pGetFileAttributesW)GetProcAddress(hKernel32, "GetFileAttributesW");
		fnQueryActCtxW=(pQueryActCtxW)GetProcAddress(hKernel32, "QueryActCtxW");
	}
	
	if (hVersion) {
		fnGetFileVersionInfoSizeExW=(pGetFileVersionInfoSizeExW)GetProcAddress(hVersion, "GetFileVersionInfoSizeExW");
		fnGetFileVersionInfoExW=(pGetFileVersionInfoExW)GetProcAddress(hVersion, "GetFileVersionInfoExW");
	}
	
	if (hAdvapi32) {
		fnGetKernelObjectSecurity=(pGetKernelObjectSecurity)GetProcAddress(hAdvapi32, "GetKernelObjectSecurity");
		fnGetSecurityDescriptorOwner=(pGetSecurityDescriptorOwner)GetProcAddress(hAdvapi32, "GetSecurityDescriptorOwner");
		fnGetSecurityInfo=(pGetSecurityInfo)GetProcAddress(hAdvapi32, "GetSecurityInfo");
		fnLookupAccountSidA=(pLookupAccountSidA)GetProcAddress(hAdvapi32, "LookupAccountSidA");
	}
}

//And here we are testing for NULLs because LoadLibrary can fail in method above
void Externs::UnloadFunctions() 
{
	if (hNtDll) FreeLibrary(hNtDll);
	if (hKernel32) FreeLibrary(hKernel32);
	if (hVersion) FreeLibrary(hVersion);
	if (hAdvapi32) FreeLibrary(hAdvapi32);
}

#include "externs.h"

pRtlGetVersion fnRtlGetVersion=NULL;
pGetNativeSystemInfo fnGetNativeSystemInfo=NULL;
pIsWow64Process fnIsWow64Process=NULL;
pGetProductInfo fnGetProductInfo=NULL;
pWow64DisableWow64FsRedirection fnWow64DisableWow64FsRedirection=NULL;
pWow64RevertWow64FsRedirection fnWow64RevertWow64FsRedirection=NULL;
pGetMappedFileNameW fnGetMappedFileNameW=NULL;
pNtCreateFile fnNtCreateFile=NULL;
pNtQueryObject fnNtQueryObject=NULL;
pGetSystemWow64DirectoryW fnGetSystemWow64DirectoryW=NULL;
pNtOpenSymbolicLinkObject fnNtOpenSymbolicLinkObject=NULL;
pNtQuerySymbolicLinkObject fnNtQuerySymbolicLinkObject=NULL;
pNtQueryInformationFile fnNtQueryInformationFile=NULL;
pSetSearchPathMode fnSetSearchPathMode=NULL;
pwine_get_version fnwine_get_version=NULL;

std::unique_ptr<Externs> Externs::instance;

Externs::Externs(): 
	hNtDll(NULL), hKernel32(NULL), hPsapi(NULL)
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
	hPsapi=LoadLibrary("psapi.dll");
	
	if (hNtDll) {
		fnRtlGetVersion=(pRtlGetVersion)GetProcAddress(hNtDll, "RtlGetVersion");
		fnNtCreateFile=(pNtCreateFile)GetProcAddress(hNtDll, "NtCreateFile");
		fnNtQueryObject=(pNtQueryObject)GetProcAddress(hNtDll, "NtQueryObject");
		fnNtQueryInformationFile=(pNtQueryInformationFile)GetProcAddress(hNtDll, "NtQueryInformationFile");
		fnNtOpenSymbolicLinkObject=(pNtOpenSymbolicLinkObject)GetProcAddress(hNtDll, "NtOpenSymbolicLinkObject");
		fnNtQuerySymbolicLinkObject=(pNtQuerySymbolicLinkObject)GetProcAddress(hNtDll, "NtQuerySymbolicLinkObject");
		fnwine_get_version=(pwine_get_version)GetProcAddress(hNtDll, "wine_get_version");
	}
	
	if (hKernel32) {
		fnSetSearchPathMode=(pSetSearchPathMode)GetProcAddress(hKernel32, "SetSearchPathMode");
		fnIsWow64Process=(pIsWow64Process)GetProcAddress(hKernel32, "IsWow64Process");
		fnGetSystemWow64DirectoryW=(pGetSystemWow64DirectoryW)GetProcAddress(hKernel32, "GetSystemWow64DirectoryW");
		fnGetNativeSystemInfo=(pGetNativeSystemInfo)GetProcAddress(hKernel32, "GetNativeSystemInfo");
		fnGetProductInfo=(pGetProductInfo)GetProcAddress(hKernel32, "GetProductInfo");
		fnWow64DisableWow64FsRedirection=(pWow64DisableWow64FsRedirection)GetProcAddress(hKernel32, "Wow64DisableWow64FsRedirection");
		fnWow64RevertWow64FsRedirection=(pWow64RevertWow64FsRedirection)GetProcAddress(hKernel32, "Wow64RevertWow64FsRedirection");
	}
	
	if (hPsapi) {
		fnGetMappedFileNameW=(pGetMappedFileNameW)GetProcAddress(hPsapi, "GetMappedFileNameW");
	}
}

//And here we are testing for NULLs because LoadLibrary can fail in method above
void Externs::UnloadFunctions() 
{
	if (hNtDll) FreeLibrary(hNtDll);
	if (hKernel32) FreeLibrary(hKernel32);
	if (hPsapi) FreeLibrary(hPsapi);
}

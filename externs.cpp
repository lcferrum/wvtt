#include "externs.h"

pRtlGetVersion fnRtlGetVersion=NULL;
pGetNativeSystemInfo fnGetNativeSystemInfo=NULL;
pIsWow64Process fnIsWow64Process=NULL;
pGetProductInfo fnGetProductInfo=NULL;
pNtCreateFile fnNtCreateFile=NULL;
pNtQueryObject fnNtQueryObject=NULL;
pGetSystemWow64DirectoryW fnGetSystemWow64DirectoryW=NULL;
pNtOpenSymbolicLinkObject fnNtOpenSymbolicLinkObject=NULL;
pNtQuerySymbolicLinkObject fnNtQuerySymbolicLinkObject=NULL;
pNtQueryInformationFile fnNtQueryInformationFile=NULL;
pNtQueryVirtualMemory fnNtQueryVirtualMemory=NULL;
pGetFileAttributesW fnGetFileAttributesW=NULL;
pwine_get_version fnwine_get_version=NULL;

std::unique_ptr<Externs> Externs::instance;

Externs::Externs(): 
	hNtDll(NULL), hKernel32(NULL)
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
		fnGetFileAttributesW=(pGetFileAttributesW)GetProcAddress(hKernel32, "GetFileAttributesW");
	}
}

//And here we are testing for NULLs because LoadLibrary can fail in method above
void Externs::UnloadFunctions() 
{
	if (hNtDll) FreeLibrary(hNtDll);
	if (hKernel32) FreeLibrary(hKernel32);
}

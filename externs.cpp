#include "externs.h"

pRtlGetVersion fnRtlGetVersion=NULL;
pGetNativeSystemInfo fnGetNativeSystemInfo=NULL;
pIsWow64Process fnIsWow64Process=NULL;
pGetProductInfo fnGetProductInfo=NULL;
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
		fnwine_get_version=(pwine_get_version)GetProcAddress(hNtDll, "wine_get_version");
	}
	
	if (hKernel32) {
		fnIsWow64Process=(pIsWow64Process)GetProcAddress(hKernel32, "IsWow64Process");
		fnGetNativeSystemInfo=(pGetNativeSystemInfo)GetProcAddress(hKernel32, "GetNativeSystemInfo");
		fnGetProductInfo=(pGetProductInfo)GetProcAddress(hKernel32, "GetProductInfo");
	}
}

//And here we are testing for NULLs because LoadLibrary can fail in method above
void Externs::UnloadFunctions() 
{
	if (hNtDll) FreeLibrary(hNtDll);
	if (hKernel32) FreeLibrary(hKernel32);
}

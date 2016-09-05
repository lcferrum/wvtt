#ifndef EXTERNS_H
#define EXTERNS_H

#include <memory>
#include <windows.h>

class Externs {
private:
	static std::unique_ptr<Externs> instance;
	
	HMODULE hNtDll;
	HMODULE hKernel32;
	
	void LoadFunctions();
	void UnloadFunctions();
	
	Externs();
public:
	~Externs();
	Externs(const Externs&)=delete;				//Get rid of default copy constructor
	Externs& operator=(const Externs&)=delete;	//Get rid of default copy assignment operator
	Externs(const Externs&&)=delete;				//Get rid of default move constructor
	Externs& operator=(const Externs&&)=delete;	//Get rid of default move assignment operator
	
	static bool MakeInstance();	
};

typedef NTSTATUS (WINAPI *pRtlGetVersion)(PRTL_OSVERSIONINFOW lpVersionInformation);
typedef void (WINAPI *pGetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);
typedef BOOL (WINAPI *pIsWow64Process)(HANDLE hProcess, PBOOL Wow64Process);
typedef BOOL (WINAPI *pGetProductInfo)(DWORD dwOSMajorVersion, DWORD dwOSMinorVersion, DWORD dwSpMajorVersion, DWORD dwSpMinorVersion, PDWORD pdwReturnedProductType);
typedef const char* (CDECL *pwine_get_version)(void);

#endif //EXTERNS_H

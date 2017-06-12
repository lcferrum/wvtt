#ifndef EXTERNS_H
#define EXTERNS_H

#include <memory>
#include <windows.h>
#include <winternl.h>

class Externs {
private:
	static std::unique_ptr<Externs> instance;
	
	HMODULE hNtDll;
	HMODULE hKernel32;
	HMODULE hVersion;
	HMODULE hAdvapi32;
	
	void LoadFunctions();
	void UnloadFunctions();
	
	Externs();
public:
	~Externs();
	Externs(const Externs&)=delete;				//Get rid of default copy constructor
	Externs& operator=(const Externs&)=delete;	//Get rid of default copy assignment operator
	Externs(const Externs&&)=delete;			//Get rid of default move constructor
	Externs& operator=(const Externs&&)=delete;	//Get rid of default move assignment operator
	
	static bool MakeInstance();	
};

typedef NTSTATUS (WINAPI *pRtlGetVersion)(PRTL_OSVERSIONINFOW lpVersionInformation);
typedef void (WINAPI *pGetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);
typedef BOOL (WINAPI *pIsWow64Process)(HANDLE hProcess, PBOOL Wow64Process);
typedef BOOL (WINAPI *pGetFileVersionInfoExW)(DWORD dwFlags, LPCWSTR lpwstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData);
typedef DWORD (WINAPI *pGetFileVersionInfoSizeExW)(DWORD dwFlags, LPCWSTR lpwstrFilename, LPDWORD lpdwHandle);
typedef BOOL (WINAPI *pGetProductInfo)(DWORD dwOSMajorVersion, DWORD dwOSMinorVersion, DWORD dwSpMajorVersion, DWORD dwSpMinorVersion, PDWORD pdwReturnedProductType);
typedef NTSTATUS (WINAPI *pNtCreateFile)(PHANDLE FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, PLARGE_INTEGER AllocationSize, ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions, PVOID EaBuffer, ULONG EaLength);
typedef NTSTATUS (WINAPI *pNtQueryObject)(HANDLE Handle, OBJECT_INFORMATION_CLASS ObjectInformationClass, PVOID ObjectInformation, ULONG ObjectInformationLength, PULONG ReturnLength);
typedef UINT (WINAPI *pGetSystemWow64DirectoryW)(LPWSTR lpBuffer, UINT uSize);
typedef NTSTATUS (WINAPI *pNtOpenSymbolicLinkObject)(PHANDLE LinkHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes);
typedef NTSTATUS (WINAPI *pNtQuerySymbolicLinkObject)(HANDLE LinkHandle, PUNICODE_STRING LinkTarget, PULONG ReturnedLength);
typedef NTSTATUS (WINAPI *pNtQueryInformationFile)(HANDLE FileHandle, PIO_STATUS_BLOCK IoStatusBlock, PVOID FileInformation, ULONG Length, FILE_INFORMATION_CLASS FileInformationClass);
typedef BOOL (WINAPI *pGetVersionExA)(LPOSVERSIONINFOA lpVersionInfo);
enum MEMORY_INFORMATION_CLASS {MemorySectionName=0x2};
typedef NTSTATUS (WINAPI *pNtQueryVirtualMemory)(HANDLE ProcessHandle, PVOID BaseAddress, MEMORY_INFORMATION_CLASS MemoryInformationClass, PVOID MemoryInformation, SIZE_T MemoryInformationLength, PSIZE_T ReturnLength);
typedef DWORD (WINAPI *pGetFileAttributesW)(LPCWSTR lpFileName);
typedef BOOL (WINAPI *pQueryActCtxW)(DWORD dwFlags, HANDLE hActCtx, PVOID pvSubInstance, ULONG ulInfoClass, PVOID pvBuffer, SIZE_T cbBuffer, SIZE_T *pcbWrittenOrRequired);
typedef DWORD (WINAPI *pGetSecurityInfo)(HANDLE handle, SE_OBJECT_TYPE ObjectType, SECURITY_INFORMATION SecurityInfo, PSID *ppsidOwner, PSID *ppsidGroup, PACL *ppDacl, PACL *ppSacl, PSECURITY_DESCRIPTOR *ppSecurityDescriptor);
typedef BOOL (WINAPI *pLookupAccountSidA)(LPCSTR lpSystemName, PSID lpSid, LPSTR lpName, LPDWORD cchName, LPSTR lpReferencedDomainName, LPDWORD cchReferencedDomainName, PSID_NAME_USE peUse);
typedef const char* (CDECL *pwine_get_version)(void);

#endif //EXTERNS_H

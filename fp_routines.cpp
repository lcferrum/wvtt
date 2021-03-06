#include "fp_routines.h"
#include "externs.h"
#include <iostream>
#include <vector>
#include <ntstatus.h>
#include <windows.h>

#define SYMBOLIC_LINK_QUERY 0x0001

extern pNtOpenSymbolicLinkObject fnNtOpenSymbolicLinkObject;
extern pNtQuerySymbolicLinkObject fnNtQuerySymbolicLinkObject;
extern pNtCreateFile fnNtCreateFile;
extern pNtQueryInformationFile fnNtQueryInformationFile;
extern pNtQueryObject fnNtQueryObject;
extern pNtQueryVirtualMemory fnNtQueryVirtualMemory;
extern pIsWow64Process fnIsWow64Process;
extern pGetFileAttributesW fnGetFileAttributesW;
extern pGetSystemWow64DirectoryW fnGetSystemWow64DirectoryW;

namespace FPRoutines {
	std::vector<std::pair<std::wstring, wchar_t>> DriveList;
	bool KernelToWin32Path(const wchar_t* krn_fpath, std::wstring &w32_fpath, USHORT krn_ulen=USHRT_MAX, USHORT krn_umaxlen=0);
	bool UnredirectWow64FsPath(const char* fpath, std::string &real_fpath);
	bool GetMappedFileNameWrapper(LPVOID hMod, std::string &fpath);
	bool SearchPathWrapper(const char* fname, const char* spath, const char* ext, std::string &fpath);
	bool GetSFP_LoadLibrary(const char* fname, std::string &fpath); 
	bool GetSFP_SearchPathForDLL(const char* fname, std::string &fpath); 
	bool GetSFP_SearchPathForVXD(const char* fname, std::string &fpath);
}

void FPRoutines::FillDriveList() 
{
	DriveList.clear();
	
	if (!fnNtCreateFile||!fnNtQueryObject||!fnNtOpenSymbolicLinkObject||!fnNtQuerySymbolicLinkObject)
		return;
	
	HANDLE hFile;
	OBJECT_ATTRIBUTES objAttribs;	
	wchar_t drive_lnk[]=L"\\??\\A:";
	UNICODE_STRING u_drive_lnk={(USHORT)(sizeof(drive_lnk)-sizeof(wchar_t)), (USHORT)sizeof(drive_lnk), drive_lnk};
	IO_STATUS_BLOCK ioStatusBlock;
	BYTE oni_buf[1024];
	
	//InitializeObjectAttributes is a macros that assigns OBJECT_ATTRIBUTES it's parameters
	//Second parameter is assigned to OBJECT_ATTRIBUTES.ObjectName
	//OBJECT_ATTRIBUTES.ObjectName is a UNICODE_STRING which Buffer member is just a pointer to actual buffer (drive_lnk)
	//That's why changing buffer contents won't require calling InitializeObjectAttributes second time
	InitializeObjectAttributes(&objAttribs, &u_drive_lnk, OBJ_CASE_INSENSITIVE, NULL, NULL);

	//There are several ways to enumerate all drives and get their NT paths
	//QueryDosDevice does the job but doesn't display paths for drive letters that are mapped network drives
	//NtOpenSymbolicLinkObject/NtQuerySymbolicLinkObject is better approach because it resolves mapped network drives letters
	//But it has drawback - mapped network drives letters often resolve to NT path that contains some internal IDs making it difficult to use it NT->Win32 path conversion
	//Example of such path: "\Device\LanmanRedirector\;Z:00000000000894aa\PC_NAME\SHARE_NAME"
	//And at last we have NtCreateFile(FILE_DIRECTORY_FILE)/NtQueryObject approach
	//It works even better than NtOpenSymbolicLinkObject/NtQuerySymbolicLinkObject - mapped drives are resolved to ordinary NT paths
	//With previous example it will now look like this: "\Device\LanmanRedirector\PC_NAME\SHARE_NAME"
	//Though there are two caveats:
	//	1) Don't append drive with backslash or NtCreateFile will open root directory instead (imagine it's floppy drive without disk inserted)
	//	2) Under NT4 NtQueryObject will fail with every drive letter except real mapped network drives (though ObjectNameInformation still gets filled with proper path)
	//Also, when mapped network drive being queried - code won't force system to check whether drive is offline or not
	//This means code won't stop there to wait for system response, which is good
	//The only time when there is really delay in execution is when system is in the process of changing drive status (e.g. explorer trying to access no longer available network drive)
	//Only in this case when trying to open such drive code will wait for system to update drive status
	//So we keep NtOpenSymbolicLinkObject/NtQuerySymbolicLinkObject as backup approach in case we are on NT4 (this approach still works there)
	for (drive_lnk[4]=L'A'; drive_lnk[4]<=L'Z'; drive_lnk[4]++)	{	//4'th index is a drive letter
		if (NT_SUCCESS(fnNtCreateFile(&hFile, FILE_READ_ATTRIBUTES, &objAttribs, &ioStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, FILE_OPEN, FILE_DIRECTORY_FILE, NULL, 0))) {
			if (NT_SUCCESS(fnNtQueryObject(hFile, ObjectNameInformation, (OBJECT_NAME_INFORMATION*)oni_buf, 1024, NULL))) {
				//Actually OBJECT_NAME_INFORMATION contains NULL-terminated UNICODE_STRING but we can save std::wstring constructor time and provide it with string length
				DriveList.push_back(std::make_pair(std::wstring(((OBJECT_NAME_INFORMATION*)oni_buf)->Name.Buffer, ((OBJECT_NAME_INFORMATION*)oni_buf)->Name.Length/sizeof(wchar_t)), drive_lnk[4]));
				CloseHandle(hFile);
				continue;
			}
			CloseHandle(hFile);
		}
		
		if (NT_SUCCESS(fnNtOpenSymbolicLinkObject(&hFile, SYMBOLIC_LINK_QUERY, &objAttribs))) {
			//In buf_len function returns not the actual buffer size needed for UNICODE_STRING, but the size of the UNICODE_STRING.Buffer itself
			//And this UNICODE_STRING should be already initialized before the second call - buffer created, Buffer pointer and MaximumLength set
			//Returned UNICODE_STRING includes terminating NULL
			UNICODE_STRING u_path={};
			DWORD buf_len;
			if (fnNtQuerySymbolicLinkObject(hFile, &u_path, &buf_len)==STATUS_BUFFER_TOO_SMALL) {
				u_path.Buffer=(wchar_t*)new BYTE[buf_len];	//buf_len is Length + terminating NULL
				u_path.MaximumLength=buf_len;
				if (NT_SUCCESS(fnNtQuerySymbolicLinkObject(hFile, &u_path, &buf_len))) {
					//As was said earlier returned UNICODE_STRING is NULL-terminated but we can save std::wstring constructor time and provide it with string length
					DriveList.push_back(std::make_pair(std::wstring(u_path.Buffer, u_path.Length/sizeof(wchar_t)), drive_lnk[4]));
				}
				delete[] (BYTE*)u_path.Buffer;
			}
			
			CloseHandle(hFile);
		}
	}
}

bool FPRoutines::UnredirectWow64FsPath(const char* fpath, std::string &real_fpath)
{
#ifdef _WIN64
	//Useless for 64-bit binary - no redirection is done for it
	real_fpath=fpath;
	return true;
#else
	//How does it work
	//We are supplying original path (prefixed with \??\ so to make NT path from it) to NtCreateFile
	//WoW64 redirection expands to NtCreateFile for \??\ paths so when NtCreateFile opens this file - it opens the right (intended) one
	//Now we use NtQueryObject to get canonical NT path (\Device\...) from opened handle
	//These paths are not affected by WoW64 redirection so returned path is the real one
	//After that we get WoW64 directory (with GetSystemWow64Directory) and convert it to NT path the same way as original path
	//Find out if original path has something to do with WoW64 redirection by checking if it is prefixed by WoW64 directory
	//If not - just return original path because it is not actually redirected
	//Now all what is left is to convert original path in it's NT canonical form back to it's Win32 equivalent
	//In this case it is simple because we already know Win32 equivalent of it's device prefix which is WoW64 directory
	//Just swap this prefix with it's Win32 equivalent and we are good to go
	
	//Some caveats
	//Original path should be a valid file path - file must exist and everything
	//Algorithm can be mofified to also allow directory as original path - but it still must exist
	//On any conversion error (i.e. error not related to original path not being redirected one) function returns empty string

	//If not on WoW64 - obviously return fpath (aka original string)
	//If WoW64 is non-zero, all functions below in theory should be available
	//But because calling unavailable function potentially causes access violation it's a good thing to check them anyway (NT functions are subject to change according to MS)
	//(And treat unavailability of functions as unavailability of WoW64)
	BOOL wow64=FALSE;
	if (!fnIsWow64Process||!fnIsWow64Process(GetCurrentProcess(), &wow64)||!wow64||!fnGetSystemWow64DirectoryW||!fnNtCreateFile||!fnNtQueryObject) {
		real_fpath=fpath;
		return true;
	}
	
	UINT chars_num=fnGetSystemWow64DirectoryW(NULL, 0);
	//GetSystemWow64Directory can fail with GetLastError()=ERROR_CALL_NOT_IMPLEMENTED indicating that we should return fpath because there are no WoW64 obviously
	//But it is impossible case because wow64 should be zero for that to work and we have already checked for it to be non-zero
	//So if GetSystemWow64Directory failed: it failed for some other nasty reason - treat as conversion error
	if (!chars_num) 
		return false;
	
	wchar_t wow64_fpath[chars_num+4];	//4 is length of "\??\" in characters not including '\0'
	wcscpy(wow64_fpath, L"\\??\\");
	if (!fnGetSystemWow64DirectoryW(wow64_fpath+4, chars_num))
		return false;
	
	chars_num=MultiByteToWideChar(CP_ACP, 0, fpath, -1, NULL, 0);
	if (!chars_num)
		return false;
	
	wchar_t wq_fpath[chars_num+4];	//4 is length of "\??\" in characters not including '\0'
	wcscpy(wq_fpath, L"\\??\\");
	if (!MultiByteToWideChar(CP_ACP, 0, fpath, -1, wq_fpath+4, chars_num))
		return false;
	
	HANDLE hFile;
	OBJECT_ATTRIBUTES objAttribs;	
	UNICODE_STRING ustr_fpath={(USHORT)(wcslen(wq_fpath)*sizeof(wchar_t)), (USHORT)((wcslen(wq_fpath)+1)*sizeof(wchar_t)), wq_fpath};
	IO_STATUS_BLOCK ioStatusBlock;
	
	InitializeObjectAttributes(&objAttribs, &ustr_fpath, OBJ_CASE_INSENSITIVE, NULL, NULL);	
	if (!NT_SUCCESS(fnNtCreateFile(&hFile, FILE_READ_ATTRIBUTES, &objAttribs, &ioStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, FILE_OPEN, FILE_NON_DIRECTORY_FILE, NULL, 0)))
		return false;
	
	DWORD buf_len=wcslen(wq_fpath)*sizeof(wchar_t)+1024;
	BYTE oni_buf_wqpath[buf_len];
	if (!NT_SUCCESS(fnNtQueryObject(hFile, ObjectNameInformation, (OBJECT_NAME_INFORMATION*)oni_buf_wqpath, buf_len, NULL))) {
		CloseHandle(hFile);
		//NtQueryObject can fail with STATUS_INVALID_INFO_CLASS indicating that ObjectNameInformation class is not supported
		//This could in theory happen on some damn old NT which, because of it's age, shouldn't also support WoW64
		//But we have already checked WoW64 presence and it should be non-zero if we are here
		//Or MS decided (again) to change NtQueryObject behaviour for one of the future releases and remove STATUS_INVALID_INFO_CLASS
		//All in all, if NtQueryObject failed for any of the reasons - it's conversion error
		return false;
	}	
	CloseHandle(hFile);
	
	ustr_fpath.Length=(USHORT)(wcslen(wow64_fpath)*sizeof(wchar_t));
	ustr_fpath.MaximumLength=(USHORT)((wcslen(wow64_fpath)+1)*sizeof(wchar_t));
	ustr_fpath.Buffer=wow64_fpath;
	
	InitializeObjectAttributes(&objAttribs, &ustr_fpath, OBJ_CASE_INSENSITIVE, NULL, NULL);	
	if (!NT_SUCCESS(fnNtCreateFile(&hFile, FILE_READ_ATTRIBUTES, &objAttribs, &ioStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, FILE_OPEN, FILE_DIRECTORY_FILE, NULL, 0)))
		return false;
	
	buf_len=wcslen(wow64_fpath)*sizeof(wchar_t)+1024;
	BYTE oni_buf_wow64path[buf_len];
	if (!NT_SUCCESS(fnNtQueryObject(hFile, ObjectNameInformation, (OBJECT_NAME_INFORMATION*)oni_buf_wow64path, buf_len, NULL))) {
		CloseHandle(hFile);
		return false;
	}	
	CloseHandle(hFile);
	
	wchar_t* wow64_krn_fpath=((OBJECT_NAME_INFORMATION*)oni_buf_wow64path)->Name.Buffer;
	wchar_t* wq_krn_fpath=((OBJECT_NAME_INFORMATION*)oni_buf_wqpath)->Name.Buffer;
	chars_num=((OBJECT_NAME_INFORMATION*)oni_buf_wow64path)->Name.Length/sizeof(wchar_t);
	//First check that queried path is prefixed by WoW64 path
	//Length of wow64_krn_fpath (chars_num) is guaranteed to be >0 because otherwise NtCreateFile or NtQueryObject should have failed
	if (wcsncmp(wow64_krn_fpath, wq_krn_fpath, chars_num)) {
		real_fpath=fpath;
		return true;
	}
	
	//If it is prefixed and prefix doesn't end with backslash - check that character next to prefix is backslash
	//We don't check wq_krn_fpath length because wq_krn_fpath can only be file path and if it passed wcsncmp, then it should be longer than it's directory prefix
	if (wow64_krn_fpath[chars_num-1]!=L'\\'&&wq_krn_fpath[chars_num]!=L'\\') {
		real_fpath=fpath;
		return true;
	}
	
	//Now everything is alright and we can swap prefix with it's Win32 equivalent
	wchar_t wunrd_fpath[wcslen(wow64_fpath)+((OBJECT_NAME_INFORMATION*)oni_buf_wqpath)->Name.Length/sizeof(wchar_t)-chars_num+1];
	wcscpy(wunrd_fpath, wow64_fpath);
	wcscat(wunrd_fpath, wq_krn_fpath+chars_num);
	
	chars_num=WideCharToMultiByte(CP_ACP, 0, wunrd_fpath+4, -1, NULL, 0, NULL, NULL);	//4 is length of "\??\" in characters not including '\0'
	if (!chars_num)
		return false;
	
	char unredirected_fpath[chars_num];
	if (!WideCharToMultiByte(CP_ACP, 0, wunrd_fpath+4, -1, unredirected_fpath, chars_num, NULL, NULL))	//4 is length of "\??\" in characters not including '\0'
		return false;
	
	real_fpath=unredirected_fpath;
	return true;
#endif
}

bool FPRoutines::KernelToWin32Path(const wchar_t* krn_fpath, std::wstring &w32_fpath, USHORT krn_ulen, USHORT krn_umaxlen)
{
	//GetFileAttributesW is dynamically loaded along with NT function so to use the same code on Win 9x
	if (!krn_ulen||!fnGetFileAttributesW||!fnNtCreateFile||!fnNtQueryObject||!fnNtQueryInformationFile)
		return false;
	
	//Function accepts both UNICODE_STRINGs and null-terminated wchar_t arrays
	if (krn_ulen==USHRT_MAX&&krn_umaxlen==0) {
		krn_ulen=wcslen(krn_fpath)*sizeof(wchar_t);
		krn_umaxlen=krn_ulen+sizeof(wchar_t);
	}
	
	//Basic algorithm is the following:
	//We have NT kernel path like "\Device\HarddiskVolume2\Windows\System32\wininit.exe" and should turn it to "user-readable" Win32 path
	//First, we should resolve any symbolic links in the path like "\SystemRoot" and "\??"
	//It could be done by opening the file (NtCreateFile) and then getting path to the handle (NtQueryObject(ObjectNameInformation))
	//Then, we try to match resulting path with one of the device prefixes from DriveList - this way we resolve device name to it's Win32 equivalent
	//If we have a match - swap device prefix with it's drive letter and we are good to go
	//If no match, usually that means it's some kind of network path which hasn't been mapped to any of drives
	//That's why we extract relative (to device prefix) path using NtQueryInformationFile(FileNameInformation) and make UNC path from it
	//Check if guess was right by testing resulting UNC path for existence

	HANDLE hFile;
	OBJECT_ATTRIBUTES objAttribs;	
	UNICODE_STRING ustr_fpath={krn_ulen, krn_umaxlen, const_cast<wchar_t*>(krn_fpath)};
	IO_STATUS_BLOCK ioStatusBlock;
	
	InitializeObjectAttributes(&objAttribs, &ustr_fpath, OBJ_CASE_INSENSITIVE, NULL, NULL);

	//NtCreateFile will accept only NT kernel paths
	//NtCreateFile will not accept ordinary Win32 paths (will fail with STATUS_OBJECT_PATH_SYNTAX_BAD)
	if (!NT_SUCCESS(fnNtCreateFile(&hFile, FILE_READ_ATTRIBUTES, &objAttribs, &ioStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, FILE_OPEN, FILE_NON_DIRECTORY_FILE, NULL, 0)))
		return false;
	
	//Very inconsistent function which behaviour differs between OS versions
	//Starting from Vista things are easy - just pass NULL buffer and zero length and you'll get STATUS_INFO_LENGTH_MISMATCH and needed buffer size
	//Before Vista things are ugly - you will get all kinds of error statuses because of insufficient buffer
	//And function won't necessary return needed buffer size - it actually depends on passed buffer size!
	//But worst of all is NT4 where function will never return needed buffer size and you can get real buffer overflow with some buffer sizes
	//So enumerating buffer sizes here is dangerous because of potential buffer overflow and all the unexpected nasty things that could occur afterwards
	//Internally, when calling NtQueryObject, Windows doesn't try to find actual buffer size - it just supplies some large buffer (like entire page!) and hopes for the best
	//We'll try something similar here: we already have kernel path, only portion of which may expand to something bigger
	//So let's assume that [current path length in bytes + 1024] is a sane buffer size (1024 - most common buffer size that Windows passes to NtQueryObject)
	//Returned path is NULL-terminated (MaximumLength is Length plus NULL-terminator, all in bytes)
	DWORD buf_len=krn_ulen+1024;
	BYTE oni_buf[buf_len];
	if (!NT_SUCCESS(fnNtQueryObject(hFile, ObjectNameInformation, (OBJECT_NAME_INFORMATION*)oni_buf, buf_len, NULL))) {
		CloseHandle(hFile);
		return false;
	}
	
	wchar_t* res_krn_path=((OBJECT_NAME_INFORMATION*)oni_buf)->Name.Buffer;
	for (std::pair<std::wstring, wchar_t> &drive: DriveList) {
		if (!wcsncmp(drive.first.c_str(), res_krn_path, drive.first.length())&&(drive.first.back()==L'\\'||res_krn_path[drive.first.length()]==L'\\')) {
			CloseHandle(hFile);
			if (drive.first.back()==L'\\')
				w32_fpath={drive.second, L':', L'\\'};
			else
				w32_fpath={drive.second, L':'};
			w32_fpath.append(res_krn_path+drive.first.length());
			return true;
		}
	}
	
	//In contrast with NtQueryObject, NtQueryInformationFile is pretty predictable
	//To get needed buffer size just supply buffer that holds FILE_NAME_INFORMATION structure and wait for STATUS_BUFFER_OVERFLOW (in this case it's just a status, don't worry)
	//Needed buffer size (minus sizeof(FILE_NAME_INFORMATION)) will be in FILE_NAME_INFORMATION.FileNameLength
	//But we already know sufficient buffer size from the call to NtQueryObject - no need to second guess here
	//NtQueryInformationFile(FileNameInformation) returns path relative to device
	//Returned path is not NULL-terminated (FileNameLength is string length in bytes)
	buf_len=((OBJECT_NAME_INFORMATION*)oni_buf)->Name.Length+sizeof(FILE_NAME_INFORMATION);
	BYTE fni_buf[buf_len];
	if (!NT_SUCCESS(fnNtQueryInformationFile(hFile, &ioStatusBlock, (FILE_NAME_INFORMATION*)fni_buf, buf_len, FileNameInformation))) {
		CloseHandle(hFile);
		return false;
	}	
	
	CloseHandle(hFile);
	std::wstring unc_fpath(L"\\");
	unc_fpath.append(((FILE_NAME_INFORMATION*)fni_buf)->FileName, ((FILE_NAME_INFORMATION*)fni_buf)->FileNameLength/sizeof(wchar_t));
	DWORD dwAttrib=fnGetFileAttributesW(unc_fpath.c_str());
	if (dwAttrib!=INVALID_FILE_ATTRIBUTES&&!(dwAttrib&FILE_ATTRIBUTE_DIRECTORY)) {
		w32_fpath=std::move(unc_fpath);
		return true;
	}

	return false;
}

bool FPRoutines::GetMappedFileNameWrapper(LPVOID hMod, std::string &fpath)
{
	//Alright, this is reinvention of GetMappedFileName from psapi.dll
	//But we have several reasons for this
	//NT4 not necessarry includes this DLL out-of-the-box - official MS installation disks don't have one as none of SPs
	//It should be installed as redistributable package that shipped separately (psinst.exe) or as part of "Windows NT 4.0 Resource Kit" or "Windows NT 4.0 SDK"
	//Though most modern non-MS "all-inclusive" NT4 install disks usually install it by default
	//On Win 9x we have no psapi.dll though some "unofficial" SPs for 9x systems install it anyway
	//In the end exporting it dynamically may succeed on Win 9x if such SP was installed but using it will result in crash
	//So here we re-implementing NT's GetMappedFileName so not to be dependent on psapi.dll
	if (fnNtQueryVirtualMemory) {
		//Actual string buffer size is MAX_PATH characters - same size is used in MS's GetMappedFileName implementation
		SIZE_T buf_len=sizeof(UNICODE_STRING)+MAX_PATH*sizeof(wchar_t);
		SIZE_T ret_len;
		BYTE msn_buf[buf_len];
		
		if (NT_SUCCESS(fnNtQueryVirtualMemory(GetCurrentProcess(), hMod, MemorySectionName, msn_buf, buf_len, &ret_len))) {
			//UNICODE_STRING returned by NtQueryVirtualMemory(MemorySectionName) not necessary NULL terminated
			std::wstring w32_path;
			if (KernelToWin32Path(((UNICODE_STRING*)msn_buf)->Buffer, w32_path, ((UNICODE_STRING*)msn_buf)->Length, ((UNICODE_STRING*)msn_buf)->MaximumLength)) {
				if (int chars_num=WideCharToMultiByte(CP_ACP, 0, w32_path.c_str(), -1, NULL, 0, NULL, NULL)) {
					char full_path[chars_num];
					if (WideCharToMultiByte(CP_ACP, 0, w32_path.c_str(), -1, full_path, chars_num, NULL, NULL)) {
						fpath=full_path;
						return true;
					}
				}
			}
		}
	}
	
	return false;
}

bool FPRoutines::GetSFP_LoadLibrary(const char* fname, std::string &fpath)
{
	//Function searches for file as LoadLibrary itself would do it
	//In fact it uses LoadLibrary to search the file and then just queries what it found in the end
	//File can be already loaded by executable - in this case function gets name path to loaded module, which works for botn NT and 9x
	//If not - LoadLibraryEx(LOAD_LIBRARY_AS_DATAFILE) is called to load it and then the name is queried, but this works only on NT
	HMODULE hMod;

	//First see if this library was already loaded by current process
	if ((hMod=GetModuleHandle(fname))) {
		//MAX_PATH related issues is a tough question regarding loading modules
		//At present (Win 10) it seems that LoadLibrary and CreateProcess really doesn't handle path lengths more than MAX_PATH
		//Current workaround is converting such paths to short "8.3" format and using it with aforementioned functions
		//In this case paths returned by GetModuleFileName won't exceed MAX_PATH because they will be in 8.3 format
		//(for executable, modules loaded from executable directory and modules loaded with 8.3 paths)
		char full_path[MAX_PATH];
		DWORD retlen=GetModuleFileName(hMod, full_path, MAX_PATH);
		//GetModuleFileName returns 0 if everything is bad and nSize (which is MAX_PATH) if buffer size is insufficient and returned path truncated
		//In case of MAX_PATH, buffer size really should be enough to hold any library path but someday MS may decide to lift LoadLibrary limitation on path size
		//It's better to be safe than sorry
		if (retlen&&retlen<MAX_PATH)
			//It is observed that some file paths returned by GetModuleFileName are not affected by WoW64 redirection
			//But it's not always the case so it's better to convert anyway
			return UnredirectWow64FsPath(full_path, fpath);
	//Next we load library and get path to it
	//By using LOAD_LIBRARY_AS_DATAFILE flag we load library as a resource (DLLMain not executed and no functions are exported)
	} else if ((hMod=LoadLibraryEx(fname, NULL, LOAD_LIBRARY_AS_DATAFILE))) {
		bool res=GetMappedFileNameWrapper(hMod, fpath);
		FreeLibrary(hMod);
		return res;
	}
	
	return false;
}

bool FPRoutines::SearchPathWrapper(const char* fname, const char* spath, const char* ext, std::string &fpath)
{
	//SearchPath uses the following algorithm
	//First it checks if supplied filename is a relative path (completely relative - to both current drive and directory)
	//If it's not relative - it applies GetFullPathName to filename and returns resulting path (whatever it might be)
	//If it's relative, real search commences
	//It searches supplied search path or, in case it is missing, searches these paths:
	//	- Current image directory
	//	- Current working directory (if SafeProcessSearchMode is 0 or not supported by OS)
	//	- Windows system directory (GetSystemDirectory())
	//	- Windows 16-bit system directory (GetWindowsDirectory()+"\system", NT only)
	//	- Windows directory (GetWindowsDirectory())
	//	- Directories listed in the PATH environment variable
	//	- Current working directory (if SafeProcessSearchMode is 1)
	//Supplying extension (should start with period character, '.') changes behavior in the following way:
	//If non relative filename, before applying GetFullPathName function checks if file exists and if not - appends extension and tries this way
	//If relative filename and filename lacks extension, it is appended before the search commences
	
	if (DWORD buflen=SearchPath(spath, fname, ext, 0, NULL, NULL)) {
		char full_path[buflen];
		if (DWORD cpylen=SearchPath(spath, fname, ext, buflen, full_path, NULL)) {
			if (cpylen<buflen)
				return UnredirectWow64FsPath(full_path, fpath);
		}
	}

	return false;
}

bool FPRoutines::GetSFP_SearchPathForDLL(const char* fname, std::string &fpath)
{
	//This is almost generic SearchPath call to search for all kinds of system files
	//If no extension supplied - default "dll" is used, like in LoadLibrary call
	//On 9x and early NTs SearchPath search algorithm is actually the same one that is used in LoadLibrary
	//So it can be used as substitute to "getting-path-to-module-loaded-by-LoadLibrary" here
	
	return SearchPathWrapper(fname, NULL, ".dll", fpath);
}

bool FPRoutines::GetSFP_SearchPathForVXD(const char* fname, std::string &fpath)
{
	//This SearchPath call is specifically tailored to search for Win 9x VXDs
	//It searches directories from where system and some of the 3rd-party static VXDs are loaded
	//Of course 3rd-party VXD can be placed anywhere in the system
	//But if they are loaded just by name (and not full path) they reside in these directories
	//Querying registry and system.ini to get full paths of 3rd-party VXDs is out-of-scope for this function
	//It was designed mostly for system VXDs
	
	std::vector<std::string> VxdPaths;
	
	if (DWORD buflen=GetSystemDirectory(NULL, 0)) {
		char dir_path[buflen];
		if (DWORD cpylen=GetSystemDirectory(dir_path, buflen)) {
			if (cpylen<buflen) {
				VxdPaths.push_back(dir_path);
				if (dir_path[cpylen-1]=='\\') dir_path[cpylen-1]='\0';
				VxdPaths.push_back(std::string(dir_path)+"\\VMM32");
				VxdPaths.push_back(std::string(dir_path)+"\\IOSUBSYS");
			}
		}
	}
	
	for (std::string &spath: VxdPaths)
		if (SearchPathWrapper(fname, spath.c_str(), ".vxd", fpath))
			return true;
	
	return false;
}	

std::string FPRoutines::GetSystemFilePath(const char* fname)
{
	std::string fpath;
	
	if (!GetSFP_LoadLibrary(fname, fpath))			//First we use LoadLibrary-like search algorithm to find full file path - this works best on NT systems with real DLL files
		if (!GetSFP_SearchPathForDLL(fname, fpath))	//If this fails, fallback option is to use SearchPath with default search location - it works best on 9x systems with DLL files and on both 9x and NT with non-DLL types
			GetSFP_SearchPathForVXD(fname, fpath);	//On 9x VXD files can be placed in paths that differ from SearchPath default search location - for such case SearchPath is called to search in VXD specific directories
	
	return fpath;
}

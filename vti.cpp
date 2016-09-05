#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>

//Defines not found in MinGW's shlwapi.h:
#define OS_WINDOWS                  0           // Windows 9x vs. NT
#define OS_NT                       1           // Windows 9x vs. NT
#define OS_WIN95ORGREATER           2           // Win95 or greater
#define OS_NT4ORGREATER             3           // NT4 or greater
#define OS_WIN98ORGREATER           5           // Win98 or greater
#define OS_WIN98_GOLD               6           // Win98 Gold (Version 4.10 build 1998)
#define OS_WIN2000ORGREATER         7           // Some derivative of Win2000
#define OS_WIN2000PRO               8           // Windows 2000 Professional (Workstation)
#define OS_WIN2000SERVER            9           // Windows 2000 Server
#define OS_WIN2000ADVSERVER         10          // Windows 2000 Advanced Server
#define OS_WIN2000DATACENTER        11          // Windows 2000 Data Center Server
#define OS_WIN2000TERMINAL          12          // Windows 2000 Terminal Server in "Application Server" mode (now simply called "Terminal Server")
#define OS_EMBEDDED                 13          // Embedded Windows Edition
#define OS_TERMINALCLIENT           14          // Windows Terminal Client (eg user is comming in via tsclient)
#define OS_TERMINALREMOTEADMIN      15          // Terminal Server in "Remote Administration" mode
#define OS_WIN95_GOLD               16          // Windows 95 Gold (Version 4.0 Build 1995)
#define OS_MEORGREATER              17          // Windows Millennium (Version 5.0)
#define OS_XPORGREATER              18          // Windows XP or greater
#define OS_HOME                     19          // Home Edition (eg NOT Professional, Server, Advanced Server, or Datacenter)
#define OS_PROFESSIONAL             20          // Professional     (aka Workstation; eg NOT Server, Advanced Server, or Datacenter)
#define OS_DATACENTER               21          // Datacenter       (eg NOT Server, Advanced Server, Professional, or Personal)
#define OS_ADVSERVER                22          // Advanced Server  (eg NOT Datacenter, Server, Professional, or Personal)
#define OS_SERVER                   23          // Server           (eg NOT Datacenter, Advanced Server, Professional, or Personal)
#define OS_TERMINALSERVER           24          // Terminal Server - server running in what used to be called "Application Server" mode (now simply called "Terminal Server")
#define OS_PERSONALTERMINALSERVER   25          // Personal Terminal Server - per/pro machine running in single user TS mode
#define OS_FASTUSERSWITCHING        26          // Fast User Switching
#define OS_WELCOMELOGONUI           27          // New friendly logon UI
#define OS_DOMAINMEMBER             28          // Is this machine a member of a domain (eg NOT a workgroup)
#define OS_ANYSERVER                29          // is this machine any type of server? (eg datacenter or advanced server or server)?
#define OS_WOW6432                  30          // Is this process a 32-bit process running on an 64-bit platform?
#define OS_WEBSERVER                31          // Web Edition Server
#define OS_SMALLBUSINESSSERVER      32          // SBS Server
#define OS_TABLETPC                 33          // Are we running on a TabletPC?
#define OS_SERVERADMINUI            34          // Should defaults lean towards those preferred by server administrators?
#define OS_MEDIACENTER              35          // eHome Freestyle Project
#define OS_APPLIANCE                36          // Windows .NET Appliance Server
#define OS_STARTER 					38 			// Windows XP Starter Edition
#define OS_WEPOS 					43 			// Windows Embedded for Point of Service
#define OS_FUNDAMENTALS 			44			// Windows Fundamentals for Legacy PCs (WinFLP / Eiger)
#define OS_WES 						45			// Windows Embedded Standard

#define OSENTRY(x) {x, #x}

typedef BOOL (WINAPI *pIsOS)(DWORD dwOS);

struct OsEntry{
	DWORD dw;
    const char* str;
};

OsEntry available_oses[]={
	OSENTRY(OS_WINDOWS), OSENTRY(OS_NT), OSENTRY(OS_WIN95ORGREATER), OSENTRY(OS_NT4ORGREATER), OSENTRY(OS_WIN98ORGREATER), OSENTRY(OS_WIN98_GOLD), OSENTRY(OS_WIN2000ORGREATER), OSENTRY(OS_WIN2000PRO), OSENTRY(OS_WIN2000SERVER), OSENTRY(OS_WIN2000ADVSERVER), OSENTRY(OS_WIN2000DATACENTER), OSENTRY(OS_WIN2000TERMINAL), OSENTRY(OS_EMBEDDED), OSENTRY(OS_TERMINALCLIENT), OSENTRY(OS_TERMINALREMOTEADMIN), OSENTRY(OS_WIN95_GOLD), OSENTRY(OS_MEORGREATER), OSENTRY(OS_XPORGREATER), OSENTRY(OS_HOME), OSENTRY(OS_PROFESSIONAL), OSENTRY(OS_DATACENTER), OSENTRY(OS_ADVSERVER), OSENTRY(OS_SERVER), OSENTRY(OS_TERMINALSERVER), OSENTRY(OS_PERSONALTERMINALSERVER), OSENTRY(OS_FASTUSERSWITCHING), OSENTRY(OS_WELCOMELOGONUI), OSENTRY(OS_DOMAINMEMBER), OSENTRY(OS_ANYSERVER), OSENTRY(OS_WOW6432), OSENTRY(OS_WEBSERVER), OSENTRY(OS_SMALLBUSINESSSERVER), OSENTRY(OS_TABLETPC), OSENTRY(OS_SERVERADMINUI), OSENTRY(OS_MEDIACENTER), OSENTRY(OS_APPLIANCE), OSENTRY(OS_STARTER), OSENTRY(OS_WEPOS), OSENTRY(OS_FUNDAMENTALS), OSENTRY(OS_WES),
	{0, NULL}
};

int main(int argc, char* argv[])
{	
	HINSTANCE hShlwapi=LoadLibrary("shlwapi.dll");
	
	if (hShlwapi) {
		pIsOS fnIsOS;
		//IsOS is available starting from Windows 2000
		//In versions earlier than Windows Vista IsOS should be exported by ordinal
		if (fnIsOS=(pIsOS)GetProcAddress(hShlwapi, (LPCSTR)437)) {
			for (OsEntry* os=available_oses; os->str; os++)
				printf("IsOS(%s) = %s\n", os->str, fnIsOS(os->dw)?"TRUE":"FALSE");
		} else {
			printf("Can't load IsOS from shlwapi.dll!\n");
		}
		FreeLibrary(hShlwapi);
	} else {
		printf("Can't load shlwapi.dll!\n");
	}
	
	printf("\n");
    
	printf("Press ANY KEY to continue...\n");
	int c;
	c=getch();
	if (c==0||c==224) getch();
	return 0;
}

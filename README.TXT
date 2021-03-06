Windows Version Test Tools

1. License
----------
Copyright (c) 2016-2017 Lcferrum

This program comes with no warranty.
You must use this program at your own risk.
Licensed under BSD license - see LICENSE.TXT file for details.

2. About
--------
It is not "yet another all-in-one class to get Windows version". Nope. Rather
it is a collection of tools that can be used in such classes (or functions). It
won't tell you Windows version right away, but show a lot of various bits of
information, collected from the OS, that can be processed using some algorithm
to determine Windows version. Something like: ...blah blah blah, if
MajorVersion=5 and MinorVersion=2 - it's Windows Server 2003 or Server 2003 R2
if GetSystemMetrics(SM_SERVERR2)=TRUE, but not in the case of ProductType
=VER_NT_WORKSTATION and ProcessorArchitecture=PROCESSOR_ARCHITECTURE_AMD64 when
it's Windows XP x64, blah blah blah...

So why not publish algorithm itself? 

There is actually no point. You can search on the internet. There are tons of
such algorithms published elsewhere. Anyway, Windows is evolving and you need
to update them every time new version of Windows is released to be able to
detect it. While means of obtaining basic version information, that all these
algorithms use, stay more or less the same. And it's not so trivial as you may
actually think.

Also, it's better not to "test by version", but "test by feature". If your code
depends on some OS functionality that is present only on certain OS versions -
do not check version, but check if needed functionality is actually available.
And this collection of tools emphasize this approach: not only it never depends
on OS version and checks feature availability directly, but it also shows how
cumbersome might be "simple" code that "just" determines OS version.

Finally, this collection shows that by carefully following "test by
feature" principle it is possible to design the code that can be run almost on
any version of Windows. Specifically, this code runs on any 32-bit x86 and
64-bit x86-64 versions of Windows, i.e.: Windows 3.1 w/ Win32s, Windows NT 3.1,
Windows 95, Windows XP x64, Windows 10, etc. Other architectures, like IA-64,
that were at one time or another supported by Windows, are out of scope.

3. MSVCRT and OS compatibility
------------------------------
Windows Version Test Tools binary compiled using current distribution for x86
platform, with maximum compatibility options enabled, requires MSVCRT v2.0 or
newer. Needed version is already distributed with Windows starting from Win NT4
and Win 95. For Win NT3.X and Win 3.1 w/ Win32s you should get MSVCRT
separately. For example, MSVCRT v2.0 (MSVCRT20.DLL) for Win NT3.X and Win32s
(separate DLLs) is included with Microsoft Visual C++ 2.0 distribution. Install
it with corresponding Visual C++ redistributable or, in case of standalone DLL,
copy it to system folder. Windows Version Test Tools x86 binary accepts MSVCRT
named either MSVCRT.DLL, MSVCRT40.DLL or MSVCRT20.DLL.

64-bit binary compiled for x86-64 platform supports any MSVCRT version that is
supported by compatible OS.

Windows 3.1 should have Win32s installed to be able to run 32-bit x86 binaries.

4. Where to get
---------------
You can compile binary by yourself (refer to COMPILE.TXT that comes with the
sources) or download binary distribution from GitHub releases section. Project
homepage at GitHub:

	https://github.com/lcferrum/wvtt

5. Usage
--------
Besides just running binary AS-IS and gazing on it's output (or maybe parsing
it) you can trash the sources and use whatever part of it you may find suitable
for your own needs (but don't forget about BSD license terms!).

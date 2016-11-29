Windows Version Test Tools

1. License
----------
Copyright (c) 2016 Lcferrum

This program comes with no warranty.
You must use this program at your own risk.
Licensed under BSD license - see LICENSE.TXT file for details.

2. About
--------
It is not "yet another all-in-one class to get Windows version". Nope. Rather
it is a collection of tools that can be used in such classes (or functions). It
won't tell you Windows version right away, but show a lot of various bits of
information, collected from the OS, that can be processed using some rules to
determine Windows version. ...if MajorVersion=5 and MinorVersion=2 - it's
Windows Server 2003 or Server 2003 R2 if GetSystemMetrics(SM_SERVERR2)=TRUE,
but not in the case of ProductType=VER_NT_WORKSTATION and ProcessorArchitecture
=PROCESSOR_ARCHITECTURE_AMD64 when it's Windows XP x64... Yeah, these kind of
rules.

So why not completing the cycle and implementing mentioned rules besides
providing tools to mine data for them? 

First of all, there is no complete set of such rules because Windows is 
evolving and you need to add new rules every time new version of Windows is
released to be able to detect it. While means of getting information, that will
be processed by such rules, stay more or less the same.

Second, it's better not to "test by version", but "test by feature". If your
code depends on some OS functionality that is present only on certain OS
versions - do not check version, but check if needed functionality is actually
available. And this collection of tools emphasize this approach: not only it
never depends on OS version but checks feature availability directly, but it
also shows how cumbersome might be "simple" code that "just" determines OS
version.

And finally, this collection shows that by carefully following "test by
feature" principle it is possible to design the code that can be run almost on
any version of Windows. Specifically, this code runs on any 32-bit and x86-64
versions of Windows, i.e.: Windows 3.1 w/ Win32s, Windows NT 3.1, Windows
95, Windows XP x64, Windows 10, etc.

3. Where to get
---------------
You can compile binary by yourself (refer to COMPILE.TXT that comes with the
sources) or download binary distribution from GitHub releases section. Project
homepage at GitHub:

	https://github.com/lcferrum/wvtt

4. Usage
--------
Besides just running binary AS-IS and gazing on it's output (or maybe parsing
it) you can trash the sources and use whatever part of it you may find suitable
for your own needs (but don't forget about BSD license terms!).

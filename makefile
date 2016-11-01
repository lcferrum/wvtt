# Usage:
# make CC=COMPILER
#	Makes VerTest binary
# make clean
#	Cleans directory of executables
# make upx
#	Pack executables with upx
# make UPSTREAM_INC=PATH
#	Change include path for clang++/g++ (default is /c/cygwin/usr/i686-w64-mingw32/sys-root/mingw/include/)

# Conditionals
ifeq (,$(if $(filter-out upx clean,$(MAKECMDGOALS)),,$(MAKECMDGOALS)))
ifeq (,$(filter $(CC),i686-w64-mingw32-g++ x86_64-w64-mingw32-g++ g++ clang++))
$(info Compiler not selected! Please set CC variable.)
$(info Possible CC values: x86_64-w64-mingw32-g++, i686-w64-mingw32-g++, g++, clang++)
$(error CC not set)
endif
endif

# Common section
RM=rm -f
UPX=upx
CFLAGS=-std=c++11 -Wno-write-strings -D_WIN32_WINNT=0x0600 -DNOMINMAX $(NT3)
LDFLAGS=-static-libgcc -static-libstdc++ -lversion -Wl,--enable-stdcall-fixup -O2 -s
UPSTREAM_INC=/c/cygwin/usr/i686-w64-mingw32/sys-root/mingw/include/
SRC=vt.cpp externs.cpp fp_routines.cpp asm_patches.S
OBJ=$(patsubst %.S,%.o,$(patsubst %.cpp,%.o,$(patsubst %.rc,%.o,$(SRC))))
TARGET=vt.exe

# WinNT3.x specific common section
ifdef NT3
	LDFLAGS+=-Wl,-pie -Wl,-e_CompatCRTStartup -Wl,--subsystem,console:3.10
	SRC+=compat.cpp
	override NT3:=-DNT3=$(NT3)
endif

# Compiler specific section
ifeq ($(CC),x86_64-w64-mingw32-g++)
	CFLAGS+=-Wno-attributes
endif
ifeq ($(CC),i686-w64-mingw32-g++)
	CFLAGS+=-Wno-attributes
endif
# Extra options for outdated clang++/g++ with upstream includes to generate binaries compatible with Win 9x/NT4
# i386 is minimum system requirement for Windows 95 (MinGW 4.7.2 default arch)
# i486 is minimum system requirement for Windows NT4
# i386 is minimum system requirement for Windows NT3.1
# i386 is minimum system requirement for Win32s
# It's assumed that g++ (MinGW) version is 4.7.2, clang++ (LLVM) version is 3.6.2 and includes are from MinGW-w64 4.9.2
ifeq ($(CC),clang++)
	INC=-I$(UPSTREAM_INC)
	CFLAGS+=-target i386-pc-windows-gnu -march=i386 -Wno-dll-attribute-on-redeclaration -Wno-ignored-attributes -Wno-deprecated-register -Wno-inconsistent-dllimport -Wno-unused-value -DUMDF_USING_NTSTATUS
endif
ifeq ($(CC),g++)
	INC=-I$(UPSTREAM_INC)
	CFLAGS+=-march=i386 -Wno-attributes -DUMDF_USING_NTSTATUS
endif

.PHONY: all clean upx
.INTERMEDIATE: $(OBJ)

all: $(TARGET)

$(TARGET): $(OBJ) 
	$(CC) -o $@ $(OBJ) $(LDFLAGS)
	
%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS) $(INC)

%.o: %.S
	$(CC) -c -o $@ $< $(CFLAGS) $(INC)
	
upx:
	$(UPX) $(TARGET) ||:

clean:
	$(RM) $(TARGET) $(OBJ) ||:

# Usage:
# make BUILD=COMPILER HOST=OS_TYPE
#	Makes VerTest binary
# make clean
#	Cleans directory of executables
# make upx
#	Pack executables with upx
# make UPSTREAM_INC=PATH
#	Change include path for clang++ 3.6.2 and g++ 4.7.2 (default is /c/cygwin/usr/i686-w64-mingw32/sys-root/mingw/include/)

# Conditionals
ifeq (,$(if $(filter-out upx clean,$(MAKECMDGOALS)),,$(MAKECMDGOALS)))
ifeq (,$(and $(filter $(BUILD),MinGW-w64 MinGW MinGW_472 Clang Clang_362),$(filter $(HOST),x86-64 x86 x86_3X)))
$(info Compiler and/or OS type is invalid! Please correctly set BUILD and HOST variables.)
$(info Possible BUILD values: MinGW-w64, MinGW, MinGW_472, Clang, Clang_362)
$(info Possible HOST values: x86-64, x86, x86_3X)
$(error BUILD/HOST is invalid)
endif
endif

# Common section
RM=rm -f
UPX=upx
CFLAGS=$(CPP11) -D_WIN32_WINNT=0x0600 -O2 $(X86_3X)
LDFLAGS=-static-libgcc -static-libstdc++ -lversion -Wl,--enable-stdcall-fixup -O2 -s
UPSTREAM_INC=/c/cygwin/usr/i686-w64-mingw32/sys-root/mingw/include/
SRC=vt.cpp externs.cpp fp_routines.cpp asm_patches.S
OBJ=$(patsubst %.S,%.o,$(patsubst %.cpp,%.o,$(patsubst %.rc,%.o,$(SRC))))
TARGET=vt.exe

# Compiler/OS specific sections
# N.B.:
# i386 is minimum system requirement for Windows 95 (MinGW 4.7.2 default arch), maximum is pentium2 (doesn't handle SSE instructions)
# i486 is minimum system requirement for Windows NT4, maximum is pentium2 (doesn't handle SSE instructions)
# i386 is minimum system requirement for Windows NT3.1
# i386 is minimum system requirement for Win32s

# MinGW 4.7.2 with includes from current MinGW-w64
ifeq ($(BUILD),MinGW_472)
	CC=g++
	CPP11=-std=c++11
	INC=-I$(UPSTREAM_INC)
	CFLAGS+=-march=i386 -Wno-attributes -DUMDF_USING_NTSTATUS
ifeq ($(HOST),x86-64)
$(error not implemented)
endif
ifeq ($(HOST),x86)
endif
ifeq ($(HOST),x86_3X)
	LDFLAGS+=-Wl,-pie -Wl,-e_CompatCRTStartup -Wl,--subsystem,console:3.10
	SRC+=compat.cpp
	X86_3X=-DNT3=1
endif
endif

# Current MinGW
ifeq ($(BUILD),MinGW)
	CC=g++
	CPP11=-std=gnu++11
ifeq ($(HOST),x86-64)
$(error not implemented)
endif
ifeq ($(HOST),x86)
endif
ifeq ($(HOST),x86_3X)
	LDFLAGS+=-Wl,-pie -Wl,-e_CompatCRTStartup -Wl,--subsystem,console:3.10
	SRC+=compat.cpp
	X86_3X=-DNT3=1
endif
endif

# Current MinGW-w64
ifeq ($(BUILD),MinGW-w64)
	CPP11=-std=c++11
ifeq ($(HOST),x86-64)
	CC=x86_64-w64-mingw32-g++
endif
ifeq ($(HOST),x86)
	CFLAGS+=-march=pentium2
	CC=i686-w64-mingw32-g++
endif
ifeq ($(HOST),x86_3X)
$(error not implemented)
endif
endif

# Clang 3.6.2 with includes from current MinGW-w64
ifeq ($(BUILD),Clang_362)
	CC=clang++
	CPP11=-std=c++11
	INC=-I$(UPSTREAM_INC)
	CFLAGS+=-target i386-pc-windows-gnu -march=i386 -Wno-dll-attribute-on-redeclaration -Wno-ignored-attributes -Wno-deprecated-register -Wno-inconsistent-dllimport -Wno-unused-value -DUMDF_USING_NTSTATUS
ifeq ($(HOST),x86-64)
$(error not implemented)
endif
ifeq ($(HOST),x86)
endif
ifeq ($(HOST),x86_3X)
	LDFLAGS+=-Wl,-pie -Wl,-e_CompatCRTStartup -Wl,--subsystem,console:3.10
	SRC+=compat.cpp
	X86_3X=-DNT3=1
endif
endif

# Current Clang
ifeq ($(BUILD),Clang)
ifeq ($(HOST),x86-64)
$(error not implemented)
endif
ifeq ($(HOST),x86)
$(error not implemented)
endif
ifeq ($(HOST),x86_3X)
$(error not implemented)
endif
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

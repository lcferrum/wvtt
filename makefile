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
CFLAGS=-std=c++11 -Wno-write-strings -D_WIN32_WINNT=0x0600 -DNOMINMAX
LDFLAGS=-static-libgcc -static-libstdc++ -lversion -s
UPSTREAM_INC=/c/cygwin/usr/i686-w64-mingw32/sys-root/mingw/include/
SRC=vt.cpp externs.cpp
OBJ=$(patsubst %.S,%.o,$(patsubst %.cpp,%.o,$(patsubst %.rc,%.o,$(SRC))))
TARGET=vt.exe

# Compiler specific section
ifeq ($(CC),x86_64-w64-mingw32-g++)
	WINDRES=x86_64-w64-mingw32-windres
endif
ifeq ($(CC),i686-w64-mingw32-g++)
	WINDRES=i686-w64-mingw32-windres
endif
# Extra options for outdated clang++/g++ with upstream includes to generate binaries compatible with Win 9x/NT4
# i386 is minimum system requirement for Windows 95 (MinGW 4.7.2 default arch)
# i486 is minimum system requirement for Windows NT4
# It's assumed that g++ (MinGW) version is 4.7.2, clang++ (LLVM) version is 3.6.2 and includes are from MinGW-w64 4.9.2
ifeq ($(CC),clang++)
	INC=-I$(UPSTREAM_INC)
	CFLAGS+=-target i486-pc-windows-gnu -march=i486 -Wno-ignored-attributes -Wno-deprecated-register -Wno-inconsistent-dllimport -Wno-unused-value -DUMDF_USING_NTSTATUS
	WINDRES=windres
endif
ifeq ($(CC),g++)
	INC=-I$(UPSTREAM_INC)
	CFLAGS+=-Wno-attributes -DUMDF_USING_NTSTATUS
	WINDRES=windres
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
	
%.o: %.rc
	$(WINDRES) $< $@ $(filter -D% -U% -I%,$(CFLAGS)) $(INC)
	
upx:
	$(UPX) $(TARGET) ||:

clean:
	$(RM) $(TARGET) $(OBJ) ||:
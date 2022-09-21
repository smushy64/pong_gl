# Desired compiler and C++ version
CC = g++ -std=c++17
# Change between DEBUG/RELEASE
# Change target path as well
# remove -mwindows for debug builds
CFLAGS    = $(RELEASE)
LNKFLAGS  = --static -mwindows
TARGETDIR = ./bin/release

# Mingw include path
MINGWINC = C:/msys64/mingw64/include

# defines
DEF      = -D UNICODE -D WINDOWS

# DONOT EDIT BEYOND THIS POINT!!! ===============================================

DEBUG   = $(DFLAGS) $(foreach D, $(INC), -I$(D)) $(DEPFLAGS)
RELEASE = $(RFLAGS) $(foreach D, $(INC), -I$(D)) $(DEPFLAGS)

BINARY = $(TARGETDIR)/$(EXE)
EXE = PongGL.exe

RES = ./resources
DIR = ./src ./src/platform ./src/core
OBJDIR = /bin/obj

WARN     = -Wall -Wextra
OPT      = -O0 -g
DFLAGS   = $(WARN) $(DEF) $(OPT) -D DEBUG
RFLAGS   = $(DEF) -O2
DEPFLAGS = -MP -MD
INC      = ./src $(MINGWINC)
LNK      = -static-libstdc++ -static-libgcc -lmingw32 -lopengl32 -lgdi32

CPP      = $(foreach D, $(DIR), $(wildcard $(D)/*.cpp))
C        = $(foreach D, ./src, $(wildcard $(D)/*.c))
OBJ      = $(patsubst %.c,%.o, $(C)) $(patsubst %.cpp,%.o, $(CPP))
DEPS     = $(patsubst %.c,%.d,$(C)) $(patsubst %.cpp,%.d,$(CPP))

all: $(BINARY)

run: all copy rm_nul
	$(BINARY)

-include $(DEPS)
$(BINARY): $(OBJ)
	$(CC) -o $@ $(LIB) $^ $(LNK) $(LNKFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

copy:
	-@robocopy $(RES) $(TARGETDIR)/resources/ > nul

rm_nul:
	-@rm nul

cleano:
	-@rm $(OBJ) $(DEPS)

clean: cleano
	-@rm $(BINARY); rm -r $(TARGETDIR)/resources

.PHONY: run all clean cleano
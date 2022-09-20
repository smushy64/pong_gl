# Desired compiler and C++ version
CC = g++ -std=c++20
# Change between DEBUG/RELEASE
# Change target path as well
CFLAGS = $(DEBUG)
TARGETDIR = ./bin/debug


# DONOT EDIT BEYOND THIS POINT!!! ===============================================

DEBUG   = $(DFLAGS) $(foreach D, $(INC), -I$(D)) $(DEPFLAGS)
RELEASE = $(RFLAGS) $(foreach D, $(INC), -I$(D)) $(DEPFLAGS)

BINARY = $(TARGETDIR)/$(EXE)
EXE = PongGL.exe

RES = ./resources
DIR = ./src ./src/platform ./src/core
OBJDIR = /bin/obj

WARN     = -Wall
OPT      = -O0 -g
DEF      = -D UNICODE -D WINDOWS
DFLAGS   = $(WARN) $(DEF) $(OPT) -D DEBUG
RFLAGS   = $(DEF) -O2
DEPFLAGS = -MP -MD
INC      = ./src C:/msys64/mingw64/include
LNK      = -lmingw32 -lopengl32 -lgdi32 -lfreetype

CPP      = $(foreach D, $(DIR), $(wildcard $(D)/*.cpp))
C        = $(foreach D, ./src, $(wildcard $(D)/*.c))
OBJ      = $(patsubst %.c,%.o, $(C)) $(patsubst %.cpp,%.o, $(CPP))
DEPS     = $(patsubst %.c,%.d,$(C)) $(patsubst %.cpp,%.d,$(CPP))

all: $(BINARY)

run: all copy rm_nul
	$(BINARY)

-include $(DEPS)
$(BINARY): $(OBJ)
	$(CC) -o $@ $(LIB) $^ $(LNK)

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

.PHONY: all clean cleano
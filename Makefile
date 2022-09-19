CC = g++ -std=c++17

build: copy_resources rm_null
	$(CC) $(debug_flags) $(src) -o $(bin)/debug/$(exe) $(def) -D DEBUG $(lnk) -I $(minc) $(inc)

build_r: copy_resources_r rm_null
	$(CC) $(release_flags) $(src) -o $(bin)/release/$(exe) $(def) $(lnk) -I $(minc) $(inc) $(release_linker_flags)

run:
	$(bin)/debug/$(exe)

run_r:
	$(bin)/release/$(exe)

clean:
	rm -r -f $(bin)/debug/*

clean_r:
	rm -r -f $(bin)/release/*

copy_resources:
	-@robocopy $(res) $(bin)/debug/resources/ > nul

copy_resources_r:
	-@robocopy $(res) $(bin)/release/resources/ > nul

rm_null:
	rm ./nul

res = ./resources

exe = PongGL.exe

def = -D WINDOWS -D UNICODE

src = ./src/*.cpp ./src/*.c ./src/platform/*.cpp ./src/core/*.cpp
inc = -I ./src/

bin = ./bin

debug_flags = -Wall -O0
release_flags = -O2

minc = C:/msys64/mingw64/include

lnk = -lmingw32 -lopengl32 -lgdi32 -lfreetype

release_linker_flags = -mwindows

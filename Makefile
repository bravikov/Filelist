appName = filelist
buildDir = build
std = --std='c++11'

all:
	mkdir -p $(buildDir)
	g++ -o $(buildDir)/$(appName) $(std) src/main.cpp
	i686-w64-mingw32-g++ -o $(buildDir$)/$(appName).exe $(std) -static src/main.cpp

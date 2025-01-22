@echo off

for /f "tokens=*" %%i in (..\version.conf) do set VERSION=%%i

mkdir build_spdlog
mkdir build

cd build_spdlog
cmake ..\..\lib\spdlog -G "MinGW Makefiles"
mingw32-make.exe

cd ..\build
cmake ..\.. -G "MinGW Makefiles"
mingw32-make.exe install

cd ..

rmdir /S /Q shipping
mkdir shipping\launcher\windows

tar -cvzf shipping\vortex\windows\vortex_%VERSION%.tar.gz dist
certutil -hashfile shipping\vortex\windows\vortex_%VERSION%.tar.gz SHA256 > shipping\vortex\windows\vortex_%VERSION%.tar.gz.sha256

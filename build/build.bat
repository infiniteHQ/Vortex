@echo off

for /f "tokens=*" %%i in (..\version.conf) do set VERSION=%%i

rmdir /S /Q .\dist
rmdir /S /Q .\build\bin

mkdir build_spdlog
mkdir build

cd build_spdlog
cmake ..\..\lib\spdlog -G "MinGW Makefiles"
mingw32-make.exe -j%NUMBER_OF_PROCESSORS%

cd ..\build
cmake ..\.. -G "MinGW Makefiles"
mingw32-make.exe install -j%NUMBER_OF_PROCESSORS%

cd ..

xcopy /E /Y .\build\bin\ dist\%VERSION%\bin

rmdir /S /Q shipping
mkdir shipping\vortex\windows

tar -cvzf shipping\vortex\windows\vortex_%VERSION%.tar.gz dist
certutil -hashfile shipping\vortex\windows\vortex_%VERSION%.tar.gz SHA256 > shipping\vortex\windows\vortex_%VERSION%.tar.gz.sha256

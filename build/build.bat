@echo off

for /f "tokens=*" %%i in (..\version.conf) do set VERSION=%%i

rmdir /S /Q .\dist
rmdir /S /Q .\build\bin

mkdir build_spdlog
mkdir build

cd build

cmake -G "Visual Studio 17" -A x64 ..\..

for /f %%i in ('powershell -command "(Get-WmiObject -Class Win32_Processor).NumberOfLogicalProcessors"') do set THREADS=%%i

cmake --build . --config Release -- /m:%THREADS%
cmake --install . --config Release

xcopy /Y /E /I .\bin\Release\* .\
xcopy /Y /E /I .\build\Release\* .\build
xcopy /Y /E /I .\Release\vortex_shared.dll .\bin
xcopy /Y /E /I .\Release\vortex.exe .\bin
xcopy /Y /E /I .\Release\vortex_utils.exe .\bin
xcopy /Y ..\handle_crash.bat .\bin


echo %cd%
cd ..
echo %cd%


xcopy /E /Y .\build\bin\ dist\%VERSION%\bin
xcopy /E /Y .\build\Release\ prod
xcopy /E /Y .\build\build\cherry_build\Release\ prod

rmdir /S /Q shipping
mkdir shipping\vortex\windows

tar -cvzf shipping\vortex\windows\vortex_%VERSION%.tar.gz dist
certutil -hashfile shipping\vortex\windows\vortex_%VERSION%.tar.gz SHA256 > shipping\vortex\windows\vortex_%VERSION%.tar.gz.sha256

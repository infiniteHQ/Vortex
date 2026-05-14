@echo off

set /p VERSION=<"..\version.conf"
set "SCRIPT_DIR=%~dp0"
set "VORTEX_PATH=%SCRIPT_DIR%dist\%VERSION%\bin"
set "PROJECT_PATH=%SCRIPT_DIR%..\tests\project"

call "%VORTEX_PATH%\vx.bat" "%PROJECT_PATH%" 
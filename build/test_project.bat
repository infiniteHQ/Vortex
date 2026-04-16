@echo off
setlocal EnableExtensions EnableDelayedExpansion

set /p VERSION=<"..\version.conf"

set "SCRIPT_DIR=%~dp0"
set "VORTEX_PATH=%SCRIPT_DIR%dist\%VERSION%\bin"
set "PROJECT_PATH=%SCRIPT_DIR%..\tests\project"

set "datestr=%date:~3,2%-%date:~0,2%-%date:~6,4%"
set "timestr=%time:~0,2%-%time:~3,2%-%time:~6,2%"
set "timestr=%timestr: =0%"
set /a "random_num=%random% %% 9000 + 1000"

set "SESSION_ID=editor-%datestr%-%timestr%-%random_num%"

set "SESSION_ARG=--session_id="%SESSION_ID%""

echo SESSION_ID=%SESSION_ID%
echo SESSION_ARG=%SESSION_ARG%

cd /d "%PROJECT_PATH%"

call "%VORTEX_PATH%\handle_crash.bat" ^
  "%USERPROFILE%\.vx\sessions\%SESSION_ID%\crash\core_dumped.txt" ^
  "%VORTEX_PATH%\vortex.exe --editor %SESSION_ARG%" ^
  __END__ ^
  "%VORTEX_PATH%\vortex.exe --crash %SESSION_ARG%"
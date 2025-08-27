@echo off
setlocal enabledelayedexpansion

set /p VERSION=<"..\version.conf"

set SCRIPT_DIR=%~dp0

set VORTEX_PATH=%SCRIPT_DIR%dist\%VERSION%\bin
set PROJECT_PATH=%SCRIPT_DIR%..\tests\project

set DATESTR=%date:~3,2%-%date:~0,2%-%date:~6,4%
set TIMESTR=%time:~0,2%-%time:~3,2%-%time:~6,2%
set TIMESTR=%TIMESTR: =0%
set /a RANDNUM=%RANDOM% %% 9000 + 1000
set SESSION_ID=editor-%DATESTR%-%TIMESTR%-%RANDNUM%

cd /d "%PROJECT_PATH%"

call "%VORTEX_PATH%\handle_crash.bat" "%USERPROFILE%\.vx\sessions\%SESSION_ID%\crash\core_dumped.txt" "%VORTEX_PATH%\vortex.exe" --editor --session_id=%SESSION_ID%

endlocal

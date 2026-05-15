@echo off
setlocal EnableExtensions EnableDelayedExpansion
if "%~1"=="" exit /b 1

for %%I in ("%~f0") do set "SCRIPT_DIR=%%~dpI"

set "OUTPUT_FILE=%~1"
shift

set "VORTEX_UTILS=%SCRIPT_DIR%vortex_utils.exe"
set "MANIFEST=%SCRIPT_DIR%manifest.json"

set "MAIN_EXE="
set "MAIN_ARGS="
set "MAIN_CMD_DISPLAY="

:build_main
if "%~1"=="" goto end_main
if "%~1"=="__END__" (
  shift
  goto end_main
)
if not defined MAIN_EXE (
  set "MAIN_EXE=%~1"
  set "MAIN_CMD_DISPLAY=%~1"
) else (
  set "MAIN_ARGS=!MAIN_ARGS! %~1"
  set "MAIN_CMD_DISPLAY=!MAIN_CMD_DISPLAY! %~1"
)
shift
goto build_main

:end_main
set "END_EXE="
set "END_ARGS="

:build_end
if "%~1"=="" goto end_end
if not defined END_EXE (
  set "END_EXE=%~1"
) else (
  set "END_ARGS=!END_ARGS! %~1"
)
shift
goto build_end

:end_end
set "SESSION_ID="
set "PROJECT_PATH="
set "TEMP_ARGS=!MAIN_ARGS!"

:parse_loop
if "!TEMP_ARGS!"=="" goto parse_done
for /f "tokens=1,*" %%A in ("!TEMP_ARGS!") do (
  set "TOKEN=%%A"
  set "TEMP_ARGS=%%B"
)
set "CHECK=!TOKEN:~0,13!"
if "!CHECK!"=="--session_id=" set "SESSION_ID=!TOKEN:~13!"
set "CHECK=!TOKEN:~0,15!"
if "!CHECK!"=="--project_path=" set "PROJECT_PATH=!TOKEN:~15!"
goto parse_loop

:parse_done
set "VERSION="
if exist "%MANIFEST%" (
  for /f "tokens=2 delims=:, " %%V in ('findstr /i "\"version\"" "%MANIFEST%"') do (
    set "VERSION=%%~V"
  )
)

set "CURRENT_USER=%USERNAME%"

if not exist "%USERPROFILE%\.vx\sessions\" (
  mkdir "%USERPROFILE%\.vx\sessions\"
)
if not exist "%USERPROFILE%\.vx\sessions\active_sessions.json" (
  echo {"sessions":[]} > "%USERPROFILE%\.vx\sessions\active_sessions.json"
)

if not "!SESSION_ID!"=="" (
  if exist "%VORTEX_UTILS%" (
    "%VORTEX_UTILS%" -astj --session_id=!SESSION_ID! --version=!VERSION! --user=!CURRENT_USER! --project_path=!PROJECT_PATH!
  )
)

echo.
echo Executing: !MAIN_CMD_DISPLAY!
echo.
"!MAIN_EXE!" !MAIN_ARGS!
set "EXIT_CODE=!ERRORLEVEL!"

if not "!EXIT_CODE!"=="0" (
  echo.
  echo Command failed with exit code !EXIT_CODE!.
  echo.
  if exist "%OUTPUT_FILE%" (
    echo ==== CRASH REPORT ====
    type "%OUTPUT_FILE%"
    echo ======================
  ) else (
    echo WARNING: No crash report found.
    echo Expected: %OUTPUT_FILE%
  )
  if not "!SESSION_ID!"=="" (
    if exist "%VORTEX_UTILS%" (
      "%VORTEX_UTILS%" -wses --session_id=!SESSION_ID! --state=failed
    )
  )
  if not "!END_EXE!"=="" (
    echo.
    echo Launching crash handler UI...
    echo.
    "!END_EXE!" !END_ARGS!
  )
) else (
  echo.
  echo Command executed successfully.
  if not "!SESSION_ID!"=="" (
    if exist "%VORTEX_UTILS%" (
      "%VORTEX_UTILS%" -wses --session_id=!SESSION_ID! --state=finished
    )
  )
)

exit /b !EXIT_CODE!
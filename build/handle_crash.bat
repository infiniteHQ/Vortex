@echo off
setlocal EnableExtensions EnableDelayedExpansion

if "%~1"=="" exit /b 1

set "OUTPUT_FILE=%~1"
shift

set "SCRIPT_DIR=%~dp0"
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

:: Build end command
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

:: Extract session_id from main args
set "SESSION_ID="
for %%A in (!MAIN_ARGS!) do (
  set "ARG=%%~A"
  if "!ARG:~0,13!"=="--session_id=" (
    set "SESSION_ID=!ARG:~13!"
  )
)

:: Extract project_path from main args
set "PROJECT_PATH="
for %%A in (!MAIN_ARGS!) do (
  set "ARG=%%~A"
  if "!ARG:~0,15!"=="--project_path=" (
    set "PROJECT_PATH=!ARG:~15!"
  )
)

:: Read version from manifest.json
set "VERSION="
if exist "%MANIFEST%" (
  for /f "tokens=2 delims=:, " %%V in ('findstr /i "\"version\"" "%MANIFEST%"') do (
    set "VERSION=%%~V"
  )
)

:: Get current user
set "CURRENT_USER=%USERNAME%"

:: Register session before launching
if not "%SESSION_ID%"=="" (
  if exist "%VORTEX_UTILS%" (
    "%VORTEX_UTILS%" -astj --session_id="%SESSION_ID%" --version="%VERSION%" --user="%CURRENT_USER%" --project_path="%PROJECT_PATH%"
  )
)

echo.
echo Executing: %MAIN_CMD_DISPLAY%
echo.

"%MAIN_EXE%" !MAIN_ARGS!
set "EXIT_CODE=%ERRORLEVEL%"

if not "%EXIT_CODE%"=="0" (
  echo.
  echo Command failed with exit code %EXIT_CODE%.
  echo.

  if exist "%OUTPUT_FILE%" (
    echo ==== CRASH REPORT ====
    type "%OUTPUT_FILE%"
    echo ======================
  ) else (
    echo WARNING: No crash report found.
    echo Expected:
    echo %OUTPUT_FILE%
  )

  :: Write failed session end state
  if not "%SESSION_ID%"=="" (
    if exist "%VORTEX_UTILS%" (
      "%VORTEX_UTILS%" -wses --session_id="%SESSION_ID%" --state="failed"
    )
  )

  if not "%END_EXE%"=="" (
    echo.
    echo Launching crash handler UI...
    echo.
    "%END_EXE%" !END_ARGS!
  )
) else (
  echo.
  echo Command executed successfully.

  :: Write finished session end state
  if not "%SESSION_ID%"=="" (
    if exist "%VORTEX_UTILS%" (
      "%VORTEX_UTILS%" -wses --session_id="%SESSION_ID%" --state="finished"
    )
  )
)

exit /b %EXIT_CODE%
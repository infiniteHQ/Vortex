@echo off
setlocal EnableExtensions EnableDelayedExpansion
if "%~1"=="" exit /b 1

for %%I in ("%~f0") do set "SCRIPT_DIR=%%~dpI"

set "OUTPUT_FILE=%~1"
shift

set "VORTEX_UTILS=%SCRIPT_DIR%vortex_utils.exe"
set "MANIFEST=%SCRIPT_DIR%manifest.json"
set "LOG_FILE=%SCRIPT_DIR%vortex_utils_calls.log"

set "MAIN_EXE="
set "MAIN_ARGS="
set "MAIN_CMD_DISPLAY="

echo [%DATE% %TIME%] === handle_crash session started === >> "%LOG_FILE%"
echo [%DATE% %TIME%] OUTPUT_FILE=%OUTPUT_FILE% >> "%LOG_FILE%"

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
if "!CHECK!"=="--session_id=" (
  set "SESSION_ID=!TOKEN:~13!"
)

set "CHECK=!TOKEN:~0,15!"
if "!CHECK!"=="--project_path=" (
  set "PROJECT_PATH=!TOKEN:~15!"
)
goto parse_loop
:parse_done

set "VERSION="
if exist "%MANIFEST%" (
  for /f "tokens=2 delims=:, " %%V in ('findstr /i "\"version\"" "%MANIFEST%"') do (
    set "VERSION=%%~V"
  )
)

set "CURRENT_USER=%USERNAME%"

echo [%DATE% %TIME%] Parsed SESSION_ID=!SESSION_ID! >> "%LOG_FILE%"
echo [%DATE% %TIME%] Parsed PROJECT_PATH=!PROJECT_PATH! >> "%LOG_FILE%"
echo [%DATE% %TIME%] Parsed VERSION=!VERSION! >> "%LOG_FILE%"
echo [%DATE% %TIME%] Parsed CURRENT_USER=!CURRENT_USER! >> "%LOG_FILE%"
echo [%DATE% %TIME%] Parsed MAIN_EXE=!MAIN_EXE! >> "%LOG_FILE%"
echo [%DATE% %TIME%] Parsed MAIN_ARGS=!MAIN_ARGS! >> "%LOG_FILE%"
echo [%DATE% %TIME%] Parsed END_EXE=!END_EXE! >> "%LOG_FILE%"
echo [%DATE% %TIME%] Parsed END_ARGS=!END_ARGS! >> "%LOG_FILE%"

if not exist "%USERPROFILE%\.vx\sessions\" (
  mkdir "%USERPROFILE%\.vx\sessions\"
  echo [%DATE% %TIME%] Created sessions directory >> "%LOG_FILE%"
)
if not exist "%USERPROFILE%\.vx\sessions\active_sessions.json" (
  echo {"sessions":[]} > "%USERPROFILE%\.vx\sessions\active_sessions.json"
  echo [%DATE% %TIME%] Created active_sessions.json >> "%LOG_FILE%"
)

if not "!SESSION_ID!"=="" (
  if exist "%VORTEX_UTILS%" (
    echo [%DATE% %TIME%] CALL: "%VORTEX_UTILS%" -astj --session_id=!SESSION_ID! --version=!VERSION! --user=!CURRENT_USER! --project_path=!PROJECT_PATH! >> "%LOG_FILE%"
    "%VORTEX_UTILS%" -astj --session_id=!SESSION_ID! --version=!VERSION! --user=!CURRENT_USER! --project_path=!PROJECT_PATH!
    set "VU_EXIT=!ERRORLEVEL!"
    echo [%DATE% %TIME%] vortex_utils -astj exit code: !VU_EXIT! >> "%LOG_FILE%"
  ) else (
    echo [%DATE% %TIME%] WARNING: vortex_utils.exe not found at %VORTEX_UTILS% >> "%LOG_FILE%"
  )
) else (
  echo [%DATE% %TIME%] SKIP -astj: SESSION_ID is empty >> "%LOG_FILE%"
)

echo.
echo Executing: !MAIN_CMD_DISPLAY!
echo.
echo [%DATE% %TIME%] Launching: "!MAIN_EXE!" !MAIN_ARGS! >> "%LOG_FILE%"
"!MAIN_EXE!" !MAIN_ARGS!
set "EXIT_CODE=!ERRORLEVEL!"
echo [%DATE% %TIME%] Main command exit code: !EXIT_CODE! >> "%LOG_FILE%"

if not "!EXIT_CODE!"=="0" (
  echo.
  echo Command failed with exit code !EXIT_CODE!.
  echo.
  if exist "%OUTPUT_FILE%" (
    echo ==== CRASH REPORT ====
    type "%OUTPUT_FILE%"
    echo ======================
    echo [%DATE% %TIME%] Crash report found: %OUTPUT_FILE% >> "%LOG_FILE%"
  ) else (
    echo WARNING: No crash report found.
    echo Expected: %OUTPUT_FILE%
    echo [%DATE% %TIME%] WARNING: No crash report at %OUTPUT_FILE% >> "%LOG_FILE%"
  )
  if not "!SESSION_ID!"=="" (
    if exist "%VORTEX_UTILS%" (
      echo [%DATE% %TIME%] CALL: "%VORTEX_UTILS%" -wses --session_id=!SESSION_ID! --state=failed >> "%LOG_FILE%"
      "%VORTEX_UTILS%" -wses --session_id=!SESSION_ID! --state=failed
      set "VU_EXIT=!ERRORLEVEL!"
      echo [%DATE% %TIME%] vortex_utils -wses exit code: !VU_EXIT! >> "%LOG_FILE%"
    ) else (
      echo [%DATE% %TIME%] WARNING: vortex_utils.exe not found >> "%LOG_FILE%"
    )
  )
  if not "!END_EXE!"=="" (
    echo.
    echo Launching crash handler UI...
    echo.
    echo [%DATE% %TIME%] Launching end command: "!END_EXE!" !END_ARGS! >> "%LOG_FILE%"
    "!END_EXE!" !END_ARGS!
    echo [%DATE% %TIME%] End command exit code: !ERRORLEVEL! >> "%LOG_FILE%"
  )
) else (
  echo.
  echo Command executed successfully.
  if not "!SESSION_ID!"=="" (
    if exist "%VORTEX_UTILS%" (
      echo [%DATE% %TIME%] CALL: "%VORTEX_UTILS%" -wses --session_id=!SESSION_ID! --state=finished >> "%LOG_FILE%"
      "%VORTEX_UTILS%" -wses --session_id=!SESSION_ID! --state=finished
      set "VU_EXIT=!ERRORLEVEL!"
      echo [%DATE% %TIME%] vortex_utils -wses exit code: !VU_EXIT! >> "%LOG_FILE%"
    ) else (
      echo [%DATE% %TIME%] WARNING: vortex_utils.exe not found >> "%LOG_FILE%"
    )
  )
)

echo [%DATE% %TIME%] === handle_crash ended, exit code: !EXIT_CODE! === >> "%LOG_FILE%"
exit /b !EXIT_CODE!
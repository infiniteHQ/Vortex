@echo off
setlocal

if "%~1"=="" (
    echo Usage: %~nx0 ^<path_to_output_file^> ^<command_to_execute...^> [^<end_command^>]
    exit /b 1
)

set "OUTPUT_FILE=%~1"
shift

set "END_COMMAND="

:build_command
if "%~1"=="" goto done_args
if "%~1"=="::END" (
    shift
    set END_COMMAND=%*
    goto done_args
)
if defined COMMAND (
    set COMMAND=%COMMAND% %1
) else (
    set COMMAND=%1
)
shift
goto build_command

:done_args
echo Executing: %COMMAND%
%COMMAND%
set "EXIT_CODE=%ERRORLEVEL%"

if not "%EXIT_CODE%"=="0" (
    echo Command failed with exit code %EXIT_CODE%. Checking crash dumps...

    if exist "%SystemRoot%\MEMORY.DMP" (
        echo Crash dump file found: %SystemRoot%\MEMORY.DMP
        echo ==== DUMP ANALYSIS BEGIN ==== >> "%OUTPUT_FILE%"
        dumpchk "%SystemRoot%\MEMORY.DMP" >> "%OUTPUT_FILE%" 2>&1
        echo ==== DUMP ANALYSIS END ==== >> "%OUTPUT_FILE%"
        del /F /Q "%SystemRoot%\MEMORY.DMP"
        echo Crash dump %SystemRoot%\MEMORY.DMP has been removed.
    ) else (
        echo No crash dump file found.
    )
) else (
    echo Command executed successfully.
)

if defined END_COMMAND (
    echo Executing end command: %END_COMMAND%
    %END_COMMAND%
)

exit /b %EXIT_CODE%

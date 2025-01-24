@echo off

if "%~1"=="" (
    echo Usage: %~nx0 ^<path_to_output_file^> ^<command_to_execute^> ^<end_command^>
    exit /b 1
)

set "OUTPUT_FILE=%~1"
shift

set "COMMAND="
:build_command
if "%~1"=="" goto check_end_command
set "COMMAND=%COMMAND% \"%~1\""
shift
goto build_command

:check_end_command
set "END_COMMAND=%~1"

:execute_command
echo Executing: %COMMAND%
%COMMAND%
set "EXIT_CODE=%ERRORLEVEL%"

if "%EXIT_CODE%" NEQ "0" (
    echo Command failed with exit code %EXIT_CODE%. Checking crash dumps...

    for %%F in ("%SystemRoot%\MEMORY.DMP") do (
        if exist "%%F" (
            echo Crash dump file found: %%F
            echo ==== DUMP ANALYSIS BEGIN ==== >> "%OUTPUT_FILE%"
            dumpchk "%%F" >> "%OUTPUT_FILE%" 2>&1
            echo ==== DUMP ANALYSIS END ==== >> "%OUTPUT_FILE%"
            del /F /Q "%%F"
            echo Crash dump %%F has been removed.
        ) else (
            echo No crash dump file found.
        )
    )
) else (
    echo Command executed successfully.
)

:execute_end_command
if not "%END_COMMAND%"=="" (
    echo Executing end command: %END_COMMAND%
    %END_COMMAND%
)

exit /b %EXIT_CODE%

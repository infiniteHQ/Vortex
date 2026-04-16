@echo off
setlocal EnableExtensions

if "%~1"=="" exit /b 1

set "OUTPUT_FILE=%~1"
shift

set "MAIN_CMD=%~1"
shift

if "%~1"=="__END__" shift

set "END_CMD=%~1"
shift

:run_main
echo Executing: %MAIN_CMD%
call %MAIN_CMD%
set "EXIT_CODE=%ERRORLEVEL%"

if not "%EXIT_CODE%"=="0" (
    echo Command failed with exit code %EXIT_CODE%.

    if exist "%SystemRoot%\MEMORY.DMP" (
        echo ==== DUMP ANALYSIS BEGIN ==== >> "%OUTPUT_FILE%"
        dumpchk "%SystemRoot%\MEMORY.DMP" >> "%OUTPUT_FILE%" 2>&1
        echo ==== DUMP ANALYSIS END ==== >> "%OUTPUT_FILE%"
        del /F /Q "%SystemRoot%\MEMORY.DMP"
    ) else (
        echo No crash dump file found.
    )

    if not "%END_CMD%"=="" (
        echo Executing end command: %END_CMD%
        call %END_CMD%
    )
) else (
    echo Command executed successfully.
)

exit /b %EXIT_CODE%
@echo off
setlocal EnableExtensions EnableDelayedExpansion

if "%~1"=="" exit /b 1

set "OUTPUT_FILE=%~1"
shift

set "MAIN_CMD=%~1"
shift

if "%~1"=="__END__" shift

set "END_CMD=%~1"
shift

echo.
echo Executing: %MAIN_CMD%
echo.

call %MAIN_CMD%
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

    if not "%END_CMD%"=="" (
        echo.
        echo Launching crash handler UI...
        echo.

        call %END_CMD%
    )

) else (
    echo.
    echo Command executed successfully.
)

exit /b %EXIT_CODE%
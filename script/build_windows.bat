@echo off
:: Windows Build Script - Using MSVC Compiler

:: Initialize Pro version flag
SET PRO_VERSION=OFF

:: Parse command line arguments
FOR %%a IN (%*) DO (
    IF "%%a"=="--pro" (
        SET PRO_VERSION=ON
        echo Building Professional Version
    )
)

echo ===== Starting Windows Build =====

:: Set script variables
SET SCRIPT_DIR=%~dp0
SET PROJECT_DIR=%SCRIPT_DIR%..\
SET BUILD_DIR=%PROJECT_DIR%build\windows
SET INSTALL_DIR=%PROJECT_DIR%output

:: Clean install directory
echo Cleaning install directory...
if exist "%INSTALL_DIR%" rd /s /q "%INSTALL_DIR%"

:: Create build directory
if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
    if errorlevel 1 (
        echo Error: Cannot create build directory
        exit /b 1
    )
)

:: Configure project - Release mode
echo Configuring Windows project...
cmake -B "%BUILD_DIR%" -S "%PROJECT_DIR%" ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DGPUPIXEL_BUILD_DESKTOP_DEMO=ON ^
  -DGPUPIXEL_PRO_VERSION=%PRO_VERSION% ^
  -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%"
if errorlevel 1 (
    echo Error: Project configuration failed
    exit /b 2
)

:: Build project - Using multi-threaded compilation
echo Building Windows project...
cmake --build "%BUILD_DIR%" --config Release --parallel %NUMBER_OF_PROCESSORS%
if errorlevel 1 (
    echo Error: Project build failed
    exit /b 3
)

:: Install to output directory
echo Installing to output directory...
cmake --install "%BUILD_DIR%" --config Release
if errorlevel 1 (
    echo Error: Project installation failed
    exit /b 4
)

echo ===== Windows Build Complete, Installation Directory: %INSTALL_DIR% =====
exit /b 0 
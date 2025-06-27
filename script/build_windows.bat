@echo off
:: Windows Build Script - Using MSVC Compiler

echo ===== Starting Windows Build =====

:: Set script variables
SET BUILD_DIR=build\windows
SET INSTALL_DIR=output

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
cmake -B "%BUILD_DIR%" -S . ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DGPUPIXEL_BUILD_DESKTOP_DEMO=ON ^
  -DGPUPIXEL_ENABLE_FACE_DETECTOR=OFF ^
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
@echo off
:: Windows Build Script - Using MSVC Compiler

echo ===== Starting Windows Build =====

:: Create build directory
if not exist build (
    mkdir build
    if errorlevel 1 (
        echo Error: Cannot create build directory
        exit /b 1
    )
)

:: Configure project - Release mode
echo Configuring Windows project...
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DGPUPIXEL_BUILD_DESKTOP_DEMO=ON -DCMAKE_INSTALL_PREFIX=output
if errorlevel 1 (
    echo Error: Project configuration failed
    exit /b 2
)

:: Build project - Using multi-threaded compilation
echo Building Windows project...
cmake --build build --config Release --parallel %NUMBER_OF_PROCESSORS%
if errorlevel 1 (
    echo Error: Project build failed
    exit /b 3
)

:: Install to output directory
echo Installing to output directory...
cmake --install build --config Release
if errorlevel 1 (
    echo Error: Project installation failed
    exit /b 4
)

echo ===== Windows Build Complete, Installation Directory: output =====
exit /b 0 
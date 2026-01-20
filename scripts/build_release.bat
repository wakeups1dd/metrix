@echo off
setlocal

echo [Build] Starting Release build...

if not exist build mkdir build

echo [Build] Configuring...
cmake -S . -B build -A x64
if %ERRORLEVEL% NEQ 0 (
    echo [Build] Configuration failed!
    exit /b %ERRORLEVEL%
)

echo [Build] Building...
cmake --build build --config Release
if %ERRORLEVEL% NEQ 0 (
    echo [Build] Build failed!
    exit /b %ERRORLEVEL%
)

echo [Build] Success! Output is in build\bin\Release\openmeters.exe
endlocal

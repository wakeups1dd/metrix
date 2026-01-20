@echo off
setlocal

set METRIX_ROOT=%~dp0..
set THIRD_PARTY=%METRIX_ROOT%\third_party
set INC_DIR=%THIRD_PARTY%\include
set JSON_DIR=%INC_DIR%\nlohmann

if not exist "%THIRD_PARTY%" mkdir "%THIRD_PARTY%"
if not exist "%INC_DIR%" mkdir "%INC_DIR%"
if not exist "%JSON_DIR%" mkdir "%JSON_DIR%"

echo [Dependency Setup] Starting...

:: 1. ImGui (Docking Branch)
:: 1. ImGui (Docking Branch)
echo [ImGui] checking submodule...
git submodule update --init --recursive third_party/imgui

if not exist "%THIRD_PARTY%\imgui\imgui.h" (
    echo [ERROR] ImGui submodule failed to update.
    exit /b 1
)

:: 2. nlohmann/json
if exist "%JSON_DIR%\json.hpp" (
    echo [JSON] Already exists.
) else (
    echo [JSON] Downloading nlohmann/json.hpp...
    powershell -Command "Invoke-WebRequest -Uri 'https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp' -OutFile '%JSON_DIR%\json.hpp'"
)

:: 3. Catch2 (v2.x)
if exist "%THIRD_PARTY%\catch2\catch.hpp" (
    echo [Catch2] Already exists.
) else (
    echo [Catch2] Downloading catch.hpp...
    if not exist "%THIRD_PARTY%\catch2" mkdir "%THIRD_PARTY%\catch2"
    powershell -Command "Invoke-WebRequest -Uri 'https://github.com/catchorg/Catch2/releases/download/v2.13.9/catch.hpp' -OutFile '%THIRD_PARTY%\catch2\catch.hpp'"
)

echo [Dependency Setup] Complete.
endlocal

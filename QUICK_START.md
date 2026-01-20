# Quick Start Guide

## Prerequisites

1. **Windows 10 or later**
2. **Visual Studio 2019+** (or Build Tools)
3. **CMake 3.20+**
4. **Windows SDK**
5. **ImGui** (for GUI - see below)

## Setup ImGui

1. Download ImGui from: https://github.com/ocornut/imgui/releases
2. Extract to: `third_party/imgui/`
3. Ensure these files exist:
   - `third_party/imgui/imgui.h`
   - `third_party/imgui/imgui.cpp`
   - `third_party/imgui/backends/imgui_impl_win32.cpp`
   - `third_party/imgui/backends/imgui_impl_dx11.cpp`

## Building

### Standard Build (with GUI):
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

### Console-Only Build (no GUI):
```bash
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
# GUI will be disabled if ImGui not found
```

### With Tests:
```bash
cmake .. -DBUILD_TESTS=ON
cmake --build . --config Release
ctest
```

## Running

### GUI Mode:
```bash
build\bin\Release\openmeters.exe
```

### Console Mode:
If GUI is not available, the application falls back to console mode automatically.

## Configuration

Configuration is saved to: `%APPDATA%\OpenMeters\config.json`

Settings include:
- Window position and size
- Always-on-top toggle
- Meter visibility (Peak/RMS)
- UI theme (dark/light)
- UI scale

## Logs

Logs are written to: `logs\openmeters.log`

Log levels can be adjusted in code (default: Info).

## Testing

Run unit tests:
```bash
cd build
ctest -C Release
```

Or run test executable directly:
```bash
build\bin\Release\test_meters.exe
```

## Building Installer

1. Install NSIS: https://nsis.sourceforge.io/
2. Build installer:
```bash
makensis installer\openmeters.nsi
```

The installer will be created as `OpenMeters-Setup.exe`.

## Troubleshooting

### "ImGui not found"
- Download ImGui and place in `third_party/imgui/`
- Or build without GUI (console mode)

### "Failed to initialize audio engine"
- Check audio device is available
- Run as administrator (may be required for loopback)
- Check Windows audio settings

### "Failed to initialize window"
- Ensure DirectX11 is available
- Check graphics drivers are up to date

## Next Steps

- See `BETA_TESTING_GUIDE.md` for beta testing process
- See `IMPLEMENTATION_SUMMARY.md` for implementation details
- See `COMMERCIAL_READINESS_ASSESSMENT.md` for production readiness status


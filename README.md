# OpenMeters

An open-source, lightweight, always-on-top Windows audio metering overlay inspired by professional tools like MiniMeters.

## Features

- System audio metering via WASAPI loopback
- Peak and RMS meters
- Real-time audio visualization
- Extremely low CPU usage
- Expandable architecture for future features (LUFS, FFT, stereo balance)

## Architecture

The project follows a strict layered architecture:

- **Core Audio Engine** (`/core/audio`) - WASAPI capture and audio processing
- **Metering & DSP** (`/core/meters`) - Peak, RMS, and future LUFS/FFT implementations
- **UI Layer** (`/ui`) - ImGui-based overlay (coming soon)
- **Application Layer** (`/app`) - Entry point and lifecycle management
- **Common** (`/common`) - Shared types and utilities

## Building

### Requirements

- Windows 10 or later
- CMake 3.20 or later
- Visual Studio 2019 or later (with C++20 support)
- Windows SDK

### Build Steps

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

The executable will be in `build/bin/Release/openmeters.exe`.

## Current Status

✅ WASAPI loopback capture  
✅ Peak meter  
✅ RMS meter  
✅ Audio engine with callback interface  
⏳ UI overlay (planned)  
⏳ LUFS metering (planned)  
⏳ FFT spectrum analysis (planned)  

## License

Apache License 2.0


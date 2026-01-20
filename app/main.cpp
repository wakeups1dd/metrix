#ifdef _WIN32

// Use GUI version if available, otherwise fall back to console
#ifdef BUILD_GUI
#include "main-gui.cpp"
#else

#include "../core/audio/audio-engine.h"
#include "../common/meter-values.h"
#include "../common/logger.h"
#include "../common/config.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

using namespace openmeters;

/**
 * Simple console callback for testing audio capture.
 * Prints peak and RMS values to console.
 */
class ConsoleCallback : public core::audio::IAudioDataCallback {
public:
    void onAudioData(
        const float* buffer,
        std::size_t frameCount,
        const common::AudioFormat& format
    ) override {
        // Silently consume audio data (we only care about meters)
        (void)buffer;
        (void)frameCount;
        (void)format;
    }
    
    void onMeterData(const common::MeterSnapshot& snapshot) override {
        // Print meter values
        std::cout << "\rPeak L: " << std::fixed << std::setprecision(3) << snapshot.peak.left
                  << " R: " << snapshot.peak.right
                  << " | RMS L: " << snapshot.rms.left
                  << " R: " << snapshot.rms.right
                  << "    " << std::flush;
    }
};

int main() {
    // Initialize logger
    std::string logPath = "logs/openmeters.log";
    common::Logger::initialize(logPath, common::LogLevel::Info, true);
    
    LOG_INFO("OpenMeters starting (console mode)...");
    
    // Load configuration
    common::ConfigManager::load();
    
    std::cout << "OpenMeters - Audio Metering Test\n";
    std::cout << "================================\n\n";
    
    // Create audio engine
    core::audio::AudioEngine engine;
    
    // Initialize
    std::cout << "Initializing audio engine...\n";
    if (!engine.initialize()) {
        LOG_ERROR("Failed to initialize audio engine");
        std::cerr << "Failed to initialize audio engine.\n";
        common::Logger::shutdown();
        return 1;
    }
    
    LOG_INFO("Audio engine initialized");
    
    // Print format info
    const auto format = engine.getFormat();
    std::cout << "Audio format: " << static_cast<int>(format.sampleRate) << " Hz, "
              << static_cast<int>(format.channelCount) << " channel(s)\n\n";
    
    // Register callback
    ConsoleCallback callback;
    engine.registerCallback(&callback);
    
    // Start capture
    std::cout << "Starting audio capture...\n";
    if (!engine.start()) {
        LOG_ERROR("Failed to start audio capture");
        std::cerr << "Failed to start audio capture.\n";
        engine.shutdown();
        common::Logger::shutdown();
        return 1;
    }
    
    LOG_INFO("Audio capture started");
    
    std::cout << "Capturing audio. Press Enter to stop...\n\n";
    
    // Run for a bit (or until user presses Enter)
    std::this_thread::sleep_for(std::chrono::seconds(10));
    
    // Stop
    std::cout << "\n\nStopping audio capture...\n";
    engine.stop();
    
    // Unregister callback
    engine.unregisterCallback(&callback);
    
    // Shutdown
    LOG_INFO("Shutting down...");
    engine.shutdown();
    
    // Save configuration
    common::ConfigManager::save();
    
    std::cout << "Shutdown complete.\n";
    common::Logger::shutdown();
    return 0;
}

#endif // BUILD_GUI

#else
// Non-Windows platforms: This application is Windows-only
#error "OpenMeters is Windows-only. This application requires Windows and WASAPI."
#endif // _WIN32


#ifdef _WIN32
#ifdef BUILD_GUI

#include "../ui/window.h"
#include "../core/audio/audio-engine.h"
#include "../common/logger.h"
#include "../common/config.h"
#include <windows.h>

using namespace openmeters;

/**
 * GUI callback for audio data.
 * Updates the window with meter values.
 */
class GuiCallback : public core::audio::IAudioDataCallback {
public:
    explicit GuiCallback(ui::Window* window) : m_window(window) {}
    
    void onAudioData(
        const float* buffer,
        std::size_t frameCount,
        const common::AudioFormat& format
    ) override {
        // Silently consume audio data
        (void)buffer;
        (void)frameCount;
        (void)format;
    }
    
    void onMeterData(const common::MeterSnapshot& snapshot) override {
        if (m_window) {
            m_window->updateMeters(snapshot);
        }
    }
    
private:
    ui::Window* m_window;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    (void)hPrevInstance;
    (void)lpCmdLine;
    
    // Initialize logger
    std::string logPath = "logs/openmeters.log";
    if (!common::Logger::initialize(logPath, common::LogLevel::Info, true)) {
        MessageBoxA(nullptr, "Failed to initialize logger", "OpenMeters Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    LOG_INFO("OpenMeters starting...");
    
    // Load configuration
    common::ConfigManager::load();
    
    // Create window
    ui::Window window;
    if (!window.initialize(hInstance, nCmdShow)) {
        LOG_ERROR("Failed to initialize window");
        MessageBoxA(nullptr, "Failed to initialize window", "OpenMeters Error", MB_OK | MB_ICONERROR);
        common::Logger::shutdown();
        return 1;
    }
    
    // Create audio engine
    core::audio::AudioEngine engine;
    if (!engine.initialize()) {
        LOG_ERROR("Failed to initialize audio engine");
        MessageBoxA(nullptr, "Failed to initialize audio engine. Check audio device.", 
                   "OpenMeters Error", MB_OK | MB_ICONERROR);
        window.shutdown();
        common::Logger::shutdown();
        return 1;
    }
    
    LOG_INFO("Audio format: " + std::to_string(engine.getFormat().sampleRate) + " Hz, " +
             std::to_string(engine.getFormat().channelCount) + " channel(s)");
    
    // Register callback
    GuiCallback callback(&window);
    engine.registerCallback(&callback);
    
    // Start capture
    if (!engine.start()) {
        LOG_ERROR("Failed to start audio capture");
        MessageBoxA(nullptr, "Failed to start audio capture", "OpenMeters Error", MB_OK | MB_ICONERROR);
        engine.shutdown();
        window.shutdown();
        common::Logger::shutdown();
        return 1;
    }
    
    LOG_INFO("Audio capture started");
    
    // Run main loop
    window.run();
    
    // Cleanup
    LOG_INFO("Shutting down...");
    engine.stop();
    engine.unregisterCallback(&callback);
    engine.shutdown();
    window.shutdown();
    
    // Save configuration
    common::ConfigManager::save();
    
    common::Logger::shutdown();
    return 0;
}

#endif // BUILD_GUI
#endif // _WIN32


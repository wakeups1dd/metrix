#pragma once

#include "types.h"
#include <string>
#include <memory>

namespace openmeters::common {

/**
 * Application configuration settings.
 */
struct AppConfig {
    // Window settings
    float windowX = 100.0f;
    float windowY = 100.0f;
    float windowWidth = 400.0f;
    float windowHeight = 200.0f;
    bool alwaysOnTop = true;
    bool showWindow = true;
    
    // Meter settings
    float meterUpdateRate = 60.0f; // Updates per second
    bool showPeakMeter = true;
    bool showRmsMeter = true;
    float meterDecayRate = 0.95f; // Peak hold decay
    
    // Audio settings
    bool autoStartCapture = false;
    float audioBufferSize = 0.1f; // seconds
    
    // UI settings
    float uiScale = 1.0f;
    bool darkMode = true;
    
    /**
     * Load configuration from file.
     * 
     * @param configPath Path to config file (JSON)
     * @return true if loaded successfully, false otherwise
     */
    bool loadFromFile(const std::string& configPath);
    
    /**
     * Save configuration to file.
     * 
     * @param configPath Path to config file (JSON)
     * @return true if saved successfully, false otherwise
     */
    bool saveToFile(const std::string& configPath) const;
    
    /**
     * Get default config file path.
     * Returns: %APPDATA%/OpenMeters/config.json on Windows
     */
    static std::string getDefaultConfigPath();
};

/**
 * Configuration manager.
 * Singleton pattern for global configuration access.
 */
class ConfigManager {
public:
    /**
     * Get the global configuration instance.
     */
    static AppConfig& get();
    
    /**
     * Load configuration from default location.
     */
    static bool load();
    
    /**
     * Save configuration to default location.
     */
    static bool save();
    
    /**
     * Reset to default configuration.
     */
    static void reset();

private:
    ConfigManager() = default;
    static AppConfig s_config;
};

} // namespace openmeters::common


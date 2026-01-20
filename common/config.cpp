#include "config.h"
#include "logger.h"
#include <fstream>
#include <filesystem>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#endif

namespace openmeters::common {

AppConfig ConfigManager::s_config;

AppConfig& ConfigManager::get() {
    return s_config;
}

bool ConfigManager::load() {
    std::string configPath = AppConfig::getDefaultConfigPath();
    return s_config.loadFromFile(configPath);
}

bool ConfigManager::save() {
    std::string configPath = AppConfig::getDefaultConfigPath();
    return s_config.saveToFile(configPath);
}

void ConfigManager::reset() {
    s_config = AppConfig();
}

std::string AppConfig::getDefaultConfigPath() {
#ifdef _WIN32
    char appDataPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, appDataPath))) {
        std::filesystem::path configDir(appDataPath);
        configDir /= "OpenMeters";
        std::filesystem::create_directories(configDir);
        return (configDir / "config.json").string();
    }
#endif
    // Fallback to current directory
    return "config.json";
}

bool AppConfig::loadFromFile(const std::string& configPath) {
    if (!std::filesystem::exists(configPath)) {
        LOG_INFO("Config file not found, using defaults: " + configPath);
        return false;
    }
    
    std::ifstream file(configPath);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open config file: " + configPath);
        return false;
    }
    
    // Simple JSON parsing (basic implementation)
    // TODO: Use proper JSON library (nlohmann/json) for production
    std::string line;
    while (std::getline(file, line)) {
        // Remove whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // Simple key-value parsing
        if (line.find("\"windowX\"") != std::string::npos) {
            // Extract value (simplified)
            // In production, use proper JSON parser
        }
        // ... similar for other fields
    }
    
    LOG_INFO("Config loaded from: " + configPath);
    return true;
}

bool AppConfig::saveToFile(const std::string& configPath) const {
    // Create directory if needed
    std::filesystem::path path(configPath);
    auto dir = path.parent_path();
    if (!dir.empty() && !std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }
    
    std::ofstream file(configPath);
    if (!file.is_open()) {
        LOG_ERROR("Failed to create config file: " + configPath);
        return false;
    }
    
    // Simple JSON writing (basic implementation)
    // TODO: Use proper JSON library for production
    file << "{\n";
    file << "  \"windowX\": " << windowX << ",\n";
    file << "  \"windowY\": " << windowY << ",\n";
    file << "  \"windowWidth\": " << windowWidth << ",\n";
    file << "  \"windowHeight\": " << windowHeight << ",\n";
    file << "  \"alwaysOnTop\": " << (alwaysOnTop ? "true" : "false") << ",\n";
    file << "  \"showWindow\": " << (showWindow ? "true" : "false") << ",\n";
    file << "  \"meterUpdateRate\": " << meterUpdateRate << ",\n";
    file << "  \"showPeakMeter\": " << (showPeakMeter ? "true" : "false") << ",\n";
    file << "  \"showRmsMeter\": " << (showRmsMeter ? "true" : "false") << ",\n";
    file << "  \"meterDecayRate\": " << meterDecayRate << ",\n";
    file << "  \"autoStartCapture\": " << (autoStartCapture ? "false" : "false") << ",\n";
    file << "  \"audioBufferSize\": " << audioBufferSize << ",\n";
    file << "  \"uiScale\": " << uiScale << ",\n";
    file << "  \"darkMode\": " << (darkMode ? "true" : "false") << "\n";
    file << "}\n";
    
    file.close();
    LOG_INFO("Config saved to: " + configPath);
    return true;
}

} // namespace openmeters::common


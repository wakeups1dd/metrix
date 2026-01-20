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
    
    try {
        nlohmann::json j;
        file >> j;
        
        // Window settings
        if (j.contains("windowX")) windowX = j["windowX"];
        if (j.contains("windowY")) windowY = j["windowY"];
        if (j.contains("windowWidth")) windowWidth = j["windowWidth"];
        if (j.contains("windowHeight")) windowHeight = j["windowHeight"];
        if (j.contains("alwaysOnTop")) alwaysOnTop = j["alwaysOnTop"];
        if (j.contains("showWindow")) showWindow = j["showWindow"];
        
        // Meter settings
        if (j.contains("meterUpdateRate")) meterUpdateRate = j["meterUpdateRate"];
        if (j.contains("showPeakMeter")) showPeakMeter = j["showPeakMeter"];
        if (j.contains("showRmsMeter")) showRmsMeter = j["showRmsMeter"];
        if (j.contains("meterDecayRate")) meterDecayRate = j["meterDecayRate"];
        
        // Audio settings
        if (j.contains("autoStartCapture")) autoStartCapture = j["autoStartCapture"];
        if (j.contains("audioBufferSize")) audioBufferSize = j["audioBufferSize"];
        
        // UI settings
        if (j.contains("uiScale")) uiScale = j["uiScale"];
        if (j.contains("darkMode")) darkMode = j["darkMode"];
        
        LOG_INFO("Config loaded from: " + configPath);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to parse config file: " + std::string(e.what()));
        return false;
    }
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
    
    try {
        nlohmann::json j;
        
        // Window settings
        j["windowX"] = windowX;
        j["windowY"] = windowY;
        j["windowWidth"] = windowWidth;
        j["windowHeight"] = windowHeight;
        j["alwaysOnTop"] = alwaysOnTop;
        j["showWindow"] = showWindow;
        
        // Meter settings
        j["meterUpdateRate"] = meterUpdateRate;
        j["showPeakMeter"] = showPeakMeter;
        j["showRmsMeter"] = showRmsMeter;
        j["meterDecayRate"] = meterDecayRate;
        
        // Audio settings
        j["autoStartCapture"] = autoStartCapture;
        j["audioBufferSize"] = audioBufferSize;
        
        // UI settings
        j["uiScale"] = uiScale;
        j["darkMode"] = darkMode;
        
        file << j.dump(4); // Pretty print with 4 spaces
        file.close();
        
        LOG_INFO("Config saved to: " + configPath);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to save config file: " + std::string(e.what()));
        return false;
    }
}

} // namespace openmeters::common


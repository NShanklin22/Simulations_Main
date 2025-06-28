#include "core/ConfigManager.h"
#include <fstream>
#include <iostream>

ConfigManager::ConfigManager() {
    // Initialize with default settings
}

ConfigManager::~ConfigManager() {
    // Cleanup
}

bool ConfigManager::loadConfig(const std::string& filename) {
    // Basic file loading - will implement JSON parsing later
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open config file: " << filename << std::endl;
        return false;
    }
    
    // For now, just return success
    file.close();
    return true;
}

bool ConfigManager::saveConfig(const std::string& filename) {
    // Basic file saving - will implement JSON writing later
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not save config file: " << filename << std::endl;
        return false;
    }
    
    file.close();
    return true;
}

std::string ConfigManager::getString(const std::string& key, const std::string& defaultValue) const {
    auto it = m_settings.find(key);
    return (it != m_settings.end()) ? it->second : defaultValue;
}

int ConfigManager::getInt(const std::string& key, int defaultValue) const {
    auto it = m_settings.find(key);
    if (it != m_settings.end()) {
        try {
            return std::stoi(it->second);
        } catch (const std::exception&) {
            return defaultValue;
        }
    }
    return defaultValue;
}

float ConfigManager::getFloat(const std::string& key, float defaultValue) const {
    auto it = m_settings.find(key);
    if (it != m_settings.end()) {
        try {
            return std::stof(it->second);
        } catch (const std::exception&) {
            return defaultValue;
        }
    }
    return defaultValue;
}

bool ConfigManager::getBool(const std::string& key, bool defaultValue) const {
    auto it = m_settings.find(key);
    if (it != m_settings.end()) {
        return (it->second == "true" || it->second == "1");
    }
    return defaultValue;
}
#include "core/ConfigManager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>

ConfigManager::ConfigManager() {
    // Initialize with default settings (already set in struct defaults)
}

ConfigManager::~ConfigManager() {
    // Cleanup
}

bool ConfigManager::loadConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open config file: " << filename << ", using defaults" << std::endl;
        return false;
    }
    
    // Read entire file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonContent = buffer.str();
    file.close();
    
    // Parse JSON content
    parseJsonConfig(jsonContent);
    
    std::cout << "Configuration loaded from " << filename << std::endl;
    return true;
}

bool ConfigManager::saveConfig(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not save config file: " << filename << std::endl;
        return false;
    }
    
    // Write JSON configuration (basic implementation)
    file << "{\n";
    file << "    \"display\": {\n";
    file << "        \"window_width\": " << m_displaySettings.windowWidth << ",\n";
    file << "        \"window_height\": " << m_displaySettings.windowHeight << ",\n";
    file << "        \"fullscreen\": " << (m_displaySettings.fullscreen ? "true" : "false") << ",\n";
    file << "        \"vsync\": " << (m_displaySettings.vsync ? "true" : "false") << ",\n";
    file << "        \"fps_limit\": " << m_displaySettings.fpsLimit << "\n";
    file << "    },\n";
    file << "    \"simulation\": {\n";
    file << "        \"default_speed\": " << m_simulationSettings.defaultSpeed << ",\n";
    file << "        \"default_array_size\": " << m_simulationSettings.defaultArraySize << ",\n";
    file << "        \"min_array_size\": " << m_simulationSettings.minArraySize << ",\n";
    file << "        \"max_array_size\": " << m_simulationSettings.maxArraySize << "\n";
    file << "    }\n";
    file << "}\n";
    
    file.close();
    std::cout << "Configuration saved to " << filename << std::endl;
    return true;
}

void ConfigManager::parseJsonConfig(const std::string& jsonContent) {
    // Simple regex-based JSON parsing (for basic configuration)
    // This is a simplified implementation - in production you'd use a proper JSON library
    
    std::regex numberPattern(R"prog("([^"]+)"\s*:\s*(\d+))prog");
    std::regex boolPattern(R"prog("([^"]+)"\s*:\s*(true|false))prog");
    std::regex stringPattern(R"prog("([^"]+)"\s*:\s*"([^"]+)")prog");
    
    std::smatch match;
    
    // Parse numbers
    auto searchStart = jsonContent.cbegin();
    while (std::regex_search(searchStart, jsonContent.cend(), match, numberPattern)) {
        std::string key = match[1].str();
        int value = std::stoi(match[2].str());
        
        // Map to structured settings
        if (key == "window_width") m_displaySettings.windowWidth = value;
        else if (key == "window_height") m_displaySettings.windowHeight = value;
        else if (key == "fps_limit") m_displaySettings.fpsLimit = value;
        else if (key == "default_speed") m_simulationSettings.defaultSpeed = value;
        else if (key == "default_array_size") m_simulationSettings.defaultArraySize = value;
        else if (key == "min_array_size") m_simulationSettings.minArraySize = value;
        else if (key == "max_array_size") m_simulationSettings.maxArraySize = value;
        
        // Also store in legacy map
        m_settings[key] = match[2].str();
        
        searchStart = match.suffix().first;
    }
    
    // Parse booleans
    searchStart = jsonContent.cbegin();
    while (std::regex_search(searchStart, jsonContent.cend(), match, boolPattern)) {
        std::string key = match[1].str();
        bool value = (match[2].str() == "true");
        
        // Map to structured settings
        if (key == "fullscreen") m_displaySettings.fullscreen = value;
        else if (key == "vsync") m_displaySettings.vsync = value;
        else if (key == "enabled") m_audioSettings.enabled = value;
        
        // Also store in legacy map
        m_settings[key] = match[2].str();
        
        searchStart = match.suffix().first;
    }
    
    // Parse strings
    searchStart = jsonContent.cbegin();
    while (std::regex_search(searchStart, jsonContent.cend(), match, stringPattern)) {
        std::string key = match[1].str();
        std::string value = match[2].str();
        
        // Map to structured settings
        if (key == "primary_color") m_themeSettings.primaryColor = parseColor(value);
        else if (key == "secondary_color") m_themeSettings.secondaryColor = parseColor(value);
        else if (key == "background_color") m_themeSettings.backgroundColor = parseColor(value);
        else if (key == "inactive_color") m_themeSettings.inactiveColor = parseColor(value);
        else if (key == "pause_play_key") m_controlSettings.pausePlayKey = value;
        else if (key == "step_key") m_controlSettings.stepKey = value;
        else if (key == "reset_key") m_controlSettings.resetKey = value;
        else if (key == "menu_key") m_controlSettings.menuKey = value;
        
        // Also store in legacy map
        m_settings[key] = value;
        
        searchStart = match.suffix().first;
    }
    
    // Parse floats (for audio settings)
    std::regex floatPattern(R"prog("([^"]+)"\s*:\s*(\d+\.\d+))prog");
    searchStart = jsonContent.cbegin();
    while (std::regex_search(searchStart, jsonContent.cend(), match, floatPattern)) {
        std::string key = match[1].str();
        float value = std::stof(match[2].str());
        
        if (key == "master_volume") m_audioSettings.masterVolume = value;
        else if (key == "sfx_volume") m_audioSettings.sfxVolume = value;
        
        m_settings[key] = match[2].str();
        
        searchStart = match.suffix().first;
    }
}

sf::Color ConfigManager::parseColor(const std::string& hexColor) const {
    if (hexColor.empty() || hexColor[0] != '#' || hexColor.length() != 7) {
        return sf::Color::White; // Default fallback
    }
    
    std::string hex = hexColor.substr(1); // Remove #
    unsigned int color;
    std::stringstream ss;
    ss << std::hex << hex;
    ss >> color;
    
    return sf::Color(
        (color >> 16) & 0xFF,  // Red
        (color >> 8) & 0xFF,   // Green
        color & 0xFF           // Blue
    );
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
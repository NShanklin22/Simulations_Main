#pragma once
#include <string>
#include <map>

class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();
    
    bool loadConfig(const std::string& filename);
    bool saveConfig(const std::string& filename);
    
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    float getFloat(const std::string& key, float defaultValue = 0.0f) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    
private:
    std::map<std::string, std::string> m_settings;
};
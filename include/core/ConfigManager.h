#pragma once
#include <string>
#include <map>
#include <SFML/Graphics.hpp>

struct DisplaySettings {
    int windowWidth = 1200;
    int windowHeight = 800;
    bool fullscreen = false;
    bool vsync = true;
    int fpsLimit = 60;
};

struct SimulationSettings {
    int defaultSpeed = 500;
    int defaultArraySize = 50;
    int minArraySize = 10;
    int maxArraySize = 100;
};

struct ThemeSettings {
    sf::Color primaryColor = sf::Color(0, 255, 65);      // #00FF41
    sf::Color secondaryColor = sf::Color(255, 165, 0);   // #FFA500
    sf::Color backgroundColor = sf::Color(0, 17, 0);     // #001100
    sf::Color inactiveColor = sf::Color(0, 102, 0);      // #006600
};

struct ControlSettings {
    std::string pausePlayKey = "Space";
    std::string stepKey = "Right";
    std::string resetKey = "R";
    std::string menuKey = "Escape";
};

struct AudioSettings {
    float masterVolume = 0.7f;
    float sfxVolume = 0.8f;
    bool enabled = true;
};

class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();
    
    bool loadConfig(const std::string& filename);
    bool saveConfig(const std::string& filename);
    
    // Legacy string-based access
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    float getFloat(const std::string& key, float defaultValue = 0.0f) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    
    // Structured settings access
    const DisplaySettings& getDisplaySettings() const { return m_displaySettings; }
    const SimulationSettings& getSimulationSettings() const { return m_simulationSettings; }
    const ThemeSettings& getThemeSettings() const { return m_themeSettings; }
    const ControlSettings& getControlSettings() const { return m_controlSettings; }
    const AudioSettings& getAudioSettings() const { return m_audioSettings; }
    
    // Setters for runtime changes
    void setDisplaySettings(const DisplaySettings& settings) { m_displaySettings = settings; }
    void setSimulationSettings(const SimulationSettings& settings) { m_simulationSettings = settings; }
    void setThemeSettings(const ThemeSettings& settings) { m_themeSettings = settings; }
    void setControlSettings(const ControlSettings& settings) { m_controlSettings = settings; }
    void setAudioSettings(const AudioSettings& settings) { m_audioSettings = settings; }
    
private:
    void parseJsonConfig(const std::string& jsonContent);
    sf::Color parseColor(const std::string& hexColor) const;
    
    std::map<std::string, std::string> m_settings;
    
    // Structured settings
    DisplaySettings m_displaySettings;
    SimulationSettings m_simulationSettings;
    ThemeSettings m_themeSettings;
    ControlSettings m_controlSettings;
    AudioSettings m_audioSettings;
};
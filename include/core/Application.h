#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "MenuSystem.h"
#include "ThemeManager.h"
#include "ConfigManager.h"

class Application
{
public:
    Application();
    ~Application();

    bool initialize();
    void run();
    void shutdown();

private:
    void handleEvents();
    void update(float deltaTime);
    void render();

    sf::RenderWindow m_window;
    bool m_running;
    
    std::unique_ptr<MenuSystem> m_menuSystem;
    std::unique_ptr<ThemeManager> m_themeManager;
    std::unique_ptr<ConfigManager> m_configManager;
    
    sf::Clock m_clock;
};
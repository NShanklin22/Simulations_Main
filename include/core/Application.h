#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "MenuSystem.h"
#include "ThemeManager.h"
#include "ConfigManager.h"
#include "AudioManager.h"
#include "simulations/sorting/quicksort/QuicksortController.h"
#include "simulations/sorting/quicksort/QuicksortVisualizer.h"
#include "simulations/pathfinding/astar/AStarController.h"
#include "simulations/pathfinding/astar/AStarVisualizer.h"

enum class AppState {
    MENU,
    QUICKSORT_SIMULATION,
    ASTAR_SIMULATION,
    SETTINGS
};

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
    
    void initializeQuicksortDemo();
    void initializeAStarDemo();
    void switchToQuicksort();
    void switchToAStar();
    void switchToMenu();
    void exitApplication();
    void applyConfiguration();

    sf::RenderWindow m_window;
    bool m_running;
    AppState m_currentState;
    
    std::unique_ptr<MenuSystem> m_menuSystem;
    std::unique_ptr<ThemeManager> m_themeManager;
    std::unique_ptr<ConfigManager> m_configManager;
    std::unique_ptr<AudioManager> m_audioManager;
    
    // Simulations
    std::unique_ptr<QuicksortController> m_quicksortController;
    std::unique_ptr<QuicksortVisualizer> m_quicksortVisualizer;
    std::unique_ptr<AStarController> m_astarController;
    std::unique_ptr<AStarVisualizer> m_astarVisualizer;
    
    sf::Clock m_clock;
};
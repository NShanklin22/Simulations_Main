#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <functional>

class AudioManager;

enum class MenuState {
    MAIN_MENU,
    SORTING_MENU,
    SETTINGS_MENU,
    SOUND_BOARD
};

struct MenuItem {
    std::string text;
    std::function<void()> action;
    bool enabled = true;
};

class MenuSystem {
public:
    MenuSystem();
    ~MenuSystem();
    
    void initialize(sf::RenderWindow& window);
    void setAudioManager(AudioManager* audioManager);
    void handleEvent(const sf::Event* event);
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    
    // Menu navigation
    void setOnStartQuicksort(std::function<void()> callback);
    void setOnStartAStar(std::function<void()> callback);
    void setOnSettings(std::function<void()> callback);
    void setOnSoundBoard(std::function<void()> callback);
    void setOnExit(std::function<void()> callback);
    
    MenuState getCurrentState() const { return m_currentState; }
    
private:
    void initializeMainMenu();
    void initializeSortingMenu();
    void initializePathfindingMenu();
    void initializeSettingsMenu();
    void initializeSoundBoardMenu();
    
    void navigateUp();
    void navigateDown();
    void selectItem();
    void goBack();
    
    void renderMenuItem(sf::RenderWindow& window, const MenuItem& item, float x, float y, bool selected);
    void renderTitle(sf::RenderWindow& window, const std::string& title, float x, float y);
    
    MenuState m_currentState;
    std::vector<MenuItem> m_currentItems;
    int m_selectedIndex;
    
    // Rendering
    sf::Font m_font;
    bool m_fontLoaded;
    sf::Color m_primaryColor;
    sf::Color m_secondaryColor;
    sf::Color m_backgroundColor;
    sf::Color m_inactiveColor;
    
    // Callbacks
    std::function<void()> m_onStartQuicksort;
    std::function<void()> m_onStartAStar;
    std::function<void()> m_onSettings;
    std::function<void()> m_onSoundBoard;
    std::function<void()> m_onExit;
    
    // Animation
    float m_blinkTimer;
    bool m_showCursor;
    
    // Audio
    AudioManager* m_audioManager;
};
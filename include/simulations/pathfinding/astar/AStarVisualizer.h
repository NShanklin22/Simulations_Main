#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>
#include "AStarController.h"

class AudioManager;

class AStarVisualizer {
public:
    AStarVisualizer();
    ~AStarVisualizer();
    
    void initialize(sf::RenderWindow& window);
    void setController(AStarController* controller);
    void setAudioManager(AudioManager* audioManager);
    
    void handleEvent(const sf::Event* event);
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    void resetView();
    
private:
    enum class EditMode {
        NONE,
        PLACE_WALL,
        PLACE_START,
        PLACE_GOAL
    };
    
    struct ControlButton {
        std::string text;
        std::function<void()> action;
        bool enabled = true;
    };
    
    void drawGrid(sf::RenderWindow& window);
    void drawInfo(sf::RenderWindow& window);
    void drawControls(sf::RenderWindow& window);
    void drawPath(sf::RenderWindow& window, const std::vector<std::pair<int, int>>& path, sf::Color color, float thickness = 3.0f);
    void drawCurrentPath(sf::RenderWindow& window, int currentX, int currentY);
    
    void onAStarStep(const AStarStep& step);
    sf::Color getCellColor(const GridCell& cell);
    void updateGridDisplay();
    
    void initializeControls();
    void navigateControlsLeft();
    void navigateControlsRight();
    void selectControl();
    void renderControlButton(sf::RenderWindow& window, const ControlButton& button, float x, float y, bool selected);
    
    // Grid interaction
    void handleMouseClick(int mouseX, int mouseY);
    void handleMouseMove(int mouseX, int mouseY);
    std::pair<int, int> screenToGrid(int screenX, int screenY);
    bool isValidGridPos(int gridX, int gridY);
    
    AStarController* m_controller;
    AudioManager* m_audioManager;
    
    sf::Font m_font;
    bool m_fontLoaded;
    
    // Control system
    std::vector<ControlButton> m_controlButtons;
    int m_selectedControlIndex;
    bool m_controlsActive;
    
    // Colors (Fallout theme)
    sf::Color m_primaryColor;        // Bright green
    sf::Color m_secondaryColor;      // Brighter green variant
    sf::Color m_backgroundColor;     // Dark green/black
    sf::Color m_wallColor;           // Dark gray
    sf::Color m_startColor;          // Blue
    sf::Color m_goalColor;           // Red
    sf::Color m_openListColor;       // Light green
    sf::Color m_closedListColor;     // Orange
    sf::Color m_pathColor;           // Bright yellow
    sf::Color m_currentColor;        // Cyan
    sf::Color m_inactiveColor;       // Dim green
    
    // Grid rendering
    float m_gridAreaX;
    float m_gridAreaY;
    float m_gridAreaWidth;
    float m_gridAreaHeight;
    float m_cellSize;
    float m_gridOffsetX;
    float m_gridOffsetY;
    
    // Statistics
    int m_stepCount;
    int m_openListSize;
    int m_closedListSize;
    float m_currentSpeed;
    
    // Grid interaction
    EditMode m_editMode;
    int m_hoveredGridX;
    int m_hoveredGridY;
    bool m_mousePressed;
    
    // Camera/view
    sf::View m_gridView;
    bool m_viewInitialized;
};
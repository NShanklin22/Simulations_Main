#pragma once
#include <SFML/Graphics.hpp>
#include "QuicksortController.h"
#include "core/AudioManager.h"
#include "core/AnimationSystem.h"
#include <vector>
#include <string>
#include <functional>
#include <map>

struct ControlButton {
    std::string text;
    std::function<void()> action;
    bool enabled = true;
};

class QuicksortVisualizer {
public:
    QuicksortVisualizer();
    ~QuicksortVisualizer();
    
    void initialize(sf::RenderWindow& window);
    void setController(QuicksortController* controller);
    void setAudioManager(AudioManager* audioManager);
    void handleEvent(const sf::Event* event);
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    
private:
    void drawArray(sf::RenderWindow& window);
    void drawControls(sf::RenderWindow& window);
    void drawInfo(sf::RenderWindow& window);
    void onQuicksortStep(const QuicksortStep& step);
    
    void initializeControls();
    void navigateControlsLeft();
    void navigateControlsRight();
    void selectControl();
    void renderControlButton(sf::RenderWindow& window, const ControlButton& button, float x, float y, bool selected);
    
    sf::Color getBarColor(int index, const QuicksortStep& step);
    void updateBarPositions();
    
    QuicksortController* m_controller;
    AudioManager* m_audioManager;
    
    // Visual elements
    std::vector<sf::RectangleShape> m_bars;
    sf::Font m_font;
    bool m_fontLoaded;
    
    // Control system
    std::vector<ControlButton> m_controlButtons;
    int m_selectedControlIndex;
    bool m_controlsActive;
    
    // Colors (Fallout theme)
    sf::Color m_primaryColor;     // Bright green
    sf::Color m_secondaryColor;   // Amber
    sf::Color m_backgroundColor;  // Dark green/black
    sf::Color m_pivotColor;       // Bright amber
    sf::Color m_activeColor;      // Bright green
    sf::Color m_inactiveColor;    // Dim green
    
    // Layout
    float m_arrayAreaX;
    float m_arrayAreaY;
    float m_arrayAreaWidth;
    float m_arrayAreaHeight;
    float m_barWidth;
    float m_barSpacing;
    
    // Animation system for smooth easing transitions
    AnimationSystem m_animationSystem;
    std::map<int, int> m_barAnimationIds;  // Map bar index to animation ID
    std::vector<sf::Vector2f> m_targetPositions;  // Target positions for each bar
    bool m_animationEnabled;
    float m_animationSpeed;
    bool m_animationsInProgress;
    
    // Animation helpers
    void startBarAnimations(const std::vector<int>& array);
    void calculateTargetPositions(const std::vector<int>& array);
    void updateAnimatedBarPositions();
    bool areAnimationsComplete() const;
    
    // Statistics
    int m_operationCount;
    int m_comparisonCount;
    int m_swapCount;
    float m_currentSpeed;
};
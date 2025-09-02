#include "simulations/sorting/quicksort/QuicksortVisualizer.h"
#include <iostream>
#include <sstream>
#include <algorithm>

QuicksortVisualizer::QuicksortVisualizer() 
    : m_controller(nullptr)
    , m_audioManager(nullptr)
    , m_fontLoaded(false)
    , m_selectedControlIndex(0)
    , m_controlsActive(true)
    , m_primaryColor(0, 255, 65)      // Bright green
    , m_secondaryColor(0, 255, 150)   // Brighter green variant
    , m_backgroundColor(0, 17, 0)     // Dark green/black
    , m_pivotColor(100, 255, 100)     // Light green for pivot
    , m_activeColor(0, 255, 65)       // Bright green
    , m_inactiveColor(0, 102, 0)      // Dim green
    , m_arrayAreaX(50.0f)
    , m_arrayAreaY(120.0f)
    , m_arrayAreaWidth(1100.0f)
    , m_arrayAreaHeight(350.0f)
    , m_barWidth(0.0f)
    , m_barSpacing(2.0f)
    , m_animationEnabled(true)
    , m_animationSpeed(1.0f)
    , m_animationsInProgress(false)
    , m_operationCount(0)
    , m_comparisonCount(0)
    , m_swapCount(0)
    , m_currentSpeed(500.0f)
{
}

QuicksortVisualizer::~QuicksortVisualizer() {
}

void QuicksortVisualizer::initialize(sf::RenderWindow& window) {
    // Try to load fonts with fallback
    m_fontLoaded = m_font.openFromFile("assets/fonts/CourierNew.ttf");
    if (!m_fontLoaded) {
        m_fontLoaded = m_font.openFromFile("C:/Windows/Fonts/consola.ttf");
        if (!m_fontLoaded) {
            m_fontLoaded = m_font.openFromFile("C:/Windows/Fonts/arial.ttf");
            if (!m_fontLoaded) {
                std::cout << "Warning: Could not load any font for QuicksortVisualizer\n";
            }
        }
    }
    
    // Initialize control buttons
    initializeControls();
}

void QuicksortVisualizer::setController(QuicksortController* controller) {
    m_controller = controller;
    
    if (m_controller) {
        // Set up callback to receive step updates
        m_controller->setStepCallback([this](const QuicksortStep& step) {
            onQuicksortStep(step);
        });
        
        updateBarPositions();
    }
}

void QuicksortVisualizer::handleEvent(const sf::Event* event) {
    if (!m_controller || !event) return;
    
    if (auto keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        switch (keyPressed->code) {
            case sf::Keyboard::Key::Left:
                navigateControlsLeft();
                break;
            case sf::Keyboard::Key::Right:
                navigateControlsRight();
                break;
            case sf::Keyboard::Key::Enter:
                selectControl();
                break;
            default:
                break;
        }
    }
}

void QuicksortVisualizer::update(float deltaTime) {
    // Update animation system for smooth transitions
    m_animationSystem.update(deltaTime);
    
    // Update controller (algorithm stepping)
    if (m_controller) {
        m_controller->update(deltaTime);
    }
    
    // Update bar positions based on current animations
    if (m_animationsInProgress) {
        updateAnimatedBarPositions();
        
        // Check if all animations are complete
        if (areAnimationsComplete()) {
            m_animationsInProgress = false;
            // Clean up completed animations
            m_animationSystem.cleanupCompletedAnimations();
        }
    }
}

void QuicksortVisualizer::render(sf::RenderWindow& window) {
    drawArray(window);
    drawInfo(window);
    drawControls(window);
}

void QuicksortVisualizer::drawArray(sf::RenderWindow& window) {
    for (const auto& bar : m_bars) {
        window.draw(bar);
    }
}

void QuicksortVisualizer::drawInfo(sf::RenderWindow& window) {
    if (!m_controller || !m_fontLoaded) return;
    
    // Top line - Status and current operation
    std::stringstream topInfo;
    topInfo << "QUICKSORT VISUALIZATION | ";
    
    switch (m_controller->getState()) {
        case QuicksortState::READY:
            topInfo << "STATUS: READY";
            break;
        case QuicksortState::SORTING:
            topInfo << "STATUS: SORTING";
            break;
        case QuicksortState::PAUSED:
            topInfo << "STATUS: PAUSED";
            break;
        case QuicksortState::COMPLETED:
            topInfo << "STATUS: COMPLETED";
            break;
    }
    
    const auto& step = m_controller->getCurrentStep();
    if (!step.description.empty()) {
        topInfo << " | " << step.description;
    }
    
    sf::Text topText(m_font, topInfo.str(), 18);
    topText.setFillColor(m_primaryColor);
    topText.setPosition({50.0f, 20.0f});
    window.draw(topText);
    
    // Second line - Statistics
    std::stringstream statsInfo;
    statsInfo << "SPEED: " << static_cast<int>(m_controller->getSpeed()) << "ms | ";
    statsInfo << "STEP: " << (m_controller->getCurrentStepIndex() + 1) << "/" << m_controller->getTotalSteps() << " | ";
    statsInfo << "OPERATIONS: " << m_controller->getOperationCount() << " | ";
    statsInfo << "COMPARISONS: " << m_controller->getComparisonCount() << " | ";
    statsInfo << "SWAPS: " << m_controller->getSwapCount();
    
    sf::Text statsText(m_font, statsInfo.str(), 16);
    statsText.setFillColor(m_inactiveColor);
    statsText.setPosition({50.0f, 45.0f});
    window.draw(statsText);
}

void QuicksortVisualizer::drawControls(sf::RenderWindow& window) {
    if (!m_fontLoaded) return;
    
    // Draw instruction text
    sf::Text instructionText(m_font, "CONTROLS: Use LEFT/RIGHT arrows to navigate, ENTER to select", 14);
    instructionText.setFillColor(m_inactiveColor);
    instructionText.setPosition({50.0f, 520.0f});
    window.draw(instructionText);
    
    // Draw control buttons in a row
    float startX = 50.0f;
    float startY = 550.0f;
    float buttonSpacing = 140.0f;
    
    for (size_t i = 0; i < m_controlButtons.size(); ++i) {
        bool selected = (i == static_cast<size_t>(m_selectedControlIndex));
        float x = startX + i * buttonSpacing;
        renderControlButton(window, m_controlButtons[i], x, startY, selected);
    }
}

void QuicksortVisualizer::setAudioManager(AudioManager* audioManager) {
    m_audioManager = audioManager;
}

void QuicksortVisualizer::onQuicksortStep(const QuicksortStep& step) {
    // Start smooth animations to new positions
    if (m_animationEnabled) {
        startBarAnimations(step.array);
    } else {
        // Instant update without animation
        updateBarPositions();
    }
    
    // Play audio based on step description
    if (m_audioManager) {
        if (step.description.find("Finale:") != std::string::npos) {
            // Play ascending pitch for finale sequence based on the highlighted value
            if (step.pivotIndex >= 0 && step.pivotIndex < static_cast<int>(step.array.size())) {
                int value = step.array[step.pivotIndex];
                int minVal = *std::min_element(step.array.begin(), step.array.end());
                int maxVal = *std::max_element(step.array.begin(), step.array.end());
                float pitch = m_audioManager->mapValueToPitch(value, minVal, maxVal);
                m_audioManager->playSound(SoundType::COMPARISON, pitch, 1.0f);
            }
        } else if (step.description.find("Sorting Complete!") != std::string::npos) {
            // Play final completion fanfare
            m_audioManager->playSound(SoundType::ALGORITHM_COMPLETE, 1.0f, 1.0f);
        } else if (step.description.find("Left scan") != std::string::npos || 
                   step.description.find("Right scan") != std::string::npos ||
                   step.description.find("Compare") != std::string::npos) {
            // Play comparison sound with pitch based on array values
            if (step.lowIndex >= 0 && step.lowIndex < static_cast<int>(step.array.size())) {
                int value = step.array[step.lowIndex];
                int minVal = *std::min_element(step.array.begin(), step.array.end());
                int maxVal = *std::max_element(step.array.begin(), step.array.end());
                float pitch = m_audioManager->mapValueToPitch(value, minVal, maxVal);
                m_audioManager->playSound(SoundType::COMPARISON, pitch, 0.8f);
            }
        } else if (step.description.find("Will swap") != std::string::npos || 
                   step.description.find("Swapped") != std::string::npos) {
            // Play swap sound
            m_audioManager->playSound(SoundType::SWAP, 1.0f, 0.9f);
        } else if (step.description.find("Pivot") != std::string::npos || 
                   step.description.find("pivot") != std::string::npos) {
            // Play pivot selection sound
            m_audioManager->playSound(SoundType::PIVOT_SELECT, 1.0f, 0.7f);
        } else if (step.description.find("Complete") != std::string::npos) {
            // Play completion sound
            m_audioManager->playSound(SoundType::ALGORITHM_COMPLETE, 1.0f, 1.0f);
        }
    }
}

sf::Color QuicksortVisualizer::getBarColor(int index, const QuicksortStep& step) {
    // Special finale sequence highlighting
    if (step.description.find("Finale:") != std::string::npos) {
        if (index == step.pivotIndex) {
            // Bright highlight for the current bar in finale
            return sf::Color(0, 255, 150);  // Bright cyan-green for finale highlight
        } else if (index < step.pivotIndex) {
            // Already highlighted bars stay bright
            return sf::Color(0, 255, 65);  // Bright green for completed bars
        } else {
            // Not yet highlighted bars stay dim
            return m_inactiveColor;  // Dim green
        }
    }
    
    // Final celebration - all bars bright
    if (step.description.find("Sorting Complete!") != std::string::npos) {
        return sf::Color(0, 255, 65);  // All bars bright green
    }
    
    // Proper QuickSort Hoare partitioning highlighting - focused and clear
    
    // 1. BRIGHTEST: Active scanning pointers (left and right)
    if (step.description.find("Left scan") != std::string::npos ||
        step.description.find("Right scan") != std::string::npos ||
        step.description.find("Move pointers") != std::string::npos) {
        // Highlight the current left and right pointers
        if (index == step.lowIndex || index == step.highIndex) {
            return sf::Color(0, 255, 65);  // Brightest green - active pointers
        }
    }
    
    // 2. BRIGHTEST: Elements being swapped
    if (step.description.find("Will swap") != std::string::npos || 
        step.description.find("Swapped") != std::string::npos ||
        step.description.find("Moving pivot") != std::string::npos ||
        step.description.find("Partition complete") != std::string::npos) {
        if (index == step.lowIndex || index == step.highIndex) {
            return sf::Color(0, 255, 65);  // Brightest green - elements being swapped
        }
    }
    
    // 3. MEDIUM: Pivot (always visible as reference)
    if (index == step.pivotIndex) {
        return sf::Color(100, 255, 100);  // Medium green - pivot reference
    }
    
    // 4. NO RANGE HIGHLIGHTING - removed excessive background highlighting
    // Elements are either active (brightest), pivot (medium), or inactive (dim)
    
    // 5. DIM: Everything else
    return m_inactiveColor;  // Dim green (0, 102, 0)
}

void QuicksortVisualizer::updateBarPositions() {
    if (!m_controller) return;
    
    const auto& array = m_controller->getCurrentArray();
    const auto& step = m_controller->getCurrentStep();
    
    if (array.empty()) return;
    
    // Calculate bar dimensions
    float totalBarArea = m_arrayAreaWidth - (array.size() - 1) * m_barSpacing;
    m_barWidth = totalBarArea / array.size();
    
    // Find max value for scaling
    int maxValue = *std::max_element(array.begin(), array.end());
    if (maxValue == 0) maxValue = 1; // Avoid division by zero
    
    // Update bars
    m_bars.clear();
    m_bars.reserve(array.size());
    
    for (size_t i = 0; i < array.size(); ++i) {
        sf::RectangleShape bar;
        
        // Calculate position and size
        float x = m_arrayAreaX + i * (m_barWidth + m_barSpacing);
        float height = (static_cast<float>(array[i]) / maxValue) * m_arrayAreaHeight;
        float y = m_arrayAreaY + m_arrayAreaHeight - height;
        
        bar.setPosition({x, y});
        bar.setSize(sf::Vector2f(m_barWidth, height));
        bar.setFillColor(getBarColor(static_cast<int>(i), step));
        
        // Add border for better visibility
        bar.setOutlineThickness(1.0f);
        bar.setOutlineColor(m_backgroundColor);
        
        m_bars.push_back(bar);
    }
}

void QuicksortVisualizer::initializeControls() {
    m_controlButtons.clear();
    
    // Add control buttons
    m_controlButtons.push_back({"PLAY/PAUSE", [this]() {
        if (m_controller->getState() == QuicksortState::READY || 
            m_controller->getState() == QuicksortState::PAUSED) {
            m_controller->start();
        } else if (m_controller->getState() == QuicksortState::SORTING) {
            m_controller->pause();
        }
    }});
    
    m_controlButtons.push_back({"STEP FORWARD", [this]() {
        if (m_controller->getState() == QuicksortState::READY ||
            m_controller->getState() == QuicksortState::PAUSED) {
            m_controller->step();
        }
    }});
    
    m_controlButtons.push_back({"STEP BACK", [this]() {
        if (m_controller->getState() == QuicksortState::PAUSED) {
            m_controller->stepBack();
        }
    }});
    
    m_controlButtons.push_back({"SPEED UP", [this]() {
        m_controller->speedUp();
        m_currentSpeed = m_controller->getSpeed();
    }});
    
    m_controlButtons.push_back({"SLOW DOWN", [this]() {
        m_controller->slowDown();
        m_currentSpeed = m_controller->getSpeed();
    }});
    
    m_controlButtons.push_back({"RESET", [this]() {
        m_controller->reset();
        updateBarPositions();
        m_operationCount = 0;
        m_comparisonCount = 0;
        m_swapCount = 0;
    }});
}

void QuicksortVisualizer::navigateControlsLeft() {
    if (m_selectedControlIndex > 0) {
        m_selectedControlIndex--;
    } else {
        m_selectedControlIndex = static_cast<int>(m_controlButtons.size()) - 1;
    }
    
    // Play button hover sound
    if (m_audioManager) {
        m_audioManager->playSound(SoundType::BUTTON_HOVER, 1.1f, 0.5f);
    }
}

void QuicksortVisualizer::navigateControlsRight() {
    if (m_selectedControlIndex < static_cast<int>(m_controlButtons.size()) - 1) {
        m_selectedControlIndex++;
    } else {
        m_selectedControlIndex = 0;
    }
    
    // Play button hover sound
    if (m_audioManager) {
        m_audioManager->playSound(SoundType::BUTTON_HOVER, 0.9f, 0.5f);
    }
}

void QuicksortVisualizer::selectControl() {
    if (m_selectedControlIndex >= 0 && m_selectedControlIndex < static_cast<int>(m_controlButtons.size())) {
        const auto& button = m_controlButtons[m_selectedControlIndex];
        if (button.enabled && button.action) {
            // Play button press sound
            if (m_audioManager) {
                m_audioManager->playSound(SoundType::BUTTON_PRESS, 1.0f, 0.7f);
            }
            button.action();
        }
    }
}

void QuicksortVisualizer::renderControlButton(sf::RenderWindow& window, const ControlButton& button, float x, float y, bool selected) {
    if (!m_fontLoaded) return;
    
    sf::Color textColor = button.enabled ? 
        (selected ? m_secondaryColor : m_primaryColor) : 
        m_inactiveColor;
    
    std::string displayText = button.text;
    if (selected) {
        displayText = "> " + displayText + " <";
    } else {
        displayText = "  " + displayText + "  ";
    }
    
    sf::Text text(m_font, displayText, 16);
    text.setFillColor(textColor);
    text.setPosition({x, y});
    window.draw(text);
}

// Animation helper methods for smooth easing transitions

void QuicksortVisualizer::startBarAnimations(const std::vector<int>& array) {
    if (!m_controller) return;
    
    // Ensure bars exist before trying to animate them
    if (m_bars.empty() || m_bars.size() != array.size()) {
        // First time or array size changed - create bars without animation
        updateBarPositions();
        return;
    }
    
    // Disable animations for now - just update positions directly
    // This avoids the Y-position bug until we can properly implement horizontal-only animation
    m_animationsInProgress = false;
    updateBarPositions();
    
    // Update colors for visual feedback
    const auto& step = m_controller->getCurrentStep();
    for (size_t i = 0; i < m_bars.size(); ++i) {
        m_bars[i].setFillColor(getBarColor(static_cast<int>(i), step));
    }
}

void QuicksortVisualizer::calculateTargetPositions(const std::vector<int>& array) {
    m_targetPositions.clear();
    m_targetPositions.reserve(array.size());
    
    if (array.empty()) return;
    
    // Calculate bar dimensions
    float totalBarArea = m_arrayAreaWidth - (array.size() - 1) * m_barSpacing;
    float barWidth = totalBarArea / array.size();
    
    // Find max value for scaling
    int maxValue = *std::max_element(array.begin(), array.end());
    if (maxValue == 0) maxValue = 1; // Avoid division by zero
    
    // Calculate target position for each bar
    for (size_t i = 0; i < array.size(); ++i) {
        float x = m_arrayAreaX + i * (barWidth + m_barSpacing);
        float height = (static_cast<float>(array[i]) / maxValue) * m_arrayAreaHeight;
        float y = m_arrayAreaY + m_arrayAreaHeight - height;
        
        m_targetPositions.push_back(sf::Vector2f(x, y));
    }
}

void QuicksortVisualizer::updateAnimatedBarPositions() {
    // Update bar positions based on current animation states
    for (const auto& pair : m_barAnimationIds) {
        int barIndex = pair.first;
        int animationId = pair.second;
        
        if (barIndex >= 0 && barIndex < static_cast<int>(m_bars.size())) {
            sf::Vector2f animatedPos = m_animationSystem.getCurrentPosition(animationId);
            
            // Check for invalid animation position (avoid glitching to upper left)
            if (animatedPos.x >= 0.0f && animatedPos.y >= 0.0f) {
                m_bars[barIndex].setPosition(animatedPos);
            }
            // If invalid position, keep the bar at its current position
        }
    }
}

bool QuicksortVisualizer::areAnimationsComplete() const {
    // Check if all bar animations are complete
    for (const auto& pair : m_barAnimationIds) {
        int animationId = pair.second;
        if (!m_animationSystem.isAnimationComplete(animationId)) {
            return false;
        }
    }
    return true;
}
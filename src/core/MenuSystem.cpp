#include "core/MenuSystem.h"
#include "core/AudioManager.h"
#include <iostream>

MenuSystem::MenuSystem() 
    : m_currentState(MenuState::MAIN_MENU)
    , m_selectedIndex(0)
    , m_primaryColor(0, 255, 65)      // Bright green #00FF41
    , m_secondaryColor(0, 255, 150)   // Brighter green variant #00FF96
    , m_backgroundColor(0, 17, 0)     // Dark green #001100
    , m_inactiveColor(0, 102, 0)      // Dim green #006600
    , m_blinkTimer(0.0f)
    , m_showCursor(true)
    , m_fontLoaded(false)
    , m_audioManager(nullptr)
{
}

MenuSystem::~MenuSystem() {
}

void MenuSystem::initialize(sf::RenderWindow& window) {
    // Try to load custom font, fallback to system fonts
    m_fontLoaded = m_font.openFromFile("assets/fonts/terminal.ttf");
    
    if (!m_fontLoaded) {
        std::cout << "Warning: Could not load terminal font, trying system font...\n";
        
        // Try to load common system fonts
        m_fontLoaded = m_font.openFromFile("C:/Windows/Fonts/consola.ttf");
        if (!m_fontLoaded) {
            m_fontLoaded = m_font.openFromFile("C:/Windows/Fonts/arial.ttf");
            if (!m_fontLoaded) {
                m_fontLoaded = m_font.openFromFile("C:/Windows/Fonts/calibri.ttf");
                if (!m_fontLoaded) {
                    std::cout << "Error: Could not load any font!\n";
                }
            }
        }
    }
    
    if (m_fontLoaded) {
        std::cout << "Font loaded successfully\n";
    } else {
        std::cout << "WARNING: No font loaded - text may not be visible!\n";
    }
    
    initializeMainMenu();
}

void MenuSystem::setAudioManager(AudioManager* audioManager) {
    m_audioManager = audioManager;
}

void MenuSystem::handleEvent(const sf::Event* event) {
    if (auto keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        switch (keyPressed->code) {
            case sf::Keyboard::Key::Up:
                navigateUp();
                break;
            case sf::Keyboard::Key::Down:
                navigateDown();
                break;
            case sf::Keyboard::Key::Enter:
                selectItem();
                break;
            case sf::Keyboard::Key::Escape:
                goBack();
                break;
            default:
                break;
        }
    }
}

void MenuSystem::update(float deltaTime) {
    // Update cursor blink animation
    m_blinkTimer += deltaTime;
    if (m_blinkTimer >= 0.5f) {
        m_showCursor = !m_showCursor;
        m_blinkTimer = 0.0f;
    }
}

void MenuSystem::render(sf::RenderWindow& window) {
    // Render title based on current menu state
    std::string title;
    switch (m_currentState) {
        case MenuState::MAIN_MENU:
            title = "ALGORITHM VISUALIZATION TERMINAL";
            break;
        case MenuState::SORTING_MENU:
            title = "SORTING ALGORITHMS";
            break;
        case MenuState::SETTINGS_MENU:
            title = "SETTINGS";
            break;
        case MenuState::SOUND_BOARD:
            title = "SOUND BOARD - TEST SWAP SOUNDS";
            break;
    }
    
    renderTitle(window, title, 100, 100);
    
    // Render menu items
    float startY = 250;
    float itemSpacing = 60;
    
    for (size_t i = 0; i < m_currentItems.size(); ++i) {
        bool selected = (i == static_cast<size_t>(m_selectedIndex));
        renderMenuItem(window, m_currentItems[i], 150, startY + i * itemSpacing, selected);
    }
    
    // Render footer instructions
    if (m_fontLoaded) {
        sf::Text instructions(m_font, "Use UP/DOWN arrows to navigate, ENTER to select, ESC to go back", 18);
        instructions.setFillColor(m_inactiveColor);
        instructions.setPosition({100, 650});
        window.draw(instructions);
    }
}

void MenuSystem::setOnStartQuicksort(std::function<void()> callback) {
    m_onStartQuicksort = callback;
}

void MenuSystem::setOnStartAStar(std::function<void()> callback) {
    m_onStartAStar = callback;
}

void MenuSystem::setOnSettings(std::function<void()> callback) {
    m_onSettings = callback;
}

void MenuSystem::setOnSoundBoard(std::function<void()> callback) {
    m_onSoundBoard = callback;
}

void MenuSystem::setOnExit(std::function<void()> callback) {
    m_onExit = callback;
}

void MenuSystem::initializeMainMenu() {
    m_currentState = MenuState::MAIN_MENU;
    m_selectedIndex = 0;
    m_currentItems.clear();
    
    // Main menu items
    m_currentItems.push_back({"SORTING ALGORITHMS", [this]() {
        initializeSortingMenu();
    }});
    
    m_currentItems.push_back({"PATHFINDING ALGORITHMS", [this]() {
        initializePathfindingMenu();
    }});
    
    m_currentItems.push_back({"SOUND BOARD", [this]() {
        initializeSoundBoardMenu();
    }});
    
    m_currentItems.push_back({"SEARCH ALGORITHMS", [this]() {
        // TODO: Implement search menu
        std::cout << "Search algorithms not yet implemented\n";
    }, false}); // Disabled for now
    
    m_currentItems.push_back({"SETTINGS", [this]() {
        initializeSettingsMenu();
    }});
    
    m_currentItems.push_back({"EXIT", [this]() {
        if (m_onExit) m_onExit();
    }});
}

void MenuSystem::initializeSortingMenu() {
    m_currentState = MenuState::SORTING_MENU;
    m_selectedIndex = 0;
    m_currentItems.clear();
    
    // Sorting algorithm items
    m_currentItems.push_back({"QUICKSORT", [this]() {
        if (m_onStartQuicksort) m_onStartQuicksort();
    }});
    
    m_currentItems.push_back({"MERGE SORT", [this]() {
        // TODO: Implement merge sort
        std::cout << "Merge sort not yet implemented\n";
    }, false}); // Disabled for now
    
    m_currentItems.push_back({"HEAP SORT", [this]() {
        // TODO: Implement heap sort
        std::cout << "Heap sort not yet implemented\n";
    }, false}); // Disabled for now
    
    m_currentItems.push_back({"BACK TO MAIN MENU", [this]() {
        initializeMainMenu();
    }});
}

void MenuSystem::initializePathfindingMenu() {
    m_currentState = MenuState::SORTING_MENU; // Reuse sorting menu state for pathfinding
    m_selectedIndex = 0;
    m_currentItems.clear();
    
    // Pathfinding algorithm items
    m_currentItems.push_back({"A* PATHFINDING", [this]() {
        if (m_onStartAStar) m_onStartAStar();
    }});
    
    m_currentItems.push_back({"DIJKSTRA'S ALGORITHM", [this]() {
        std::cout << "Dijkstra's algorithm not yet implemented\n";
    }, false}); // Disabled for now
    
    m_currentItems.push_back({"BREADTH-FIRST SEARCH", [this]() {
        std::cout << "BFS not yet implemented\n";
    }, false}); // Disabled for now
    
    m_currentItems.push_back({"DEPTH-FIRST SEARCH", [this]() {
        std::cout << "DFS not yet implemented\n";
    }, false}); // Disabled for now
    
    m_currentItems.push_back({"BACK TO MAIN MENU", [this]() {
        initializeMainMenu();
    }});
}

void MenuSystem::initializeSettingsMenu() {
    m_currentState = MenuState::SETTINGS_MENU;
    m_selectedIndex = 0;
    m_currentItems.clear();
    
    // Settings items
    m_currentItems.push_back({"DISPLAY SETTINGS", [this]() {
        // TODO: Implement display settings
        std::cout << "Display settings not yet implemented\n";
    }, false}); // Disabled for now
    
    m_currentItems.push_back({"AUDIO SETTINGS", [this]() {
        // TODO: Implement audio settings
        std::cout << "Audio settings not yet implemented\n";
    }, false}); // Disabled for now
    
    m_currentItems.push_back({"BACK TO MAIN MENU", [this]() {
        initializeMainMenu();
    }});
}

void MenuSystem::initializeSoundBoardMenu() {
    m_currentState = MenuState::SOUND_BOARD;
    m_selectedIndex = 0;
    m_currentItems.clear();
    
    // Sound testing items
    m_currentItems.push_back({"CURRENT SWAP SOUND", [this]() {
        if (m_audioManager) {
            std::cout << "Playing current swap sound\n";
            m_audioManager->playSound(SoundType::SWAP, 1.0f, 0.9f);
        }
    }});
    
    m_currentItems.push_back({"UP ARROW SOUND", [this]() {
        if (m_audioManager) {
            std::cout << "Playing up arrow navigation sound\n";
            m_audioManager->playSound(SoundType::MENU_NAVIGATE, 1.1f, 0.7f);
        }
    }});
    
    m_currentItems.push_back({"LOW WARM HUM", [this]() {
        if (m_audioManager) {
            std::cout << "Playing low warm hum\n";
            m_audioManager->playSound(SoundType::COMPARISON, 0.6f, 0.8f);
        }
    }});
    
    m_currentItems.push_back({"SOFT RUMBLE", [this]() {
        if (m_audioManager) {
            std::cout << "Playing soft rumble\n";
            m_audioManager->playSound(SoundType::MENU_SELECT, 0.5f, 0.7f);
        }
    }});
    
    m_currentItems.push_back({"QUICK CLICK (FAST)", [this]() {
        if (m_audioManager) {
            std::cout << "Playing quick click for fast simulation\n";
            m_audioManager->playSound(SoundType::COMPARISON, 1.0f, 0.4f);
        }
    }});
    
    m_currentItems.push_back({"TINY POP (FAST)", [this]() {
        if (m_audioManager) {
            std::cout << "Playing tiny pop for fast simulation\n";
            m_audioManager->playSound(SoundType::PIVOT_SELECT, 1.5f, 0.3f);
        }
    }});
    
    m_currentItems.push_back({"MINI CHIME (FAST)", [this]() {
        if (m_audioManager) {
            std::cout << "Playing mini chime for fast simulation\n";
            m_audioManager->playSound(SoundType::SWAP, 1.4f, 0.25f);
        }
    }});
    
    m_currentItems.push_back({"BACK TO MAIN MENU", [this]() {
        initializeMainMenu();
    }});
}

void MenuSystem::navigateUp() {
    do {
        m_selectedIndex--;
        if (m_selectedIndex < 0) {
            m_selectedIndex = static_cast<int>(m_currentItems.size()) - 1;
        }
    } while (!m_currentItems[m_selectedIndex].enabled);
    
    // Play navigation sound
    if (m_audioManager) {
        m_audioManager->playSound(SoundType::MENU_NAVIGATE, 1.1f, 0.7f);
    }
}

void MenuSystem::navigateDown() {
    do {
        m_selectedIndex++;
        if (m_selectedIndex >= static_cast<int>(m_currentItems.size())) {
            m_selectedIndex = 0;
        }
    } while (!m_currentItems[m_selectedIndex].enabled);
    
    // Play navigation sound (same as up arrow)
    if (m_audioManager) {
        m_audioManager->playSound(SoundType::MENU_NAVIGATE, 1.1f, 0.7f);
    }
}

void MenuSystem::selectItem() {
    if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(m_currentItems.size())) {
        const auto& item = m_currentItems[m_selectedIndex];
        if (item.enabled && item.action) {
            // Play selection sound
            if (m_audioManager) {
                m_audioManager->playSound(SoundType::MENU_SELECT, 1.0f, 0.8f);
            }
            item.action();
        }
    }
}

void MenuSystem::goBack() {
    switch (m_currentState) {
        case MenuState::SORTING_MENU:
        case MenuState::SETTINGS_MENU:
        case MenuState::SOUND_BOARD:
            initializeMainMenu();
            break;
        case MenuState::MAIN_MENU:
            // ESC from main menu exits application
            if (m_onExit) m_onExit();
            break;
    }
}

void MenuSystem::renderMenuItem(sf::RenderWindow& window, const MenuItem& item, float x, float y, bool selected) {
    sf::Color textColor = item.enabled ? 
        (selected ? m_secondaryColor : m_primaryColor) : 
        m_inactiveColor;
    
    std::string displayText = item.text;
    if (selected && m_showCursor) {
        displayText = "> " + displayText + " <";
    } else if (selected) {
        displayText = "  " + displayText + "  ";
    } else {
        displayText = "  " + displayText;
    }
    
    if (m_fontLoaded) {
        sf::Text text(m_font, displayText, 24);
        text.setFillColor(textColor);
        text.setPosition({x, y});
        window.draw(text);
    } else {
        // Skip rendering if no font available
        std::cout << "Skipping text render: " << displayText << std::endl;
    }
}

void MenuSystem::renderTitle(sf::RenderWindow& window, const std::string& title, float x, float y) {
    if (m_fontLoaded) {
        sf::Text titleText(m_font, title, 32);
        titleText.setFillColor(m_primaryColor);
        titleText.setPosition({x, y});
        window.draw(titleText);
    }
    
    // Add decorative line under title
    sf::RectangleShape line(sf::Vector2f(static_cast<float>(title.length() * 20), 3.0f));
    line.setFillColor(m_primaryColor);
    line.setPosition({x, y + 45});
    window.draw(line);
}
#include "core/Application.h"
#include <iostream>
#include <random>
#include <cstdint>
#include <algorithm>

Application::Application() 
    : m_running(false)
    , m_currentState(AppState::MENU) // Start in menu for Phase 3
    , m_configManager(std::make_unique<ConfigManager>())
    , m_menuSystem(std::make_unique<MenuSystem>())
    , m_audioManager(std::make_unique<AudioManager>())
{}

Application::~Application() {
    shutdown();
}

bool Application::initialize() {
    // Load configuration first
    m_configManager->loadConfig("config/settings.json");
    
    // Apply configuration to window
    applyConfiguration();
    
    m_running = true;
    
    // Initialize audio system
    m_audioManager->initialize();
    
    // Apply audio configuration
    const auto& audioSettings = m_configManager->getAudioSettings();
    m_audioManager->setMasterVolume(audioSettings.masterVolume);
    m_audioManager->setEnabled(audioSettings.enabled);
    
    // Initialize menu system
    m_menuSystem->initialize(m_window);
    m_menuSystem->setAudioManager(m_audioManager.get());
    
    // Set up menu callbacks
    m_menuSystem->setOnStartQuicksort([this]() {
        switchToQuicksort();
    });
    
    m_menuSystem->setOnStartAStar([this]() {
        switchToAStar();
    });
    
    m_menuSystem->setOnExit([this]() {
        exitApplication();
    });
    
    std::cout << "Application initialized successfully\n";
    return true;
}

void Application::run() {
    if (!initialize()) {
        std::cerr << "Failed to initialize application\n";
        return;
    }
    
    while (m_running && m_window.isOpen()) {
        float deltaTime = m_clock.restart().asSeconds();
        
        handleEvents();
        update(deltaTime);
        render();
    }
}

void Application::shutdown() {
    if (m_window.isOpen()) {
        m_window.close();
    }
    m_running = false;
}

void Application::handleEvents() {
    while (auto event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_running = false;
        }
        
        // Handle global escape key - different behavior based on state
        if (auto keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                if (m_currentState == AppState::QUICKSORT_SIMULATION || 
                    m_currentState == AppState::ASTAR_SIMULATION) {
                    switchToMenu(); // Return to menu from simulation
                } else if (m_currentState == AppState::MENU) {
                    // Let menu system handle escape (might exit or go back)
                }
            }
        }
        
        // Pass events to current state
        switch (m_currentState) {
            case AppState::MENU:
                if (m_menuSystem) {
                    m_menuSystem->handleEvent(&(*event));
                }
                break;
            case AppState::QUICKSORT_SIMULATION:
                if (m_quicksortVisualizer) {
                    m_quicksortVisualizer->handleEvent(&(*event));
                }
                break;
            case AppState::ASTAR_SIMULATION:
                if (m_astarVisualizer) {
                    m_astarVisualizer->handleEvent(&(*event));
                }
                break;
            case AppState::SETTINGS:
                // TODO: Implement settings event handling
                break;
        }
    }
}

void Application::update(float deltaTime) {
    switch (m_currentState) {
        case AppState::MENU:
            if (m_menuSystem) {
                m_menuSystem->update(deltaTime);
            }
            break;
        case AppState::QUICKSORT_SIMULATION:
            if (m_quicksortVisualizer) {
                m_quicksortVisualizer->update(deltaTime);
            }
            break;
        case AppState::ASTAR_SIMULATION:
            if (m_astarVisualizer) {
                m_astarVisualizer->update(deltaTime);
            }
            break;
        case AppState::SETTINGS:
            // TODO: Implement settings update
            break;
    }
}

void Application::render() {
    // Get background color from configuration
    sf::Color backgroundColor = m_configManager->getThemeSettings().backgroundColor;
    m_window.clear(backgroundColor);
    
    switch (m_currentState) {
        case AppState::MENU:
            if (m_menuSystem) {
                m_menuSystem->render(m_window);
            }
            break;
        case AppState::QUICKSORT_SIMULATION:
            if (m_quicksortVisualizer) {
                m_quicksortVisualizer->render(m_window);
            }
            break;
        case AppState::ASTAR_SIMULATION:
            if (m_astarVisualizer) {
                m_astarVisualizer->render(m_window);
            }
            break;
        case AppState::SETTINGS:
            // TODO: Implement settings rendering
            break;
    }
    
    m_window.display();
}

void Application::initializeQuicksortDemo() {
    // Create quicksort controller and visualizer if not already created
    if (!m_quicksortController) {
        m_quicksortController = std::make_unique<QuicksortController>();
    }
    if (!m_quicksortVisualizer) {
        m_quicksortVisualizer = std::make_unique<QuicksortVisualizer>();
    }
    
    // Initialize visualizer
    m_quicksortVisualizer->initialize(m_window);
    m_quicksortVisualizer->setController(m_quicksortController.get());
    m_quicksortVisualizer->setAudioManager(m_audioManager.get());
    
    // Generate array with uniform height distribution for visual appeal
    const auto& simSettings = m_configManager->getSimulationSettings();
    std::vector<int> demoArray;
    
    // Create array with configured size and uniform height increments
    int arraySize = simSettings.defaultArraySize;
    int minHeight = 20;
    int maxHeight = 300;
    int heightIncrement = (maxHeight - minHeight) / arraySize;
    
    // Create sequential values for uniform distribution
    for (int i = 0; i < arraySize; ++i) {
        demoArray.push_back(minHeight + i * heightIncrement);
    }
    
    // Shuffle the array to make it unsorted but maintain good height distribution
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(demoArray.begin(), demoArray.end(), gen);
    
    // Initialize controller with demo data and configuration
    m_quicksortController->initialize(demoArray);
    
    // Apply speed setting from configuration
    m_quicksortController->setSpeed(static_cast<float>(simSettings.defaultSpeed));
    
    std::cout << "Quicksort demo initialized with " << demoArray.size() << " elements, speed: " 
              << simSettings.defaultSpeed << "ms\n";
}

void Application::switchToQuicksort() {
    initializeQuicksortDemo();
    m_currentState = AppState::QUICKSORT_SIMULATION;
    std::cout << "Switched to quicksort simulation\n";
}

void Application::initializeAStarDemo() {
    // Create A* controller and visualizer if they don't exist
    if (!m_astarController) {
        m_astarController = std::make_unique<AStarController>();
    }
    if (!m_astarVisualizer) {
        m_astarVisualizer = std::make_unique<AStarVisualizer>();
        m_astarVisualizer->initialize(m_window);
        m_astarVisualizer->setAudioManager(m_audioManager.get());
    }
    
    // Connect controller and visualizer
    m_astarVisualizer->setController(m_astarController.get());
    
    // Initialize with a larger demo grid (30x20 with some obstacles)
    int gridWidth = 30;
    int gridHeight = 20;
    int startX = 2, startY = 2;
    int goalX = 27, goalY = 17;
    
    m_astarController->initialize(gridWidth, gridHeight, startX, startY, goalX, goalY);
    
    // Add some walls to make it interesting
    // Vertical wall
    for (int y = 3; y < 16; ++y) {
        m_astarController->setWall(12, y);
    }
    // Horizontal wall
    for (int x = 15; x < 25; ++x) {
        m_astarController->setWall(x, 10);
    }
    // L-shaped obstacle
    for (int x = 6; x < 10; ++x) {
        m_astarController->setWall(x, 6);
    }
    for (int y = 6; y < 10; ++y) {
        m_astarController->setWall(9, y);
    }
    // Some scattered obstacles
    m_astarController->setWall(20, 5);
    m_astarController->setWall(21, 5);
    m_astarController->setWall(22, 5);
    m_astarController->setWall(5, 15);
    m_astarController->setWall(6, 15);
    
    std::cout << "A* demo initialized with " << gridWidth << "x" << gridHeight << " grid\n";
}

void Application::switchToAStar() {
    initializeAStarDemo();
    m_currentState = AppState::ASTAR_SIMULATION;
    std::cout << "Switched to A* pathfinding simulation\n";
}

void Application::switchToMenu() {
    m_currentState = AppState::MENU;
    std::cout << "Switched to main menu\n";
}

void Application::exitApplication() {
    m_running = false;
    std::cout << "Exiting application\n";
}

void Application::applyConfiguration() {
    const auto& displaySettings = m_configManager->getDisplaySettings();
    
    // Create window with configured settings
    std::uint32_t style = sf::Style::Titlebar | sf::Style::Close;
    sf::State state = displaySettings.fullscreen ? sf::State::Fullscreen : sf::State::Windowed;
    
    m_window.create(
        sf::VideoMode({static_cast<unsigned int>(displaySettings.windowWidth), 
                      static_cast<unsigned int>(displaySettings.windowHeight)}), 
        "Algorithm Simulation App - Fallout Terminal",
        style,
        state
    );
    
    // Apply other display settings
    if (displaySettings.vsync) {
        m_window.setVerticalSyncEnabled(true);
    } else {
        m_window.setFramerateLimit(displaySettings.fpsLimit);
    }
    
    std::cout << "Configuration applied: " << displaySettings.windowWidth 
              << "x" << displaySettings.windowHeight << std::endl;
}
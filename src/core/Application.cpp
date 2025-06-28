#include "core/Application.h"
#include <iostream>

Application::Application() : m_running(false) {}

Application::~Application() {
    shutdown();
}

bool Application::initialize() {
    m_window.create(sf::VideoMode({1200, 800}), "Algorithm Simulation App - Fallout Terminal");
    m_window.setFramerateLimit(60);
    m_running = true;
    
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
        
        if (auto keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                m_running = false;
            }
        }
    }
}

void Application::update(float deltaTime) {
    // Update logic will go here
}

void Application::render() {
    sf::Color backgroundColor(0, 17, 0); // Dark green background
    m_window.clear(backgroundColor);
    
    // Rendering will go here
    
    m_window.display();
}
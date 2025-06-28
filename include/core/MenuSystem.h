#pragma once
#include <SFML/Graphics.hpp>

class MenuSystem {
public:
    MenuSystem();
    ~MenuSystem();
    
    void handleEvent(const sf::Event* event);
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    
private:
    // Menu state and items will be added here
};
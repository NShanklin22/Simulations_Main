#pragma once
#include <SFML/Graphics.hpp>

class ThemeManager {
public:
    ThemeManager();
    ~ThemeManager();
    
    sf::Color getPrimaryColor() const;
    sf::Color getSecondaryColor() const;
    sf::Color getBackgroundColor() const;
    sf::Color getInactiveColor() const;
    
private:
    sf::Color m_primaryColor;
    sf::Color m_secondaryColor;
    sf::Color m_backgroundColor;
    sf::Color m_inactiveColor;
};
#include "core/ThemeManager.h"

ThemeManager::ThemeManager() :
    m_primaryColor(0, 255, 65),     // Bright green #00FF41
    m_secondaryColor(255, 165, 0),  // Amber #FFA500
    m_backgroundColor(0, 17, 0),    // Dark green/black #001100
    m_inactiveColor(0, 102, 0)      // Dim green #006600
{
    // Theme initialized with Fallout terminal colors
}

ThemeManager::~ThemeManager() {
    // Cleanup
}

sf::Color ThemeManager::getPrimaryColor() const {
    return m_primaryColor;
}

sf::Color ThemeManager::getSecondaryColor() const {
    return m_secondaryColor;
}

sf::Color ThemeManager::getBackgroundColor() const {
    return m_backgroundColor;
}

sf::Color ThemeManager::getInactiveColor() const {
    return m_inactiveColor;
}
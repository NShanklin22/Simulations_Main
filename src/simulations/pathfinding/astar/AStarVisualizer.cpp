#include "simulations/pathfinding/astar/AStarVisualizer.h"
#include "core/AudioManager.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>

AStarVisualizer::AStarVisualizer() 
    : m_controller(nullptr)
    , m_audioManager(nullptr)
    , m_fontLoaded(false)
    , m_selectedControlIndex(0)
    , m_controlsActive(true)
    , m_primaryColor(0, 255, 65)        // Bright Fallout green
    , m_secondaryColor(65, 255, 100)    // Brighter green variant
    , m_backgroundColor(0, 17, 0)       // Dark green/black
    , m_wallColor(48, 48, 48)           // Dark gray with green tint
    , m_startColor(0, 255, 100)         // Bright green for start
    , m_goalColor(255, 255, 0)          // Fallout amber for goal
    , m_openListColor(150, 255, 50)     // Light green (frontier)
    , m_closedListColor(0, 150, 30)     // Dark green (explored)
    , m_pathColor(255, 255, 0)          // Bright amber (final path)
    , m_currentColor(255, 255, 150)     // Bright amber (current cell)
    , m_inactiveColor(0, 102, 0)        // Dim green
    , m_gridAreaX(50.0f)
    , m_gridAreaY(100.0f)
    , m_gridAreaWidth(900.0f)
    , m_gridAreaHeight(400.0f)
    , m_cellSize(20.0f)
    , m_gridOffsetX(0.0f)
    , m_gridOffsetY(0.0f)
    , m_stepCount(0)
    , m_openListSize(0)
    , m_closedListSize(0)
    , m_currentSpeed(1000.0f)
    , m_editMode(EditMode::NONE)
    , m_hoveredGridX(-1)
    , m_hoveredGridY(-1)
    , m_mousePressed(false)
    , m_viewInitialized(false)
{
}

AStarVisualizer::~AStarVisualizer() {
}

void AStarVisualizer::initialize(sf::RenderWindow& window) {
    // Try to load fonts with fallback
    m_fontLoaded = m_font.openFromFile("assets/fonts/CourierNew.ttf");
    if (!m_fontLoaded) {
        m_fontLoaded = m_font.openFromFile("C:/Windows/Fonts/consola.ttf");
        if (!m_fontLoaded) {
            m_fontLoaded = m_font.openFromFile("C:/Windows/Fonts/arial.ttf");
            if (!m_fontLoaded) {
                std::cout << "Warning: Could not load any font for AStarVisualizer\n";
            }
        }
    }
    
    // Initialize control buttons
    initializeControls();
    
    // Set up grid view
    resetView();
}

void AStarVisualizer::setController(AStarController* controller) {
    m_controller = controller;
    
    if (m_controller) {
        // Set up callback to receive step updates
        m_controller->setStepCallback([this](const AStarStep& step) {
            onAStarStep(step);
        });
        
        updateGridDisplay();
    }
}

void AStarVisualizer::setAudioManager(AudioManager* audioManager) {
    m_audioManager = audioManager;
}

void AStarVisualizer::handleEvent(const sf::Event* event) {
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
            case sf::Keyboard::Key::W:
                m_editMode = (m_editMode == EditMode::PLACE_WALL) ? EditMode::NONE : EditMode::PLACE_WALL;
                break;
            case sf::Keyboard::Key::S:
                m_editMode = (m_editMode == EditMode::PLACE_START) ? EditMode::NONE : EditMode::PLACE_START;
                break;
            case sf::Keyboard::Key::G:
                m_editMode = (m_editMode == EditMode::PLACE_GOAL) ? EditMode::NONE : EditMode::PLACE_GOAL;
                break;
            case sf::Keyboard::Key::C:
                m_controller->clearGrid();
                break;
            case sf::Keyboard::Key::R:
                m_controller->generateRandomMaze(0.3f);
                break;
            case sf::Keyboard::Key::Escape:
                m_editMode = EditMode::NONE;
                break;
            default:
                break;
        }
    }
    else if (auto mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            m_mousePressed = true;
            handleMouseClick(mousePressed->position.x, mousePressed->position.y);
        }
    }
    else if (auto mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
        if (mouseReleased->button == sf::Mouse::Button::Left) {
            m_mousePressed = false;
        }
    }
    else if (auto mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
        handleMouseMove(mouseMoved->position.x, mouseMoved->position.y);
    }
}

void AStarVisualizer::update(float deltaTime) {
    // Update controller (algorithm stepping)
    if (m_controller) {
        m_controller->update(deltaTime);
    }
}

void AStarVisualizer::render(sf::RenderWindow& window) {
    drawGrid(window);
    drawInfo(window);
    drawControls(window);
}

void AStarVisualizer::drawGrid(sf::RenderWindow& window) {
    if (!m_controller) return;
    
    const auto& grid = m_controller->getCurrentGrid();
    const auto& step = m_controller->getCurrentStep();
    
    if (grid.empty()) return;
    
    // Calculate cell size based on grid dimensions and available space
    float cellWidth = m_gridAreaWidth / grid[0].size();
    float cellHeight = m_gridAreaHeight / grid.size();
    m_cellSize = std::min(cellWidth, cellHeight);
    m_cellSize = std::min(m_cellSize, 30.0f); // Cap the cell size
    
    // Center the grid
    float totalWidth = grid[0].size() * m_cellSize;
    float totalHeight = grid.size() * m_cellSize;
    m_gridOffsetX = m_gridAreaX + (m_gridAreaWidth - totalWidth) / 2.0f;
    m_gridOffsetY = m_gridAreaY + (m_gridAreaHeight - totalHeight) / 2.0f;
    
    // Draw grid background
    sf::RectangleShape gridBg;
    gridBg.setPosition(sf::Vector2f(m_gridOffsetX - 5, m_gridOffsetY - 5));
    gridBg.setSize(sf::Vector2f(totalWidth + 10, totalHeight + 10));
    gridBg.setFillColor(sf::Color(0, 0, 0, 128));
    gridBg.setOutlineThickness(2.0f);
    gridBg.setOutlineColor(m_primaryColor);
    window.draw(gridBg);
    
    // Draw grid cells
    for (size_t y = 0; y < grid.size(); ++y) {
        for (size_t x = 0; x < grid[y].size(); ++x) {
            sf::RectangleShape cell;
            
            float posX = m_gridOffsetX + x * m_cellSize;
            float posY = m_gridOffsetY + y * m_cellSize;
            
            cell.setPosition(sf::Vector2f(posX, posY));
            cell.setSize(sf::Vector2f(m_cellSize - 1.0f, m_cellSize - 1.0f)); // Small gap between cells
            cell.setFillColor(getCellColor(grid[y][x]));
            
            // Add subtle border
            cell.setOutlineThickness(0.5f);
            cell.setOutlineColor(sf::Color(0, 50, 0));
            
            // Highlight hovered cell
            if (m_hoveredGridX == static_cast<int>(x) && m_hoveredGridY == static_cast<int>(y)) {
                sf::Color hoverColor = cell.getFillColor();
                hoverColor.a = 200;
                cell.setFillColor(hoverColor);
                cell.setOutlineColor(m_secondaryColor);
                cell.setOutlineThickness(2.0f);
            }
            
            window.draw(cell);
            
            // Highlight current cell being examined
            if (step.hasCurrentCell && step.currentX == static_cast<int>(x) && step.currentY == static_cast<int>(y)) {
                sf::RectangleShape highlight;
                highlight.setPosition(sf::Vector2f(posX - 2, posY - 2));
                highlight.setSize(sf::Vector2f(m_cellSize + 3, m_cellSize + 3));
                highlight.setFillColor(sf::Color::Transparent);
                highlight.setOutlineThickness(3.0f);
                highlight.setOutlineColor(m_currentColor);
                window.draw(highlight);
            }
            
            // Path visualization is now handled separately
        }
    }
    
    // Draw current path if we're examining a cell during search
    if (step.hasCurrentCell) {
        drawCurrentPath(window, step.currentX, step.currentY);
    }
    
    // Draw final path if complete
    if (!step.path.empty()) {
        drawPath(window, step.path, m_pathColor, 4.0f);
    }
    
    // Draw edit mode indicator
    if (m_editMode != EditMode::NONE) {
        std::string modeText;
        sf::Color modeColor;
        
        switch (m_editMode) {
            case EditMode::PLACE_WALL:
                modeText = "WALL PLACEMENT MODE (W to toggle, Click to place/remove)";
                modeColor = m_wallColor;
                break;
            case EditMode::PLACE_START:
                modeText = "START PLACEMENT MODE (S to toggle, Click to place)";
                modeColor = m_startColor;
                break;
            case EditMode::PLACE_GOAL:
                modeText = "GOAL PLACEMENT MODE (G to toggle, Click to place)";
                modeColor = m_goalColor;
                break;
            default:
                break;
        }
        
        if (m_fontLoaded && !modeText.empty()) {
            sf::Text modeIndicator(m_font, modeText, 16);
            modeIndicator.setFillColor(modeColor);
            modeIndicator.setPosition({m_gridOffsetX, m_gridOffsetY - 25});
            window.draw(modeIndicator);
        }
    }
}

void AStarVisualizer::drawInfo(sf::RenderWindow& window) {
    if (!m_controller || !m_fontLoaded) return;
    
    // Top line - Status and current operation
    std::stringstream topInfo;
    topInfo << "A* PATHFINDING ALGORITHM | ";
    
    switch (m_controller->getState()) {
        case AStarState::READY:
            topInfo << "STATUS: READY";
            break;
        case AStarState::SEARCHING:
            topInfo << "STATUS: SEARCHING";
            break;
        case AStarState::PAUSED:
            topInfo << "STATUS: PAUSED";
            break;
        case AStarState::PATH_FOUND:
            topInfo << "STATUS: PATH FOUND";
            break;
        case AStarState::NO_PATH_EXISTS:
            topInfo << "STATUS: NO PATH EXISTS";
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
    statsInfo << "OPEN LIST: " << m_controller->getOpenListSize() << " | ";
    statsInfo << "CLOSED LIST: " << m_controller->getClosedListSize();
    
    sf::Text statsText(m_font, statsInfo.str(), 16);
    statsText.setFillColor(m_inactiveColor);
    statsText.setPosition({50.0f, 45.0f});
    window.draw(statsText);
    
    // Legend
    if (m_fontLoaded) {
        float legendY = 100.0f;
        sf::Text legendTitle(m_font, "LEGEND:", 14);
        legendTitle.setFillColor(m_primaryColor);
        legendTitle.setPosition({970.0f, legendY});
        window.draw(legendTitle);
        
        // Legend items
        std::vector<std::pair<sf::Color, std::string>> legendItems = {
            {m_startColor, "START"},
            {m_goalColor, "GOAL"},
            {m_wallColor, "WALL"},
            {m_openListColor, "TO EXPLORE"},
            {m_closedListColor, "EXPLORED"},
            {m_pathColor, "FINAL PATH"},
            {m_currentColor, "CURRENT"}
        };
        
        for (size_t i = 0; i < legendItems.size(); ++i) {
            float itemY = legendY + 25 + i * 20;
            
            // Color square
            sf::RectangleShape colorSquare;
            colorSquare.setPosition(sf::Vector2f(970.0f, itemY));
            colorSquare.setSize(sf::Vector2f(12.0f, 12.0f));
            colorSquare.setFillColor(legendItems[i].first);
            window.draw(colorSquare);
            
            // Label
            sf::Text label(m_font, legendItems[i].second, 12);
            label.setFillColor(m_inactiveColor);
            label.setPosition({990.0f, itemY - 2});
            window.draw(label);
        }
    }
}

void AStarVisualizer::drawControls(sf::RenderWindow& window) {
    if (!m_fontLoaded) return;
    
    // Draw instruction text
    sf::Text instructionText(m_font, "CONTROLS: LEFT/RIGHT to navigate, ENTER to select | W: Wall mode, S: Start mode, G: Goal mode, C: Clear grid, R: Random maze", 12);
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

void AStarVisualizer::onAStarStep(const AStarStep& step) {
    // Update statistics
    m_stepCount = step.stepCount;
    m_openListSize = step.openListSize;
    m_closedListSize = step.closedListSize;
    
    // Play audio based on step type
    if (m_audioManager) {
        if (step.description.find("Examining cell") != std::string::npos) {
            // Play a tone when examining a cell
            m_audioManager->playSound(SoundType::COMPARISON, 1.0f, 0.5f);
        } else if (step.description.find("Added start") != std::string::npos) {
            // Play start sound
            m_audioManager->playSound(SoundType::PIVOT_SELECT, 1.0f, 0.7f);
        } else if (step.description.find("Path found!") != std::string::npos) {
            // Play success sound
            m_audioManager->playSound(SoundType::ALGORITHM_COMPLETE, 1.0f, 1.0f);
        } else if (step.description.find("No path exists") != std::string::npos) {
            // Play failure sound
            m_audioManager->playSound(SoundType::SWAP, 0.5f, 0.8f);
        } else if (step.description.find("Building optimal path") != std::string::npos) {
            // Play path building sound
            m_audioManager->playSound(SoundType::MENU_SELECT, 1.2f, 0.6f);
        }
    }
}

sf::Color AStarVisualizer::getCellColor(const GridCell& cell) {
    switch (cell.type) {
        case CellType::EMPTY:
            return m_backgroundColor;
        case CellType::WALL:
            return m_wallColor;
        case CellType::START:
            return m_startColor;
        case CellType::GOAL:
            return m_goalColor;
        case CellType::OPEN_LIST:
            return m_openListColor;
        case CellType::CLOSED_LIST:
            return m_closedListColor;
        case CellType::PATH:
            return m_pathColor;
        default:
            return m_inactiveColor;
    }
}

void AStarVisualizer::updateGridDisplay() {
    // This method can be used for any grid-specific updates
    if (m_controller) {
        m_currentSpeed = m_controller->getSpeed();
    }
}

void AStarVisualizer::initializeControls() {
    m_controlButtons.clear();
    
    // Add control buttons
    m_controlButtons.push_back({"PLAY/PAUSE", [this]() {
        if (m_controller->getState() == AStarState::READY || 
            m_controller->getState() == AStarState::PAUSED) {
            m_controller->start();
        } else if (m_controller->getState() == AStarState::SEARCHING) {
            m_controller->pause();
        }
    }});
    
    m_controlButtons.push_back({"STEP FORWARD", [this]() {
        if (m_controller->getState() == AStarState::READY ||
            m_controller->getState() == AStarState::PAUSED) {
            m_controller->step();
        }
    }});
    
    m_controlButtons.push_back({"STEP BACK", [this]() {
        if (m_controller->getState() == AStarState::PAUSED ||
            m_controller->getState() == AStarState::PATH_FOUND ||
            m_controller->getState() == AStarState::NO_PATH_EXISTS) {
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
        updateGridDisplay();
        m_stepCount = 0;
        m_openListSize = 0;
        m_closedListSize = 0;
    }});
    
    m_controlButtons.push_back({"FAST FORWARD", [this]() {
        m_controller->fastForward();
    }});
}

void AStarVisualizer::navigateControlsLeft() {
    if (m_selectedControlIndex > 0) {
        m_selectedControlIndex--;
    } else {
        m_selectedControlIndex = static_cast<int>(m_controlButtons.size()) - 1;
    }
    
    // Play button hover sound
    if (m_audioManager) {
        m_audioManager->playSound(SoundType::MENU_NAVIGATE, 1.1f, 0.5f);
    }
}

void AStarVisualizer::navigateControlsRight() {
    if (m_selectedControlIndex < static_cast<int>(m_controlButtons.size()) - 1) {
        m_selectedControlIndex++;
    } else {
        m_selectedControlIndex = 0;
    }
    
    // Play button hover sound
    if (m_audioManager) {
        m_audioManager->playSound(SoundType::MENU_NAVIGATE, 1.1f, 0.5f);
    }
}

void AStarVisualizer::selectControl() {
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

void AStarVisualizer::renderControlButton(sf::RenderWindow& window, const ControlButton& button, float x, float y, bool selected) {
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

void AStarVisualizer::handleMouseClick(int mouseX, int mouseY) {
    if (!m_controller || m_controller->getState() != AStarState::READY) return;
    
    auto [gridX, gridY] = screenToGrid(mouseX, mouseY);
    if (!isValidGridPos(gridX, gridY)) return;
    
    switch (m_editMode) {
        case EditMode::PLACE_WALL:
            m_controller->toggleWall(gridX, gridY);
            break;
        case EditMode::PLACE_START:
            m_controller->setStart(gridX, gridY);
            m_editMode = EditMode::NONE; // Exit mode after placing
            break;
        case EditMode::PLACE_GOAL:
            m_controller->setGoal(gridX, gridY);
            m_editMode = EditMode::NONE; // Exit mode after placing
            break;
        default:
            break;
    }
}

void AStarVisualizer::handleMouseMove(int mouseX, int mouseY) {
    auto [gridX, gridY] = screenToGrid(mouseX, mouseY);
    
    if (isValidGridPos(gridX, gridY)) {
        m_hoveredGridX = gridX;
        m_hoveredGridY = gridY;
        
        // Handle wall painting while dragging
        if (m_mousePressed && m_editMode == EditMode::PLACE_WALL && 
            m_controller && m_controller->getState() == AStarState::READY) {
            m_controller->setWall(gridX, gridY);
        }
    } else {
        m_hoveredGridX = -1;
        m_hoveredGridY = -1;
    }
}

std::pair<int, int> AStarVisualizer::screenToGrid(int screenX, int screenY) {
    if (!m_controller || m_cellSize <= 0) return {-1, -1};
    
    int gridX = static_cast<int>((screenX - m_gridOffsetX) / m_cellSize);
    int gridY = static_cast<int>((screenY - m_gridOffsetY) / m_cellSize);
    
    return {gridX, gridY};
}

bool AStarVisualizer::isValidGridPos(int gridX, int gridY) {
    if (!m_controller) return false;
    
    return gridX >= 0 && gridX < m_controller->getGridWidth() && 
           gridY >= 0 && gridY < m_controller->getGridHeight();
}

void AStarVisualizer::resetView() {
    // This can be used to reset any view transformations if needed
    m_viewInitialized = true;
}

void AStarVisualizer::drawPath(sf::RenderWindow& window, const std::vector<std::pair<int, int>>& path, sf::Color color, float thickness) {
    if (path.size() < 2) return;
    
    for (size_t i = 1; i < path.size(); ++i) {
        float x1 = m_gridOffsetX + path[i-1].first * m_cellSize + m_cellSize * 0.5f;
        float y1 = m_gridOffsetY + path[i-1].second * m_cellSize + m_cellSize * 0.5f;
        float x2 = m_gridOffsetX + path[i].first * m_cellSize + m_cellSize * 0.5f;
        float y2 = m_gridOffsetY + path[i].second * m_cellSize + m_cellSize * 0.5f;
        
        // Calculate the direction vector
        float dx = x2 - x1;
        float dy = y2 - y1;
        float length = std::sqrt(dx * dx + dy * dy);
        
        if (length > 0) {
            // Normalize
            dx /= length;
            dy /= length;
            
            // Perpendicular vector for line width
            float px = -dy * thickness * 0.5f;
            float py = dx * thickness * 0.5f;
            
            // Create a quad for the line segment
            sf::VertexArray line(sf::Triangles, 6);
            
            // First triangle
            line[0] = sf::Vertex(sf::Vector2f(x1 + px, y1 + py), color);
            line[1] = sf::Vertex(sf::Vector2f(x1 - px, y1 - py), color);
            line[2] = sf::Vertex(sf::Vector2f(x2 + px, y2 + py), color);
            
            // Second triangle
            line[3] = sf::Vertex(sf::Vector2f(x1 - px, y1 - py), color);
            line[4] = sf::Vertex(sf::Vector2f(x2 - px, y2 - py), color);
            line[5] = sf::Vertex(sf::Vector2f(x2 + px, y2 + py), color);
            
            window.draw(line);
            
            // Draw end caps (circles)
            if (i == 1) { // Start cap
                sf::CircleShape startCap(thickness * 0.5f);
                startCap.setPosition(x1 - thickness * 0.5f, y1 - thickness * 0.5f);
                startCap.setFillColor(color);
                window.draw(startCap);
            }
            if (i == path.size() - 1) { // End cap
                sf::CircleShape endCap(thickness * 0.5f);
                endCap.setPosition(x2 - thickness * 0.5f, y2 - thickness * 0.5f);
                endCap.setFillColor(color);
                window.draw(endCap);
            }
        }
    }
}

void AStarVisualizer::drawCurrentPath(sf::RenderWindow& window, int currentX, int currentY) {
    if (!m_controller || m_steps.empty()) return;
    
    // Get the current step's grid to access parent information
    const auto& currentGrid = m_controller->getCurrentStep().grid;
    if (currentY >= static_cast<int>(currentGrid.size()) || 
        currentX >= static_cast<int>(currentGrid[currentY].size())) return;
    
    // Trace back from current cell to start to show potential path
    std::vector<std::pair<int, int>> currentPath;
    int x = currentX;
    int y = currentY;
    
    while (x != -1 && y != -1 && currentGrid[y][x].hasParent) {
        currentPath.push_back({x, y});
        int parentX = currentGrid[y][x].parentX;
        int parentY = currentGrid[y][x].parentY;
        
        // Prevent infinite loops
        if (parentX == x && parentY == y) break;
        
        x = parentX;
        y = parentY;
        
        // Stop at start
        if (x == m_controller->getStartX() && y == m_controller->getStartY()) {
            currentPath.push_back({x, y});
            break;
        }
        
        // Prevent extremely long paths (safety)
        if (currentPath.size() > 1000) break;
    }
    
    // Reverse to get path from start to current
    std::reverse(currentPath.begin(), currentPath.end());
    
    if (currentPath.size() >= 2) {
        // Draw the potential path in a dimmer amber
        sf::Color currentPathColor = m_currentColor;
        currentPathColor.a = 150; // Semi-transparent
        drawPath(window, currentPath, currentPathColor, 2.0f);
    }
}
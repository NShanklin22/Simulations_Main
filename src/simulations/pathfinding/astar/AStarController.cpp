#include "simulations/pathfinding/astar/AStarController.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <unordered_map>
#include <limits>
#include <queue>

AStarController::AStarController() 
    : m_state(AStarState::READY)
    , m_currentStepIndex(0)
    , m_stepDelay(1000.0f)  // 1 second per step initially
    , m_timeSinceLastStep(0.0f)
    , m_gridWidth(0)
    , m_gridHeight(0)
    , m_startX(0)
    , m_startY(0)
    , m_goalX(0)
    , m_goalY(0)
    , m_totalSteps(0)
    , m_currentOpenListSize(0)
    , m_currentClosedListSize(0)
{
}

AStarController::~AStarController() {
}

void AStarController::initialize(int gridWidth, int gridHeight, int startX, int startY, int goalX, int goalY) {
    m_gridWidth = gridWidth;
    m_gridHeight = gridHeight;
    m_startX = startX;
    m_startY = startY;
    m_goalX = goalX;
    m_goalY = goalY;
    
    // Initialize grid
    m_originalGrid.clear();
    m_originalGrid.resize(gridHeight);
    for (int y = 0; y < gridHeight; ++y) {
        m_originalGrid[y].resize(gridWidth);
        for (int x = 0; x < gridWidth; ++x) {
            m_originalGrid[y][x] = GridCell(x, y);
        }
    }
    
    // Set start and goal
    m_originalGrid[startY][startX].type = CellType::START;
    m_originalGrid[goalY][goalX].type = CellType::GOAL;
    
    m_currentGrid = m_originalGrid;
    m_state = AStarState::READY;
    m_currentStepIndex = 0;
    m_timeSinceLastStep = 0.0f;
    m_totalSteps = 0;
    
    generateSteps();
    
    if (!m_steps.empty()) {
        m_currentStep = m_steps[0];
        if (m_stepCallback) {
            m_stepCallback(m_currentStep);
        }
    }
}

void AStarController::setWall(int x, int y) {
    if (x >= 0 && x < m_gridWidth && y >= 0 && y < m_gridHeight &&
        !(x == m_startX && y == m_startY) && !(x == m_goalX && y == m_goalY)) {
        m_originalGrid[y][x].type = CellType::WALL;
        m_currentGrid[y][x].type = CellType::WALL;
        // Regenerate steps if we're in ready state
        if (m_state == AStarState::READY) {
            generateSteps();
            if (!m_steps.empty()) {
                m_currentStep = m_steps[0];
                if (m_stepCallback) {
                    m_stepCallback(m_currentStep);
                }
            }
        }
    }
}

void AStarController::clearWall(int x, int y) {
    if (x >= 0 && x < m_gridWidth && y >= 0 && y < m_gridHeight &&
        !(x == m_startX && y == m_startY) && !(x == m_goalX && y == m_goalY)) {
        m_originalGrid[y][x].type = CellType::EMPTY;
        m_currentGrid[y][x].type = CellType::EMPTY;
        // Regenerate steps if we're in ready state
        if (m_state == AStarState::READY) {
            generateSteps();
            if (!m_steps.empty()) {
                m_currentStep = m_steps[0];
                if (m_stepCallback) {
                    m_stepCallback(m_currentStep);
                }
            }
        }
    }
}

void AStarController::toggleWall(int x, int y) {
    if (x >= 0 && x < m_gridWidth && y >= 0 && y < m_gridHeight &&
        !(x == m_startX && y == m_startY) && !(x == m_goalX && y == m_goalY)) {
        if (m_originalGrid[y][x].type == CellType::WALL) {
            clearWall(x, y);
        } else {
            setWall(x, y);
        }
    }
}

void AStarController::setStart(int x, int y) {
    if (x >= 0 && x < m_gridWidth && y >= 0 && y < m_gridHeight &&
        m_originalGrid[y][x].type != CellType::WALL && !(x == m_goalX && y == m_goalY)) {
        // Clear old start
        m_originalGrid[m_startY][m_startX].type = CellType::EMPTY;
        m_currentGrid[m_startY][m_startX].type = CellType::EMPTY;
        // Set new start
        m_startX = x;
        m_startY = y;
        m_originalGrid[y][x].type = CellType::START;
        m_currentGrid[y][x].type = CellType::START;
        // Regenerate steps
        if (m_state == AStarState::READY) {
            generateSteps();
            if (!m_steps.empty()) {
                m_currentStep = m_steps[0];
                if (m_stepCallback) {
                    m_stepCallback(m_currentStep);
                }
            }
        }
    }
}

void AStarController::setGoal(int x, int y) {
    if (x >= 0 && x < m_gridWidth && y >= 0 && y < m_gridHeight &&
        m_originalGrid[y][x].type != CellType::WALL && !(x == m_startX && y == m_startY)) {
        // Clear old goal
        m_originalGrid[m_goalY][m_goalX].type = CellType::EMPTY;
        m_currentGrid[m_goalY][m_goalX].type = CellType::EMPTY;
        // Set new goal
        m_goalX = x;
        m_goalY = y;
        m_originalGrid[y][x].type = CellType::GOAL;
        m_currentGrid[y][x].type = CellType::GOAL;
        // Regenerate steps
        if (m_state == AStarState::READY) {
            generateSteps();
            if (!m_steps.empty()) {
                m_currentStep = m_steps[0];
                if (m_stepCallback) {
                    m_stepCallback(m_currentStep);
                }
            }
        }
    }
}

void AStarController::clearGrid() {
    for (int y = 0; y < m_gridHeight; ++y) {
        for (int x = 0; x < m_gridWidth; ++x) {
            if (!(x == m_startX && y == m_startY) && !(x == m_goalX && y == m_goalY)) {
                m_originalGrid[y][x].type = CellType::EMPTY;
                m_currentGrid[y][x].type = CellType::EMPTY;
            }
        }
    }
    if (m_state == AStarState::READY) {
        generateSteps();
        if (!m_steps.empty()) {
            m_currentStep = m_steps[0];
            if (m_stepCallback) {
                m_stepCallback(m_currentStep);
            }
        }
    }
}

void AStarController::generateRandomMaze(float wallDensity) {
    clearGrid();
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    for (int y = 0; y < m_gridHeight; ++y) {
        for (int x = 0; x < m_gridWidth; ++x) {
            if (!(x == m_startX && y == m_startY) && !(x == m_goalX && y == m_goalY)) {
                if (static_cast<float>(std::rand()) / RAND_MAX < wallDensity) {
                    m_originalGrid[y][x].type = CellType::WALL;
                    m_currentGrid[y][x].type = CellType::WALL;
                }
            }
        }
    }
    
    if (m_state == AStarState::READY) {
        generateSteps();
        if (!m_steps.empty()) {
            m_currentStep = m_steps[0];
            if (m_stepCallback) {
                m_stepCallback(m_currentStep);
            }
        }
    }
}

void AStarController::start() {
    if (m_state == AStarState::READY || m_state == AStarState::PAUSED) {
        m_state = AStarState::SEARCHING;
    }
}

void AStarController::pause() {
    if (m_state == AStarState::SEARCHING) {
        m_state = AStarState::PAUSED;
    }
}

void AStarController::resume() {
    if (m_state == AStarState::PAUSED) {
        m_state = AStarState::SEARCHING;
    }
}

void AStarController::reset() {
    m_currentGrid = m_originalGrid;
    m_state = AStarState::READY;
    m_currentStepIndex = 0;
    m_timeSinceLastStep = 0.0f;
    
    generateSteps();
    
    if (!m_steps.empty()) {
        m_currentStep = m_steps[0];
        if (m_stepCallback) {
            m_stepCallback(m_currentStep);
        }
    }
}

void AStarController::step() {
    if (m_currentStepIndex < m_steps.size() - 1) {
        m_currentStepIndex++;
        m_currentStep = m_steps[m_currentStepIndex];
        m_currentGrid = m_currentStep.grid;
        
        if (m_stepCallback) {
            m_stepCallback(m_currentStep);
        }
        
        if (m_currentStepIndex >= m_steps.size() - 1) {
            if (m_currentStep.description.find("Path found!") != std::string::npos) {
                m_state = AStarState::PATH_FOUND;
            } else {
                m_state = AStarState::NO_PATH_EXISTS;
            }
        }
    }
}

void AStarController::stepBack() {
    if (m_currentStepIndex > 0) {
        m_currentStepIndex--;
        m_currentStep = m_steps[m_currentStepIndex];
        m_currentGrid = m_currentStep.grid;
        
        if (m_stepCallback) {
            m_stepCallback(m_currentStep);
        }
    }
}

void AStarController::fastForward() {
    if (!m_steps.empty()) {
        m_currentStepIndex = m_steps.size() - 1;
        m_currentStep = m_steps[m_currentStepIndex];
        m_currentGrid = m_currentStep.grid;
        
        if (m_currentStep.description.find("Path found!") != std::string::npos) {
            m_state = AStarState::PATH_FOUND;
        } else {
            m_state = AStarState::NO_PATH_EXISTS;
        }
        
        if (m_stepCallback) {
            m_stepCallback(m_currentStep);
        }
    }
}

void AStarController::setSpeed(float delayMs) {
    m_stepDelay = delayMs;
}

void AStarController::slowDown() {
    m_stepDelay = std::min(5000.0f, m_stepDelay + 200.0f);
}

void AStarController::speedUp() {
    m_stepDelay = std::max(50.0f, m_stepDelay - 200.0f);
}

void AStarController::setStepCallback(std::function<void(const AStarStep&)> callback) {
    m_stepCallback = callback;
}

void AStarController::update(float deltaTime) {
    if (m_state == AStarState::SEARCHING) {
        m_timeSinceLastStep += deltaTime * 1000.0f;
        
        if (m_timeSinceLastStep >= m_stepDelay) {
            step();
            m_timeSinceLastStep = 0.0f;
        }
    }
}

float AStarController::calculateHeuristic(int x1, int y1, int x2, int y2) {
    // Manhattan distance for grid-based pathfinding
    return static_cast<float>(std::abs(x1 - x2) + std::abs(y1 - y2));
}

void AStarController::generateSteps() {
    m_steps.clear();
    m_totalSteps = 0;
    
    // Initial state
    addStep(m_originalGrid, -1, -1, "Starting A* pathfinding algorithm");
    
    // Run A* algorithm
    runAStar();
}

void AStarController::runAStar() {
    // Create a working copy of the grid
    std::vector<std::vector<GridCell>> workingGrid = m_originalGrid;
    
    // Reset all costs and parent information
    for (int y = 0; y < m_gridHeight; ++y) {
        for (int x = 0; x < m_gridWidth; ++x) {
            workingGrid[y][x].gCost = std::numeric_limits<float>::infinity();
            workingGrid[y][x].hCost = 0;
            workingGrid[y][x].fCost = std::numeric_limits<float>::infinity();
            workingGrid[y][x].hasParent = false;
        }
    }
    
    // Priority queue for open list (min-heap based on fCost)
    // We'll store coordinates instead of pointers
    struct Node {
        int x, y;
        float fCost;
        bool operator>(const Node& other) const {
            return fCost > other.fCost;
        }
    };
    
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList;
    std::unordered_map<int, bool> openSet;  // key = y * width + x
    std::unordered_map<int, bool> closedSet;
    
    // Initialize start cell
    workingGrid[m_startY][m_startX].gCost = 0;
    workingGrid[m_startY][m_startX].hCost = calculateHeuristic(m_startX, m_startY, m_goalX, m_goalY);
    workingGrid[m_startY][m_startX].fCost = workingGrid[m_startY][m_startX].hCost;
    
    openList.push({m_startX, m_startY, workingGrid[m_startY][m_startX].fCost});
    openSet[m_startY * m_gridWidth + m_startX] = true;
    
    addStep(workingGrid, m_startX, m_startY, "Added start cell to open list");
    
    while (!openList.empty()) {
        // Get the cell with lowest fCost
        Node current = openList.top();
        openList.pop();
        
        int key = current.y * m_gridWidth + current.x;
        
        // Skip if already processed
        if (closedSet.find(key) != closedSet.end()) {
            continue;
        }
        
        openSet.erase(key);
        closedSet[key] = true;
        
        // Mark as closed (except start/goal)
        if (workingGrid[current.y][current.x].type != CellType::START && 
            workingGrid[current.y][current.x].type != CellType::GOAL) {
            workingGrid[current.y][current.x].type = CellType::CLOSED_LIST;
        }
        
        m_currentOpenListSize = static_cast<int>(openSet.size());
        m_currentClosedListSize = static_cast<int>(closedSet.size());
        
        addStep(workingGrid, current.x, current.y, 
               "Examining cell (" + std::to_string(current.x) + "," + std::to_string(current.y) + 
               ") f=" + std::to_string((int)workingGrid[current.y][current.x].fCost));
        
        // Check if we reached the goal
        if (current.x == m_goalX && current.y == m_goalY) {
            auto path = reconstructPath(current.x, current.y);
            
            // Add path visualization steps
            for (const auto& p : path) {
                if (!(p.first == m_startX && p.second == m_startY) && 
                    !(p.first == m_goalX && p.second == m_goalY)) {
                    workingGrid[p.second][p.first].type = CellType::PATH;
                }
                addStep(workingGrid, p.first, p.second, "Building optimal path...", path);
            }
            
            addStep(workingGrid, -1, -1, "Path found! Total path length: " + std::to_string(path.size() - 1) + " steps", path);
            return;
        }
        
        // Check neighbors
        auto neighbors = getNeighbors(current.x, current.y);
        
        for (const auto& neighbor : neighbors) {
            int nx = neighbor.first;
            int ny = neighbor.second;
            int nkey = ny * m_gridWidth + nx;
            
            if (workingGrid[ny][nx].type == CellType::WALL || closedSet.find(nkey) != closedSet.end()) {
                continue;
            }
            
            float tentativeGCost = workingGrid[current.y][current.x].gCost + 1.0f;
            
            if (tentativeGCost < workingGrid[ny][nx].gCost) {
                // Update parent
                workingGrid[ny][nx].parentX = current.x;
                workingGrid[ny][nx].parentY = current.y;
                workingGrid[ny][nx].hasParent = true;
                workingGrid[ny][nx].gCost = tentativeGCost;
                workingGrid[ny][nx].hCost = calculateHeuristic(nx, ny, m_goalX, m_goalY);
                workingGrid[ny][nx].fCost = tentativeGCost + workingGrid[ny][nx].hCost;
                
                if (openSet.find(nkey) == openSet.end()) {
                    // Mark as open (except start/goal)
                    if (workingGrid[ny][nx].type != CellType::START && 
                        workingGrid[ny][nx].type != CellType::GOAL) {
                        workingGrid[ny][nx].type = CellType::OPEN_LIST;
                    }
                }
                
                openList.push({nx, ny, workingGrid[ny][nx].fCost});
                openSet[nkey] = true;
            }
        }
    }
    
    // No path found
    addStep(workingGrid, -1, -1, "No path exists to the goal!");
}

std::vector<std::pair<int, int>> AStarController::getNeighbors(int x, int y) {
    std::vector<std::pair<int, int>> neighbors;
    
    // Check 4-directional neighbors (up, down, left, right)
    int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    
    for (int i = 0; i < 4; ++i) {
        int newX = x + directions[i][0];
        int newY = y + directions[i][1];
        
        if (newX >= 0 && newX < m_gridWidth && newY >= 0 && newY < m_gridHeight) {
            neighbors.push_back({newX, newY});
        }
    }
    
    return neighbors;
}

std::vector<std::pair<int, int>> AStarController::reconstructPath(int goalX, int goalY) {
    std::vector<std::pair<int, int>> path;
    
    // Get the last step to access the final grid state
    if (m_steps.empty()) return path;
    
    const auto& finalGrid = m_steps.back().grid;
    
    int currentX = goalX;
    int currentY = goalY;
    
    // Trace back from goal to start
    while (currentX != -1 && currentY != -1) {
        path.push_back({currentX, currentY});
        
        if (currentX == m_startX && currentY == m_startY) {
            break;
        }
        
        if (finalGrid[currentY][currentX].hasParent) {
            int parentX = finalGrid[currentY][currentX].parentX;
            int parentY = finalGrid[currentY][currentX].parentY;
            currentX = parentX;
            currentY = parentY;
        } else {
            break;
        }
    }
    
    // Reverse to get path from start to goal
    std::reverse(path.begin(), path.end());
    
    return path;
}

void AStarController::addStep(const std::vector<std::vector<GridCell>>& grid, int currentX, int currentY, const std::string& desc, const std::vector<std::pair<int, int>>& path) {
    AStarStep step;
    step.grid = grid;
    step.currentX = currentX;
    step.currentY = currentY;
    step.hasCurrentCell = (currentX >= 0 && currentY >= 0);
    step.description = desc;
    step.stepCount = m_totalSteps++;
    step.openListSize = m_currentOpenListSize;
    step.closedListSize = m_currentClosedListSize;
    step.path = path;
    
    m_steps.push_back(step);
}

int AStarController::getStepCount() const {
    if (m_currentStepIndex < m_steps.size()) {
        return m_steps[m_currentStepIndex].stepCount;
    }
    return 0;
}

int AStarController::getOpenListSize() const {
    if (m_currentStepIndex < m_steps.size()) {
        return m_steps[m_currentStepIndex].openListSize;
    }
    return 0;
}

int AStarController::getClosedListSize() const {
    if (m_currentStepIndex < m_steps.size()) {
        return m_steps[m_currentStepIndex].closedListSize;
    }
    return 0;
}
#pragma once
#include <vector>
#include <functional>
#include <string>
#include <queue>
#include <unordered_set>

enum class AStarState {
    READY,
    SEARCHING,
    PAUSED,
    PATH_FOUND,
    NO_PATH_EXISTS
};

enum class CellType {
    EMPTY,
    WALL,
    START,
    GOAL,
    OPEN_LIST,
    CLOSED_LIST,
    PATH
};

struct GridCell {
    int x, y;
    CellType type;
    float gCost;    // Distance from start
    float hCost;    // Heuristic distance to goal
    float fCost;    // gCost + hCost
    int parentX, parentY;  // Store parent coordinates instead of pointer
    bool hasParent;
    
    GridCell() : x(0), y(0), type(CellType::EMPTY), gCost(0), hCost(0), fCost(0), parentX(-1), parentY(-1), hasParent(false) {}
    GridCell(int x_, int y_) : x(x_), y(y_), type(CellType::EMPTY), gCost(0), hCost(0), fCost(0), parentX(-1), parentY(-1), hasParent(false) {}
};

struct AStarStep {
    std::vector<std::vector<GridCell>> grid;
    int currentX, currentY;
    bool hasCurrentCell;
    std::string description;
    int stepCount;
    int openListSize;
    int closedListSize;
    std::vector<std::pair<int, int>> path;  // Store the final path
};

class AStarController {
public:
    AStarController();
    ~AStarController();
    
    void initialize(int gridWidth, int gridHeight, int startX, int startY, int goalX, int goalY);
    void setWall(int x, int y);
    void clearWall(int x, int y);
    void toggleWall(int x, int y);
    void setStart(int x, int y);
    void setGoal(int x, int y);
    void clearGrid();
    void generateRandomMaze(float wallDensity = 0.3f);
    void start();
    void pause();
    void resume();
    void reset();
    void step();
    void stepBack();
    void fastForward();
    void slowDown();
    void speedUp();
    
    void setSpeed(float delayMs);
    void setStepCallback(std::function<void(const AStarStep&)> callback);
    
    AStarState getState() const { return m_state; }
    const std::vector<std::vector<GridCell>>& getCurrentGrid() const { return m_currentGrid; }
    int getGridWidth() const { return m_gridWidth; }
    int getGridHeight() const { return m_gridHeight; }
    int getStartX() const { return m_startX; }
    int getStartY() const { return m_startY; }
    int getGoalX() const { return m_goalX; }
    int getGoalY() const { return m_goalY; }
    const AStarStep& getCurrentStep() const { return m_currentStep; }
    float getSpeed() const { return m_stepDelay; }
    size_t getCurrentStepIndex() const { return m_currentStepIndex; }
    size_t getTotalSteps() const { return m_steps.size(); }
    int getStepCount() const;
    int getOpenListSize() const;
    int getClosedListSize() const;
    
    void update(float deltaTime);
    
private:
    void generateSteps();
    void runAStar();
    float calculateHeuristic(int x1, int y1, int x2, int y2);
    std::vector<std::pair<int, int>> getNeighbors(int x, int y);
    std::vector<std::pair<int, int>> reconstructPath(int goalX, int goalY);
    void addStep(const std::vector<std::vector<GridCell>>& grid, int currentX, int currentY, const std::string& desc, const std::vector<std::pair<int, int>>& path = {});
    
    std::vector<std::vector<GridCell>> m_originalGrid;
    std::vector<std::vector<GridCell>> m_currentGrid;
    std::vector<AStarStep> m_steps;
    AStarStep m_currentStep;
    
    int m_gridWidth, m_gridHeight;
    int m_startX, m_startY;
    int m_goalX, m_goalY;
    
    AStarState m_state;
    size_t m_currentStepIndex;
    float m_stepDelay;
    float m_timeSinceLastStep;
    
    // Statistics tracking
    int m_totalSteps;
    int m_currentOpenListSize;
    int m_currentClosedListSize;
    
    std::function<void(const AStarStep&)> m_stepCallback;
};
#pragma once
#include <vector>
#include <functional>
#include <string>

enum class QuicksortState {
    READY,
    SORTING,
    PAUSED,
    COMPLETED
};

struct QuicksortStep {
    std::vector<int> array;
    int pivotIndex;
    int lowIndex;
    int highIndex;
    std::string description;
    int operationCount;
    int comparisonCount;
    int swapCount;
};

class QuicksortController {
public:
    QuicksortController();
    ~QuicksortController();
    
    void initialize(const std::vector<int>& data);
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
    void setStepCallback(std::function<void(const QuicksortStep&)> callback);
    
    QuicksortState getState() const { return m_state; }
    const std::vector<int>& getCurrentArray() const { return m_currentArray; }
    const QuicksortStep& getCurrentStep() const { return m_currentStep; }
    float getSpeed() const { return m_stepDelay; }
    size_t getCurrentStepIndex() const { return m_currentStepIndex; }
    size_t getTotalSteps() const { return m_steps.size(); }
    int getOperationCount() const;
    int getComparisonCount() const;
    int getSwapCount() const;
    
    void update(float deltaTime);
    
private:
    void generateSteps();
    void quicksortRecursive(std::vector<int>& arr, int low, int high, std::vector<QuicksortStep>& steps);
    int partition(std::vector<int>& arr, int low, int high, std::vector<QuicksortStep>& steps);
    void addStep(std::vector<QuicksortStep>& steps, const std::vector<int>& arr, int pivot, int low, int high, const std::string& desc, bool isComparison = false, bool isSwap = false);
    void addFinaleSequence(std::vector<QuicksortStep>& steps, const std::vector<int>& arr);
    
    std::vector<int> m_originalArray;
    std::vector<int> m_currentArray;
    std::vector<QuicksortStep> m_steps;
    QuicksortStep m_currentStep;
    
    QuicksortState m_state;
    size_t m_currentStepIndex;
    float m_stepDelay;
    float m_timeSinceLastStep;
    
    // Statistics tracking
    int m_totalOperations;
    int m_totalComparisons;
    int m_totalSwaps;
    
    std::function<void(const QuicksortStep&)> m_stepCallback;
};
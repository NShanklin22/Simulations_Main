#include "simulations/sorting/quicksort/QuicksortController.h"
#include <algorithm>
#include <random>
#include <string>
#include <iostream>
#include <fstream>

QuicksortController::QuicksortController() 
    : m_state(QuicksortState::READY)
    , m_currentStepIndex(0)
    , m_stepDelay(2500.0f)  // Start much slower - 2.5 seconds per step
    , m_timeSinceLastStep(0.0f)
    , m_totalOperations(0)
    , m_totalComparisons(0)
    , m_totalSwaps(0)
{
}

QuicksortController::~QuicksortController() {
}

void QuicksortController::initialize(const std::vector<int>& data) {
    m_originalArray = data;
    m_currentArray = data;
    m_state = QuicksortState::READY;
    m_currentStepIndex = 0;
    m_timeSinceLastStep = 0.0f;
    m_totalOperations = 0;
    m_totalComparisons = 0;
    m_totalSwaps = 0;
    
    generateSteps();
    
    if (!m_steps.empty()) {
        m_currentStep = m_steps[0];
        if (m_stepCallback) {
            m_stepCallback(m_currentStep);
        }
    }
}

void QuicksortController::start() {
    if (m_state == QuicksortState::READY || m_state == QuicksortState::PAUSED) {
        m_state = QuicksortState::SORTING;
    }
}

void QuicksortController::pause() {
    if (m_state == QuicksortState::SORTING) {
        m_state = QuicksortState::PAUSED;
    }
}

void QuicksortController::resume() {
    if (m_state == QuicksortState::PAUSED) {
        m_state = QuicksortState::SORTING;
    }
}

void QuicksortController::reset() {
    m_currentArray = m_originalArray;
    m_state = QuicksortState::READY;
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

void QuicksortController::step() {
    if (m_currentStepIndex < m_steps.size() - 1) {
        m_currentStepIndex++;
        m_currentStep = m_steps[m_currentStepIndex];
        m_currentArray = m_currentStep.array;
        
        if (m_stepCallback) {
            m_stepCallback(m_currentStep);
        }
        
        if (m_currentStepIndex >= m_steps.size() - 1) {
            m_state = QuicksortState::COMPLETED;
        }
    }
}

void QuicksortController::setSpeed(float delayMs) {
    m_stepDelay = delayMs;
}

void QuicksortController::setStepCallback(std::function<void(const QuicksortStep&)> callback) {
    m_stepCallback = callback;
}

void QuicksortController::update(float deltaTime) {
    if (m_state == QuicksortState::SORTING) {
        m_timeSinceLastStep += deltaTime * 1000.0f; // Convert to milliseconds
        
        if (m_timeSinceLastStep >= m_stepDelay) {
            step();
            m_timeSinceLastStep = 0.0f;
        }
    }
}

void QuicksortController::generateSteps() {
    m_steps.clear();
    
    // Reset counters
    m_totalOperations = 0;
    m_totalComparisons = 0;
    m_totalSwaps = 0;
    
    // Initial state
    addStep(m_steps, m_originalArray, -1, 0, static_cast<int>(m_originalArray.size()) - 1, "Starting Quicksort");
    
    // Generate sorting steps
    std::vector<int> workingArray = m_originalArray;
    quicksortRecursive(workingArray, 0, static_cast<int>(workingArray.size()) - 1, m_steps);
    
    // Final state
    addStep(m_steps, workingArray, -1, -1, -1, "Quicksort Complete!");
    
    // Add finale sequence - highlight each bar from smallest to largest
    addFinaleSequence(m_steps, workingArray);
}

void QuicksortController::quicksortRecursive(std::vector<int>& arr, int low, int high, std::vector<QuicksortStep>& steps) {
    if (low < high) {
        // Add step showing the current subarray being processed
        addStep(steps, arr, -1, low, high, "Processing subarray [" + std::to_string(low) + ", " + std::to_string(high) + "]");
        
        // Partition the array
        int pivotIndex = partition(arr, low, high, steps);
        
        // Add step showing the pivot in its final position
        addStep(steps, arr, pivotIndex, low, high, "Pivot " + std::to_string(arr[pivotIndex]) + " in final position");
        
        // Recursively sort left and right subarrays
        quicksortRecursive(arr, low, pivotIndex - 1, steps);
        quicksortRecursive(arr, pivotIndex + 1, high, steps);
    }
}

int QuicksortController::partition(std::vector<int>& arr, int low, int high, std::vector<QuicksortStep>& steps) {
    // Debug logging
    std::ofstream debug("quicksort_debug.log", std::ios::app);
    debug << "Partition called: low=" << low << ", high=" << high << ", array_size=" << arr.size() << std::endl;
    
    // Bounds checking
    if (low < 0 || high >= static_cast<int>(arr.size()) || low > high) {
        debug << "ERROR: Invalid partition bounds!" << std::endl;
        debug.close();
        return low; // Safe fallback
    }
    
    // Hoare Partition Scheme - proper two-pointer QuickSort
    int pivot = arr[low]; // Choose first element as pivot (like OpenDSA reference)
    int left = low + 1;   // Left pointer starts after pivot
    int right = high;     // Right pointer starts at end
    
    debug << "Initial: pivot=" << pivot << ", left=" << left << ", right=" << right << std::endl;
    debug.close();
    
    // Add step showing the chosen pivot and initial pointers
    addStep(steps, arr, low, left, right, "Pivot: " + std::to_string(pivot) + " | Left pointer at " + std::to_string(left) + ", Right pointer at " + std::to_string(right));
    
    while (left <= right) {
        // Move left pointer right until we find element >= pivot
        while (left <= right && arr[left] < pivot) {
            addStep(steps, arr, low, left, right, "Left scan: " + std::to_string(arr[left]) + " < " + std::to_string(pivot) + " -> move right", true, false);
            left++;
        }
        
        // Move right pointer left until we find element <= pivot  
        while (left <= right && arr[right] > pivot) {
            addStep(steps, arr, low, left, right, "Right scan: " + std::to_string(arr[right]) + " > " + std::to_string(pivot) + " -> move left", true, false);
            right--;
        }
        
        // If pointers haven't crossed, we found elements to swap
        if (left <= right) {
            if (left != right) {
                // Step 1: Show elements that will be swapped
                addStep(steps, arr, low, left, right, "Found: " + std::to_string(arr[left]) + " >= " + std::to_string(pivot) + " and " + std::to_string(arr[right]) + " <= " + std::to_string(pivot) + " -> Will swap", false, true);
                
                // Perform the swap
                std::swap(arr[left], arr[right]);
                
                // Step 2: Show result after swap
                addStep(steps, arr, low, left, right, "Swapped: " + std::to_string(arr[left]) + " <-> " + std::to_string(arr[right]) + " | Continue scanning", false, true);
            }
            
            // Move both pointers inward
            left++;
            right--;
            
            if (left <= right) {
                addStep(steps, arr, low, left, right, "Move pointers: Left to " + std::to_string(left) + ", Right to " + std::to_string(right));
            }
        }
    }
    
    // Debug final pointer positions
    std::ofstream debug2("quicksort_debug.log", std::ios::app);
    debug2 << "After loop: left=" << left << ", right=" << right << std::endl;
    
    // Place pivot in its final position
    // In Hoare partition, when pointers cross, right pointer indicates where pivot should go
    // But we need to ensure right is valid (>= low) before swapping
    if (right > low) {
        debug2 << "Swapping pivot at " << low << " with element at " << right << std::endl;
        debug2 << "Before swap: arr[" << low << "]=" << arr[low] << ", arr[" << right << "]=" << arr[right] << std::endl;
        
        addStep(steps, arr, low, low, right, "Partition complete: Moving pivot to position " + std::to_string(right), false, true);
        std::swap(arr[low], arr[right]);
        
        debug2 << "After swap: arr[" << low << "]=" << arr[low] << ", arr[" << right << "]=" << arr[right] << std::endl;
        debug2.close();
        
        addStep(steps, arr, right, -1, -1, "Pivot " + std::to_string(arr[right]) + " in final position | Left partition: [" + std::to_string(low) + "-" + std::to_string(right-1) + "] | Right partition: [" + std::to_string(right+1) + "-" + std::to_string(high) + "]");
        return right;
    } else {
        // Edge case: pivot is already in correct position (smallest element)
        debug2 << "Pivot stays at position " << low << " (smallest element)" << std::endl;
        debug2.close();
        
        addStep(steps, arr, low, -1, -1, "Pivot " + std::to_string(arr[low]) + " already in final position | Right partition: [" + std::to_string(low+1) + "-" + std::to_string(high) + "]");
        return low;
    }
}

void QuicksortController::stepBack() {
    if (m_currentStepIndex > 0) {
        m_currentStepIndex--;
        m_currentStep = m_steps[m_currentStepIndex];
        m_currentArray = m_currentStep.array;
        
        if (m_stepCallback) {
            m_stepCallback(m_currentStep);
        }
    }
}

void QuicksortController::fastForward() {
    // Jump to completion
    if (!m_steps.empty()) {
        m_currentStepIndex = m_steps.size() - 1;
        m_currentStep = m_steps[m_currentStepIndex];
        m_currentArray = m_currentStep.array;
        m_state = QuicksortState::COMPLETED;
        
        if (m_stepCallback) {
            m_stepCallback(m_currentStep);
        }
    }
}

void QuicksortController::slowDown() {
    m_stepDelay = std::min(5000.0f, m_stepDelay + 100.0f);  // Smaller increments for fine control
}

void QuicksortController::speedUp() {
    m_stepDelay = std::max(1.0f, m_stepDelay - 100.0f);  // Much faster minimum - down to 1ms
}

int QuicksortController::getOperationCount() const {
    if (m_currentStepIndex < m_steps.size()) {
        return m_steps[m_currentStepIndex].operationCount;
    }
    return 0;
}

int QuicksortController::getComparisonCount() const {
    if (m_currentStepIndex < m_steps.size()) {
        return m_steps[m_currentStepIndex].comparisonCount;
    }
    return 0;
}

int QuicksortController::getSwapCount() const {
    if (m_currentStepIndex < m_steps.size()) {
        return m_steps[m_currentStepIndex].swapCount;
    }
    return 0;
}

void QuicksortController::addStep(std::vector<QuicksortStep>& steps, const std::vector<int>& arr, int pivot, int low, int high, const std::string& desc, bool isComparison, bool isSwap) {
    QuicksortStep step;
    step.array = arr;
    step.pivotIndex = pivot;
    step.lowIndex = low;
    step.highIndex = high;
    step.description = desc;
    step.operationCount = m_totalOperations;
    step.comparisonCount = m_totalComparisons;
    step.swapCount = m_totalSwaps;
    steps.push_back(step);
    
    // Increment counters AFTER adding the step
    if (isComparison) m_totalComparisons++;
    if (isSwap) m_totalSwaps++;
    m_totalOperations++;
}

void QuicksortController::addFinaleSequence(std::vector<QuicksortStep>& steps, const std::vector<int>& arr) {
    // Create a sorted copy to verify the array is sorted
    std::vector<int> sortedArr = arr;
    std::sort(sortedArr.begin(), sortedArr.end());
    
    // Highlight each element from smallest to largest
    for (size_t i = 0; i < sortedArr.size(); ++i) {
        // Find the position of the current value in the actual array
        int currentValue = sortedArr[i];
        int position = -1;
        
        for (size_t j = 0; j < arr.size(); ++j) {
            if (arr[j] == currentValue) {
                position = static_cast<int>(j);
                break;
            }
        }
        
        if (position != -1) {
            // Create a step that highlights this specific bar
            addStep(steps, arr, position, -1, -1, 
                    "Finale: Highlighting value " + std::to_string(currentValue) + 
                    " (position " + std::to_string(position) + ")");
        }
    }
    
    // Final celebration step
    addStep(steps, arr, -1, -1, -1, "Sorting Complete! Array is perfectly ordered.");
}
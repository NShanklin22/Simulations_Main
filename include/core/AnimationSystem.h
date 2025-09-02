#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>
#include <memory>

// Easing function types for satisfying visual transitions
enum class EasingType {
    Linear,
    EaseOut,       // Fast start, slow end (most satisfying for UI)
    EaseIn,        // Slow start, fast end  
    EaseInOut,     // Slow start and end, fast middle
    EaseOutBounce, // Satisfying bounce effect
    EaseOutElastic // Retro spring effect
};

// Animation target for smooth transitions
struct AnimationTarget {
    sf::Vector2f startPosition;
    sf::Vector2f targetPosition;
    sf::Vector2f currentPosition;
    float duration;           // Animation duration in seconds
    float elapsedTime;        // Current elapsed time
    EasingType easingType;
    bool isComplete;
    std::function<void()> onComplete; // Callback when animation finishes
    
    AnimationTarget(sf::Vector2f start, sf::Vector2f target, float dur, EasingType easing = EasingType::EaseOut)
        : startPosition(start), targetPosition(target), currentPosition(start)
        , duration(dur), elapsedTime(0.0f), easingType(easing), isComplete(false) {}
};

class AnimationSystem {
public:
    AnimationSystem();
    ~AnimationSystem();
    
    // Update all active animations
    void update(float deltaTime);
    
    // Create new animation for an object
    int animatePosition(sf::Vector2f startPos, sf::Vector2f targetPos, float duration, 
                       EasingType easing = EasingType::EaseOut, std::function<void()> onComplete = nullptr);
    
    // Get current animated position for an animation ID
    sf::Vector2f getCurrentPosition(int animationId) const;
    
    // Check if animation is complete
    bool isAnimationComplete(int animationId) const;
    
    // Remove completed animations
    void cleanupCompletedAnimations();
    
    // Clear all animations (for reset scenarios)
    void clearAllAnimations();
    
    // Set global animation speed multiplier (for speed controls)
    void setSpeedMultiplier(float multiplier) { m_speedMultiplier = multiplier; }
    
private:
    // Easing function implementations
    float applyEasing(float t, EasingType type) const;
    float easeLinear(float t) const;
    float easeOut(float t) const;        // Quadratic ease-out
    float easeIn(float t) const;         // Quadratic ease-in
    float easeInOut(float t) const;      // Quadratic ease-in-out
    float easeOutBounce(float t) const;  // Bouncy effect
    float easeOutElastic(float t) const; // Spring/elastic effect
    
    std::vector<std::unique_ptr<AnimationTarget>> m_animations;
    int m_nextAnimationId;
    float m_speedMultiplier; // Global speed control
};
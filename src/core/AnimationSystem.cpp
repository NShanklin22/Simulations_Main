#include "core/AnimationSystem.h"
#include <cmath>
#include <algorithm>

AnimationSystem::AnimationSystem() 
    : m_nextAnimationId(0)
    , m_speedMultiplier(1.0f)
{
}

AnimationSystem::~AnimationSystem() {
}

void AnimationSystem::update(float deltaTime) {
    // Update all active animations
    for (auto& animation : m_animations) {
        if (!animation || animation->isComplete) continue;
        
        // Apply speed multiplier for global animation control
        animation->elapsedTime += deltaTime * m_speedMultiplier;
        
        if (animation->elapsedTime >= animation->duration) {
            // Animation complete
            animation->elapsedTime = animation->duration;
            animation->currentPosition = animation->targetPosition;
            animation->isComplete = true;
            
            // Call completion callback if provided
            if (animation->onComplete) {
                animation->onComplete();
            }
        } else {
            // Calculate interpolated position using easing
            float t = animation->elapsedTime / animation->duration;
            float easedT = applyEasing(t, animation->easingType);
            
            // Linear interpolation with easing applied
            animation->currentPosition = animation->startPosition + 
                easedT * (animation->targetPosition - animation->startPosition);
        }
    }
}

int AnimationSystem::animatePosition(sf::Vector2f startPos, sf::Vector2f targetPos, float duration, 
                                   EasingType easing, std::function<void()> onComplete) {
    auto animation = std::make_unique<AnimationTarget>(startPos, targetPos, duration, easing);
    animation->onComplete = onComplete;
    
    int animationId = m_nextAnimationId++;
    
    // Ensure the vector is large enough to hold this animation ID
    if (animationId >= static_cast<int>(m_animations.size())) {
        m_animations.resize(animationId + 1);
    }
    
    m_animations[animationId] = std::move(animation);
    
    return animationId;
}

sf::Vector2f AnimationSystem::getCurrentPosition(int animationId) const {
    if (animationId < 0 || animationId >= static_cast<int>(m_animations.size())) {
        // Invalid animation ID - return a safe default that won't cause visual glitches
        return sf::Vector2f(-1.0f, -1.0f); // Signal invalid position
    }
    
    if (m_animations[animationId]) {
        return m_animations[animationId]->currentPosition;
    }
    
    // Animation exists but is null - return invalid position marker
    return sf::Vector2f(-1.0f, -1.0f);
}

bool AnimationSystem::isAnimationComplete(int animationId) const {
    if (animationId < 0 || animationId >= static_cast<int>(m_animations.size())) {
        return true;
    }
    
    if (m_animations[animationId]) {
        return m_animations[animationId]->isComplete;
    }
    
    return true;
}

void AnimationSystem::cleanupCompletedAnimations() {
    m_animations.erase(
        std::remove_if(m_animations.begin(), m_animations.end(),
            [](const std::unique_ptr<AnimationTarget>& animation) {
                return animation->isComplete;
            }),
        m_animations.end()
    );
}

void AnimationSystem::clearAllAnimations() {
    m_animations.clear();
    m_nextAnimationId = 0;
}

float AnimationSystem::applyEasing(float t, EasingType type) const {
    // Clamp t to [0, 1] range
    t = std::clamp(t, 0.0f, 1.0f);
    
    switch (type) {
        case EasingType::Linear:
            return easeLinear(t);
        case EasingType::EaseOut:
            return easeOut(t);
        case EasingType::EaseIn:
            return easeIn(t);
        case EasingType::EaseInOut:
            return easeInOut(t);
        case EasingType::EaseOutBounce:
            return easeOutBounce(t);
        case EasingType::EaseOutElastic:
            return easeOutElastic(t);
        default:
            return easeOut(t); // Default to most satisfying easing
    }
}

float AnimationSystem::easeLinear(float t) const {
    return t;
}

float AnimationSystem::easeOut(float t) const {
    // Quadratic ease-out: fast start, slow end (most satisfying for UI)
    return 1.0f - (1.0f - t) * (1.0f - t);
}

float AnimationSystem::easeIn(float t) const {
    // Quadratic ease-in: slow start, fast end
    return t * t;
}

float AnimationSystem::easeInOut(float t) const {
    // Quadratic ease-in-out: slow start and end, fast middle
    if (t < 0.5f) {
        return 2.0f * t * t;
    } else {
        return 1.0f - 2.0f * (1.0f - t) * (1.0f - t);
    }
}

float AnimationSystem::easeOutBounce(float t) const {
    // Satisfying bounce effect - great for completed swaps
    const float n1 = 7.5625f;
    const float d1 = 2.75f;
    
    if (t < 1.0f / d1) {
        return n1 * t * t;
    } else if (t < 2.0f / d1) {
        t -= 1.5f / d1;
        return n1 * t * t + 0.75f;
    } else if (t < 2.5f / d1) {
        t -= 2.25f / d1;
        return n1 * t * t + 0.9375f;
    } else {
        t -= 2.625f / d1;
        return n1 * t * t + 0.984375f;
    }
}

float AnimationSystem::easeOutElastic(float t) const {
    // Retro spring effect - perfect for satisfying visual feedback
    if (t == 0.0f || t == 1.0f) {
        return t;
    }
    
    const float c4 = (2.0f * 3.14159f) / 3.0f;
    return std::pow(2.0f, -10.0f * t) * std::sin((t * 10.0f - 0.75f) * c4) + 1.0f;
}
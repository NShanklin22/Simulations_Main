#include "core/AudioManager.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <memory>

AudioManager::AudioManager() 
    : m_enabled(true)
    , m_masterVolume(0.7f)
{
}

AudioManager::~AudioManager() {
}

bool AudioManager::initialize() {
    std::cout << "Initializing AudioManager..." << std::endl;
    
    // Create terminal-style sound effects
    createTerminalSounds();
    
    std::cout << "AudioManager initialized successfully" << std::endl;
    return true;
}

void AudioManager::playSound(SoundType type, float pitch, float volume) {
    if (!m_enabled) return;
    
    auto it = m_soundBuffers.find(type);
    if (it != m_soundBuffers.end()) {
        // Clean up finished sounds first
        m_activeSounds.erase(
            std::remove_if(m_activeSounds.begin(), m_activeSounds.end(),
                [](const std::unique_ptr<sf::Sound>& sound) {
                    return sound->getStatus() == sf::SoundSource::Status::Stopped;
                }),
            m_activeSounds.end()
        );
        
        // Create a new sound and add it to active sounds
        auto sound = std::make_unique<sf::Sound>(it->second);
        sound->setPitch(pitch);
        sound->setVolume(volume * m_masterVolume * 100.0f);
        sound->play();
        
        m_activeSounds.push_back(std::move(sound));
    }
}

void AudioManager::playTone(float frequency, float duration, float volume) {
    if (!m_enabled) return;
    
    // Generate a sine wave tone
    int sampleCount = static_cast<int>(SAMPLE_RATE * duration);
    std::vector<std::int16_t> samples(sampleCount);
    
    generateSineWave(samples.data(), sampleCount, frequency, volume * m_masterVolume);
    
    std::vector<sf::SoundChannel> channelMap = {sf::SoundChannel::Mono};
    if (m_toneBuffer.loadFromSamples(samples.data(), sampleCount, CHANNELS, SAMPLE_RATE, channelMap)) {
        // Clean up finished sounds first
        m_activeSounds.erase(
            std::remove_if(m_activeSounds.begin(), m_activeSounds.end(),
                [](const std::unique_ptr<sf::Sound>& sound) {
                    return sound->getStatus() == sf::SoundSource::Status::Stopped;
                }),
            m_activeSounds.end()
        );
        
        auto toneSound = std::make_unique<sf::Sound>(m_toneBuffer);
        toneSound->play();
        m_activeSounds.push_back(std::move(toneSound));
    }
}

void AudioManager::setMasterVolume(float volume) {
    m_masterVolume = std::clamp(volume, 0.0f, 1.0f);
}

void AudioManager::setEnabled(bool enabled) {
    m_enabled = enabled;
    if (!enabled) {
        // Stop all currently playing sounds
        for (auto& sound : m_activeSounds) {
            sound->stop();
        }
        m_activeSounds.clear();
    }
}

float AudioManager::mapValueToPitch(int value, int minValue, int maxValue) {
    if (maxValue == minValue) return 1.0f;
    
    // Map value to gentle retro pitch range - narrower for ASMR quality
    float normalizedValue = static_cast<float>(value - minValue) / (maxValue - minValue);
    
    // Use gentle logarithmic scaling for pleasant musical intervals
    float logValue = std::pow(normalizedValue, 0.8f);  // Softer compression
    
    // Narrower pitch range (0.8 to 1.4) for less jarring variations
    return 0.8f + logValue * 0.6f;
}

void AudioManager::generateSineWave(std::int16_t* samples, int sampleCount, float frequency, float amplitude) {
    const float TWO_PI = 6.28318f;
    for (int i = 0; i < sampleCount; ++i) {
        float time = static_cast<float>(i) / SAMPLE_RATE;
        float sample = amplitude * std::sin(frequency * TWO_PI * time);
        samples[i] = static_cast<std::int16_t>(sample * 32767);
    }
}

void AudioManager::generateFilteredWave(std::int16_t* samples, int sampleCount, float frequency, float amplitude, float cutoffFreq) {
    const float TWO_PI = 6.28318f;
    float previousSample = 0.0f;
    
    // Simple low-pass filter coefficient
    float filterCoeff = (cutoffFreq > 0.0f) ? std::exp(-TWO_PI * cutoffFreq / SAMPLE_RATE) : 0.0f;
    
    for (int i = 0; i < sampleCount; ++i) {
        float time = static_cast<float>(i) / SAMPLE_RATE;
        
        // Generate richer waveform with harmonics
        float sample = amplitude * (
            0.6f * std::sin(frequency * TWO_PI * time) +
            0.3f * std::sin(frequency * 2.0f * TWO_PI * time) +
            0.1f * std::sin(frequency * 3.0f * TWO_PI * time)
        );
        
        // Apply low-pass filter if specified
        if (cutoffFreq > 0.0f) {
            sample = filterCoeff * previousSample + (1.0f - filterCoeff) * sample;
            previousSample = sample;
        }
        
        samples[i] = static_cast<std::int16_t>(sample * 32767);
    }
}

void AudioManager::applyEnvelope(std::int16_t* samples, int sampleCount, float attackTime, float decayTime, float sustainLevel, float releaseTime) {
    int attackSamples = static_cast<int>(attackTime * SAMPLE_RATE);
    int decaySamples = static_cast<int>(decayTime * SAMPLE_RATE);
    int releaseSamples = static_cast<int>(releaseTime * SAMPLE_RATE);
    int sustainSamples = sampleCount - attackSamples - decaySamples - releaseSamples;
    
    if (sustainSamples < 0) sustainSamples = 0;
    
    for (int i = 0; i < sampleCount; ++i) {
        float envelope = 1.0f;
        
        if (i < attackSamples) {
            // Attack phase
            envelope = static_cast<float>(i) / attackSamples;
        } else if (i < attackSamples + decaySamples) {
            // Decay phase
            float decayProgress = static_cast<float>(i - attackSamples) / decaySamples;
            envelope = 1.0f - decayProgress * (1.0f - sustainLevel);
        } else if (i < attackSamples + decaySamples + sustainSamples) {
            // Sustain phase
            envelope = sustainLevel;
        } else {
            // Release phase
            int releaseIndex = i - attackSamples - decaySamples - sustainSamples;
            envelope = sustainLevel * (1.0f - static_cast<float>(releaseIndex) / releaseSamples);
        }
        
        samples[i] = static_cast<std::int16_t>(samples[i] * envelope);
    }
}

void AudioManager::createTerminalSounds() {
    // Create retro video game sounds with ASMR quality - warm, soft, satisfying
    
    // COMPARISON: Warm retro "blip" - gentle and pleasant for repeated listening
    {
        const float frequency = 520.0f;  // Lower, warmer frequency
        const float duration = 0.15f;
        const float amplitude = 0.25f;
        
        int sampleCount = static_cast<int>(SAMPLE_RATE * duration);
        std::vector<std::int16_t> samples(sampleCount);
        
        // Generate warm triangle-sine hybrid for retro feel
        for (int i = 0; i < sampleCount; ++i) {
            float time = static_cast<float>(i) / SAMPLE_RATE;
            float progress = time / duration;
            
            // Slight frequency modulation for organic feel
            float vibrato = 1.0f + 0.02f * std::sin(8.0f * 6.28318f * time);
            float currentFreq = frequency * vibrato;
            
            // Mix triangle and sine waves for softer retro sound
            float triangle = (2.0f / 3.14159f) * std::asin(std::sin(currentFreq * 6.28318f * time));
            float sine = std::sin(currentFreq * 6.28318f * time);
            
            float sample = amplitude * (
                0.6f * triangle +                                      // Warm triangle base
                0.3f * sine +                                         // Smooth sine overlay
                0.1f * std::sin(currentFreq * 2.0f * 6.28318f * time) // Subtle harmonic
            );
            samples[i] = static_cast<std::int16_t>(sample * 32767);
        }
        
        // Gentle envelope for ASMR quality
        applyEnvelope(samples.data(), sampleCount, 0.02f, 0.04f, 0.7f, 0.09f);
        
        std::vector<sf::SoundChannel> channelMap = {sf::SoundChannel::Mono};
        if (!m_soundBuffers[SoundType::COMPARISON].loadFromSamples(samples.data(), sampleCount, CHANNELS, SAMPLE_RATE, channelMap)) {
            std::cout << "Failed to load COMPARISON sound" << std::endl;
        }
    }
    
    // SWAP: Gentle liquid drop - natural and satisfying for ASMR
    {
        const float duration = 0.12f;
        const float amplitude = 0.25f;
        
        int sampleCount = static_cast<int>(SAMPLE_RATE * duration);
        std::vector<std::int16_t> samples(sampleCount);
        
        // Create liquid drop sound with exponential frequency decay
        for (int i = 0; i < sampleCount; ++i) {
            float time = static_cast<float>(i) / SAMPLE_RATE;
            float progress = time / duration;
            
            // Exponential frequency decay mimics natural water drop
            float baseFreq = 800.0f * std::exp(-progress * 3.0f);
            
            // Add subtle randomness for natural water character
            float waterNoise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.03f;
            
            // Main drop tone with subtle harmonics
            float mainTone = std::sin(baseFreq * 6.28318f * time);
            float harmonic = 0.3f * std::sin(baseFreq * 2.0f * 6.28318f * time);
            
            float sample = amplitude * (mainTone + harmonic + waterNoise);
            samples[i] = static_cast<std::int16_t>(sample * 32767);
        }
        
        // Very soft envelope for natural drop character
        applyEnvelope(samples.data(), sampleCount, 0.005f, 0.02f, 0.4f, 0.09f);
        
        std::vector<sf::SoundChannel> channelMap = {sf::SoundChannel::Mono};
        if (!m_soundBuffers[SoundType::SWAP].loadFromSamples(samples.data(), sampleCount, CHANNELS, SAMPLE_RATE, channelMap)) {
            std::cout << "Failed to load SWAP sound" << std::endl;
        }
    }
    
    // PIVOT_SELECT: Warm retro "ding" - distinctive but gentle
    {
        const float duration = 0.4f;
        const float amplitude = 0.32f;
        
        int sampleCount = static_cast<int>(SAMPLE_RATE * duration);
        std::vector<std::int16_t> samples(sampleCount);
        
        // Generate classic arcade-style "ding" with warmth
        for (int i = 0; i < sampleCount; ++i) {
            float time = static_cast<float>(i) / SAMPLE_RATE;
            float progress = time / duration;
            
            // Gentle upward frequency bend for classic arcade feel
            float baseFreq = 720.0f + progress * 80.0f;
            
            // Mix square-ish and sine waves for retro character
            float square = (std::sin(baseFreq * 6.28318f * time) > 0) ? 1.0f : -1.0f;
            float sine = std::sin(baseFreq * 6.28318f * time);
            
            // Smooth the square wave to avoid harshness
            float smoothSquare = square * 0.3f + sine * 0.7f;
            
            float sample = amplitude * (
                0.7f * smoothSquare +                                   // Retro square-sine hybrid
                0.2f * std::sin(baseFreq * 2.0f * 6.28318f * time) +    // Octave harmonic
                0.1f * std::sin(baseFreq * 0.5f * 6.28318f * time)      // Sub-harmonic for body
            );
            samples[i] = static_cast<std::int16_t>(sample * 32767);
        }
        
        // Classic arcade envelope - quick attack, gentle decay
        applyEnvelope(samples.data(), sampleCount, 0.01f, 0.05f, 0.4f, 0.34f);
        
        std::vector<sf::SoundChannel> channelMap = {sf::SoundChannel::Mono};
        if (!m_soundBuffers[SoundType::PIVOT_SELECT].loadFromSamples(samples.data(), sampleCount, CHANNELS, SAMPLE_RATE, channelMap)) {
            std::cout << "Failed to load PIVOT_SELECT sound" << std::endl;
        }
    }
    
    // ALGORITHM_COMPLETE: Classic retro victory jingle - satisfying but not overwhelming
    {
        const float duration = 0.9f;
        const float amplitude = 0.35f;
        
        int sampleCount = static_cast<int>(SAMPLE_RATE * duration);
        std::vector<std::int16_t> samples(sampleCount);
        
        // Generate classic arcade victory melody: C-E-G-C ascending
        for (int i = 0; i < sampleCount; ++i) {
            float time = static_cast<float>(i) / SAMPLE_RATE;
            float progress = time / duration;
            
            // Four-note ascending melody with timing
            float note1_time = 0.0f;   // C (0.0 - 0.2)
            float note2_time = 0.2f;   // E (0.2 - 0.4)
            float note3_time = 0.4f;   // G (0.4 - 0.6)
            float note4_time = 0.6f;   // C (0.6 - 0.9)
            
            float frequency = 261.63f; // Default to C
            float noteProgress = 0.0f;
            
            if (progress < 0.2f) {
                frequency = 261.63f; // C4
                noteProgress = progress / 0.2f;
            } else if (progress < 0.4f) {
                frequency = 329.63f; // E4
                noteProgress = (progress - 0.2f) / 0.2f;
            } else if (progress < 0.6f) {
                frequency = 392.00f; // G4
                noteProgress = (progress - 0.4f) / 0.2f;
            } else {
                frequency = 523.25f; // C5
                noteProgress = (progress - 0.6f) / 0.3f;
            }
            
            // Add slight vibrato for retro character
            float vibrato = 1.0f + 0.03f * std::sin(12.0f * 6.28318f * time);
            frequency *= vibrato;
            
            // Generate warm square-sine hybrid for retro sound
            float square = (std::sin(frequency * 6.28318f * time) > 0) ? 1.0f : -1.0f;
            float sine = std::sin(frequency * 6.28318f * time);
            float smoothSquare = square * 0.4f + sine * 0.6f;
            
            float sample = amplitude * smoothSquare;
            
            // Apply per-note envelope
            float noteEnvelope = 1.0f;
            if (noteProgress < 0.1f) {
                noteEnvelope = noteProgress / 0.1f; // Attack
            } else if (noteProgress > 0.8f) {
                noteEnvelope = (1.0f - noteProgress) / 0.2f; // Release
            }
            
            sample *= noteEnvelope;
            samples[i] = static_cast<std::int16_t>(sample * 32767);
        }
        
        // Overall gentle envelope
        applyEnvelope(samples.data(), sampleCount, 0.02f, 0.05f, 0.8f, 0.83f);
        
        std::vector<sf::SoundChannel> channelMap = {sf::SoundChannel::Mono};
        if (!m_soundBuffers[SoundType::ALGORITHM_COMPLETE].loadFromSamples(samples.data(), sampleCount, CHANNELS, SAMPLE_RATE, channelMap)) {
            std::cout << "Failed to load ALGORITHM_COMPLETE sound" << std::endl;
        }
    }
    
    // MENU_NAVIGATE: Gentle retro cursor blip
    {
        const float frequency = 680.0f;  // Lower, warmer frequency
        const float duration = 0.1f;
        const float amplitude = 0.2f;
        
        int sampleCount = static_cast<int>(SAMPLE_RATE * duration);
        std::vector<std::int16_t> samples(sampleCount);
        
        // Generate soft triangle wave for gentle navigation sound
        for (int i = 0; i < sampleCount; ++i) {
            float time = static_cast<float>(i) / SAMPLE_RATE;
            float triangle = (2.0f / 3.14159f) * std::asin(std::sin(frequency * 6.28318f * time));
            float sample = amplitude * triangle;
            samples[i] = static_cast<std::int16_t>(sample * 32767);
        }
        
        applyEnvelope(samples.data(), sampleCount, 0.01f, 0.02f, 0.7f, 0.07f);
        
        std::vector<sf::SoundChannel> channelMap = {sf::SoundChannel::Mono};
        if (!m_soundBuffers[SoundType::MENU_NAVIGATE].loadFromSamples(samples.data(), sampleCount, CHANNELS, SAMPLE_RATE, channelMap)) {
            std::cout << "Failed to load MENU_NAVIGATE sound" << std::endl;
        }
    }
    
    // MENU_SELECT: Pleasant retro selection sound
    {
        const float duration = 0.16f;
        const float amplitude = 0.28f;
        
        int sampleCount = static_cast<int>(SAMPLE_RATE * duration);
        std::vector<std::int16_t> samples(sampleCount);
        
        // Generate warm two-tone select sound
        for (int i = 0; i < sampleCount; ++i) {
            float time = static_cast<float>(i) / SAMPLE_RATE;
            float progress = time / duration;
            
            // Quick frequency rise for satisfying "beep-boop" character
            float frequency = 480.0f + progress * 200.0f;
            
            float sine = std::sin(frequency * 6.28318f * time);
            float sample = amplitude * sine;
            samples[i] = static_cast<std::int16_t>(sample * 32767);
        }
        
        applyEnvelope(samples.data(), sampleCount, 0.01f, 0.03f, 0.7f, 0.12f);
        
        std::vector<sf::SoundChannel> channelMap = {sf::SoundChannel::Mono};
        if (!m_soundBuffers[SoundType::MENU_SELECT].loadFromSamples(samples.data(), sampleCount, CHANNELS, SAMPLE_RATE, channelMap)) {
            std::cout << "Failed to load MENU_SELECT sound" << std::endl;
        }
    }
    
    // BUTTON_HOVER: Subtle retro hover hint
    {
        const float frequency = 580.0f;  // Lower, warmer frequency
        const float duration = 0.08f;
        const float amplitude = 0.15f;
        
        int sampleCount = static_cast<int>(SAMPLE_RATE * duration);
        std::vector<std::int16_t> samples(sampleCount);
        
        // Generate very gentle triangle wave for subtle feedback
        for (int i = 0; i < sampleCount; ++i) {
            float time = static_cast<float>(i) / SAMPLE_RATE;
            float triangle = (2.0f / 3.14159f) * std::asin(std::sin(frequency * 6.28318f * time));
            float sample = amplitude * triangle * 0.6f; // Extra gentle
            samples[i] = static_cast<std::int16_t>(sample * 32767);
        }
        
        applyEnvelope(samples.data(), sampleCount, 0.005f, 0.01f, 0.8f, 0.065f);
        
        std::vector<sf::SoundChannel> channelMap = {sf::SoundChannel::Mono};
        if (!m_soundBuffers[SoundType::BUTTON_HOVER].loadFromSamples(samples.data(), sampleCount, CHANNELS, SAMPLE_RATE, channelMap)) {
            std::cout << "Failed to load BUTTON_HOVER sound" << std::endl;
        }
    }
    
    // BUTTON_PRESS: Pleasant retro button click
    {
        const float duration = 0.14f;
        const float amplitude = 0.25f;
        
        int sampleCount = static_cast<int>(SAMPLE_RATE * duration);
        std::vector<std::int16_t> samples(sampleCount);
        
        // Generate satisfying downward "click" sound
        for (int i = 0; i < sampleCount; ++i) {
            float time = static_cast<float>(i) / SAMPLE_RATE;
            float progress = time / duration;
            
            // Downward frequency sweep for click character
            float frequency = 450.0f * (1.0f - progress * 0.4f);
            
            float sine = std::sin(frequency * 6.28318f * time);
            float sample = amplitude * sine;
            samples[i] = static_cast<std::int16_t>(sample * 32767);
        }
        
        applyEnvelope(samples.data(), sampleCount, 0.01f, 0.03f, 0.6f, 0.1f);
        
        std::vector<sf::SoundChannel> channelMap = {sf::SoundChannel::Mono};
        if (!m_soundBuffers[SoundType::BUTTON_PRESS].loadFromSamples(samples.data(), sampleCount, CHANNELS, SAMPLE_RATE, channelMap)) {
            std::cout << "Failed to load BUTTON_PRESS sound" << std::endl;
        }
    }
    
    std::cout << "Retro ASMR sound effects created successfully" << std::endl;
}
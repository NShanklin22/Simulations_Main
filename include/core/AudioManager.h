#pragma once
#include <SFML/Audio.hpp>
#include <memory>
#include <map>
#include <string>
#include <cstdint>
#include <vector>

enum class SoundType {
    COMPARISON,     // Terminal beep for comparisons
    SWAP,           // Mechanical chunk for swaps
    PIVOT_SELECT,   // Distinctive ping for pivot selection
    ALGORITHM_COMPLETE, // Success chime for completion
    HIGHLIGHT,      // General highlight sound
    MENU_NAVIGATE,  // Menu cursor movement
    MENU_SELECT,    // Menu item selection
    BUTTON_HOVER,   // Control panel button hover
    BUTTON_PRESS    // Control panel button press
};

class AudioManager {
public:
    AudioManager();
    ~AudioManager();
    
    bool initialize();
    void playSound(SoundType type, float pitch = 1.0f, float volume = 1.0f);
    void playTone(float frequency, float duration = 0.1f, float volume = 1.0f);
    
    // Configuration
    void setMasterVolume(float volume);
    void setEnabled(bool enabled);
    bool isEnabled() const { return m_enabled; }
    
    // Value-to-pitch mapping
    float mapValueToPitch(int value, int minValue, int maxValue);
    
private:
    void generateSineWave(std::int16_t* samples, int sampleCount, float frequency, float amplitude);
    void generateFilteredWave(std::int16_t* samples, int sampleCount, float frequency, float amplitude, float cutoffFreq = 0.0f);
    void applyEnvelope(std::int16_t* samples, int sampleCount, float attackTime, float decayTime, float sustainLevel, float releaseTime);
    void createTerminalSounds();
    
    bool m_enabled;
    float m_masterVolume;
    
    // Sound buffers for different terminal sounds
    std::map<SoundType, sf::SoundBuffer> m_soundBuffers;
    
    // For procedural tone generation
    sf::SoundBuffer m_toneBuffer;
    
    // Active sounds for managing playback
    std::vector<std::unique_ptr<sf::Sound>> m_activeSounds;
    
    // Audio settings
    static const int SAMPLE_RATE = 44100;
    static const int CHANNELS = 1;
};
#ifndef AUDIOPLAYER_H
#define AUDIO_PLAYER_H

#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>
#include <cstdint>
#include <string>
#include <string_view>
#include <memory>

class AudioPlayer
{
public:
    AudioPlayer(std::string soundFileLocation);
    ~AudioPlayer();

    void startSound();
    void stopSound();

private:
    void loadSoundEffect();

    Mix_Chunk* m_soundEffect{};

    static constexpr int m_soundFrequency{ 44100 };
    static constexpr Uint32 m_sampleFormat{ MIX_DEFAULT_FORMAT };
    static constexpr int m_numHardwareChannels{ 2 };
    static constexpr int m_sampleSize{ 512 }; // lower number = lower sound delay, but less quality

    const std::string m_soundFileLocation{};

    static constexpr int m_defaultChannelWhenTurnedOff{ -1 };
    int m_currentChannel{ m_defaultChannelWhenTurnedOff };
};

#endif
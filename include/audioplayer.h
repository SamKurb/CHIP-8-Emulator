#ifndef AUDIO_PLAYER_H
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
    bool isAudioLoaded();

private:
    void loadSoundEffect();

    std::unique_ptr<Mix_Chunk, decltype(&Mix_FreeChunk)> m_soundEffect{nullptr, Mix_FreeChunk };

    static constexpr int s_soundFrequency{ 44100 };
    static constexpr uint32_t s_sampleFormat{ MIX_DEFAULT_FORMAT };
    static constexpr int s_numHardwareChannels{ 2 };
    static constexpr int s_sampleSize{ 512 }; // lower number = lower sound delay, but less quality

    const std::string m_soundFileLocation{};

    static constexpr int s_channelValueIfNoChannel{ -1 };
    int m_currentChannel{ s_channelValueIfNoChannel };
};

#endif
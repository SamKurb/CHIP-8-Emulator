#ifndef AUDIOPLAYER_H
#define AUDIO_PLAYER_H

#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>
#include <cstdint>
#include <string>
#include <string_view>

class AudioPlayer
{
public:
    AudioPlayer(std::string soundFileLocation)
        : m_soundFileLocation{ soundFileLocation }
    {
        bool success{ true };

        if (SDL_Init(SDL_INIT_AUDIO) < 0)
        {
            std::cout << "SDL audio Failed to initialise. SDL_Error: " << SDL_GetError() << '\n';
            success = false;
        }

        if (Mix_OpenAudio(m_soundFrequency, m_sampleFormat, m_numHardwareChannels, m_sampleSize) < 0)
        {
            std::cout << "SDL_mixer could not initialize. SDL_mixer Error: " << Mix_GetError() << '\n';
            success = false;
        }

        if (!success)
        {
            std::cout << "AudioPlayer failed to initialise properly.\n";
            std::exit(1);
        }

        loadSoundEffect();
    }

    ~AudioPlayer()
    {
        Mix_FreeChunk(m_soundEffect);
        m_soundEffect = nullptr;
        Mix_Quit();
    }

    void playSound()
    {
        Mix_PlayChannel(-1, m_soundEffect, 0);
    }

private:


    void loadSoundEffect()
    {
        m_soundEffect = Mix_LoadWAV(m_soundFileLocation.data());
        if (m_soundEffect == NULL)
        {
            std::cout << "Failed to load sound effect in AudioPlayer::loadSoundEffect(). SDL_mixer Error: " << Mix_GetError() << '\n';
            std::exit(1);
        }
    }

    Mix_Chunk* m_soundEffect{};

    static constexpr int m_soundFrequency{ 44100 };
    static constexpr Uint32 m_sampleFormat{ MIX_DEFAULT_FORMAT };
    static constexpr int m_numHardwareChannels{ 2 };
    static constexpr int m_sampleSize{ 2048 }; // 2 KiB

    const std::string m_soundFileLocation{};
};

#endif
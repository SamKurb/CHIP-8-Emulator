#include "audioplayer.h"

AudioPlayer::AudioPlayer(std::string soundFileLocation)
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

AudioPlayer::~AudioPlayer()
{
   Mix_FreeChunk(m_soundEffect);
   m_soundEffect = nullptr;
   Mix_Quit();
}

void AudioPlayer::startSound()
{
   if (m_currentChannel == -1)
   {
       m_currentChannel = Mix_PlayChannel(-1, m_soundEffect, 0);
   }
}

void AudioPlayer::stopSound()
{
   if (m_currentChannel != m_defaultChannelWhenTurnedOff)
   {
       Mix_HaltChannel(m_currentChannel);
       m_currentChannel = m_defaultChannelWhenTurnedOff;
   }
}

void AudioPlayer::loadSoundEffect()
{
    m_soundEffect = Mix_LoadWAV(m_soundFileLocation.data());
    if (m_soundEffect == nullptr)
    {
        std::cout << "Failed to load sound effect in AudioPlayer::loadSoundEffect(). SDL_mixer Error: " << Mix_GetError() << std::endl;
        std::exit(1);
    }
}

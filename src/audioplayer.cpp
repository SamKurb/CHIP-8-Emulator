#include "audioplayer.h"
#include "sdlinitexception.h"
#include "fileinputexception.h"

AudioPlayer::AudioPlayer(std::string soundFileLocation)
: m_soundFileLocation{ soundFileLocation }
{
   if (SDL_Init(SDL_INIT_AUDIO) < 0)
   {
       std::string errorMsg{ SDL_GetError() };
       throw SDLInitException("Failed to initialize audio. SDL_Error: " + errorMsg);
   }

   if (Mix_OpenAudio(m_soundFrequency, m_sampleFormat, m_numHardwareChannels, m_sampleSize) < 0)
   {
       std::string errorMsg{ Mix_GetError() };
       throw SDLInitException("SDL_mixer could not initialize. SDL_mixer Error: " + errorMsg);
   }

   try
   {
       loadSoundEffect();
   }
   catch (const FileInputException& exception)
   {
       // If audio failed to load, emulator wont produce any sound but will still play.
       std::cerr << exception.what() << std::endl;
   }
}

AudioPlayer::~AudioPlayer()
{
   Mix_Quit();
}

void AudioPlayer::startSound()
{
   constexpr int channelToUse{ -1 };
   if (m_currentChannel == -1)
   {
       m_currentChannel = Mix_PlayChannel(channelToUse, m_soundEffect.get(), 0);
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
    m_soundEffect.reset(
        Mix_LoadWAV(m_soundFileLocation.data())
    );

    if (m_soundEffect == nullptr)
    {
       std::string errorMsg{ Mix_GetError() };
       throw FileInputException("Failed to load sound effect in AudioPlayer::loadSoundEffect(). "
                                "Audio will not play. SDL_mixer Error: " + errorMsg);
    }
}

bool AudioPlayer::isAudioLoaded()
{
    return m_soundEffect != nullptr;
}

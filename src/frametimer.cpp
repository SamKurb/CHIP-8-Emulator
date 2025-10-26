#include "frametimer.h"

#include "displaysettings.h"
#include "frameinfo.h"

FrameTimer::FrameTimer(const int targetFPS)
: m_actualFPS{ 0.0f }
, m_targetFPS{ targetFPS }
, m_startTimeMs{ 0 }
, m_endTimeMs{ 0 }
, m_frameTimeMs{ 0 }
// For a target fps of 60 this will be 16ms (rounded down because it is an int), so we will actually be rendering roughly 62-63 frames rather than 60
, m_targetFrameTimeMs{ 1000u / m_targetFPS }
{
}

FrameTimer::FrameTimer()
: FrameTimer(60)
{
}

void FrameTimer::startFrameTiming()
{
    m_startTimeMs = SDL_GetTicks();
}

void FrameTimer::endFrameTiming()
{
    m_endTimeMs = SDL_GetTicks();
    m_frameTimeMs = m_endTimeMs - m_startTimeMs;
}

void FrameTimer::delayToReachTargetFrameTime()
{
    if (m_frameTimeMs < m_targetFrameTimeMs)
    {
        uint32_t timeToWaitMs{ m_targetFrameTimeMs - m_frameTimeMs };
        SDL_Delay(timeToWaitMs);
        m_frameTimeMs += timeToWaitMs;
    }

    m_actualFPS = (m_frameTimeMs > 0) ? (1000.0f / static_cast<float>(m_frameTimeMs)) : 0.0f;
}

int FrameTimer::getTargetFPS() const
{
    return m_targetFPS;
}

float FrameTimer::getActualFPS() const
{
    return m_actualFPS;
}

void FrameTimer::setTargetFPS(const int newTargetFPS)
{
    m_targetFPS = newTargetFPS;
    m_targetFrameTimeMs = 1000u / m_targetFPS;
}



FrameInfo FrameTimer::getFrameInfo() const
{
    return FrameInfo {
        .startTimeMs = m_startTimeMs,
        .endTimeMs = m_endTimeMs,
        .frameTimeMs = m_frameTimeMs,
        .fps = m_actualFPS,
        .numInstructionsExecuted = 0
    };
}




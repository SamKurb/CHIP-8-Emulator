#include "frametimer.h"
#include "frameinfo.h"

FrameTimer::FrameTimer(int targetFPS)
: m_actualFPS{ 0.0f }
, m_targetFPS{ targetFPS }
, m_startTimeMs{ 0 }
, m_endTimeMs{ 0 }
, m_frameTimeMs{ 0 }
, m_targetFrameTimeMs{ 1000u / m_targetFPS }
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



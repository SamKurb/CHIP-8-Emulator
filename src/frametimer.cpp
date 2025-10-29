#include "../include/utils/frametimer.h"

#include "../include/types/displaysettings.h"
#include "../include/types/frameinfo.h"
#include <chrono>
#include <bits/this_thread_sleep.h>

FrameTimer::FrameTimer(const int targetFPS)
: m_targetFPS{ targetFPS }
, m_targetFrameTimeMicroSec{ std::chrono::duration_cast<Microseconds>(
                        Seconds(1)) / m_targetFPS }
{
}

FrameTimer::FrameTimer()
: FrameTimer(60)
{
}

void FrameTimer::startFrameTiming()
{
    m_startTimeMicroSec = Clock::now();
}

void FrameTimer::endFrameTiming()
{
    m_endTimeMicroSec = Clock::now();
    m_frameTimeMicroSec = std::chrono::duration_cast<Microseconds>(m_endTimeMicroSec - m_startTimeMicroSec);
}

void FrameTimer::delayToReachTargetFrameTime()
{
    if (m_frameTimeMicroSec < m_targetFrameTimeMicroSec)
    {
        const auto timeToWaitMicroSec{ m_targetFrameTimeMicroSec - m_frameTimeMicroSec };
        std::this_thread::sleep_for(timeToWaitMicroSec);
        m_frameTimeMicroSec += timeToWaitMicroSec;
    }

    m_actualFPS = (m_frameTimeMicroSec.count() > 0) ?
    (1'000'000.0f / static_cast<float>(m_frameTimeMicroSec.count()) )
    : 0.0f;
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
    m_targetFrameTimeMicroSec = std::chrono::duration_cast<Microseconds>(std::chrono::seconds(1)) / m_targetFPS;
}

FrameInfo FrameTimer::getFrameInfo() const
{
    return FrameInfo {
        .startTimeMs = std::chrono::duration_cast<Milliseconds>(m_startTimeMicroSec.time_since_epoch()).count(),
        .endTimeMs = std::chrono::duration_cast<Milliseconds>(m_endTimeMicroSec.time_since_epoch()).count(),
        .frameTimeMs = static_cast<float>(m_frameTimeMicroSec.count()) / 1000.0f,
        .fps = m_actualFPS,
        .numInstructionsExecuted = 0
    };
}




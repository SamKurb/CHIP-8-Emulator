#ifndef FRAME_TIMER_H
#define FRAME_TIMER_H

#include <SDL2/SDL.h>
#include <cstdint>
#include <memory>
#include <chrono>

struct FrameInfo;
struct DisplaySettings;

class FrameTimer
{
public:
    explicit FrameTimer(const int targetFPS);
    FrameTimer();

    void startFrameTiming();
    void endFrameTiming();

    void delayToReachTargetFrameTime();

    float getActualFPS() const;
    int getTargetFPS() const;
    void setTargetFPS(const int newTargetFPS);

    uint32_t getFrameTimeMs() const;

    FrameInfo getFrameInfo() const;

    uint64_t getCurrentTimeMicroSec() const;
    void delayToReachTargetFrameTime() const;



private:
    float m_actualFPS{};
    int m_targetFPS{ 60 };

    using Clock = std::chrono::steady_clock;
    Clock::time_point m_startTimeMicroSec{};
    Clock::time_point m_endTimeMicroSec{};

    using Microseconds = std::chrono::microseconds;
    Microseconds m_frameTimeMicroSec{};
    Microseconds m_targetFrameTimeMicroSec{};

    using Milliseconds = std::chrono::milliseconds;
    using Seconds = std::chrono::seconds;
};

#endif
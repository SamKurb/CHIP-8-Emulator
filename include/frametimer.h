#ifndef FRAME_TIMER_H
#define FRAME_TIMER_H

#include <SDL2/SDL.h>
#include <cstdint>

struct FrameInfo;

class FrameTimer
{
public:
    explicit FrameTimer(int targetFPS);

    void startFrameTiming();
    void endFrameTiming();

    void delayToReachTargetFrameTime();

    float getActualFPS() const;
    int getTargetFPS() const;

    uint32_t getFrameTimeMs() const;

    FrameInfo getFrameInfo() const;

private:
    float m_actualFPS;
    int m_targetFPS{};

    uint32_t m_startTimeMs{};
    uint32_t m_endTimeMs{};

    uint32_t m_frameTimeMs{};
    uint32_t m_targetFrameTimeMs{};
};

#endif
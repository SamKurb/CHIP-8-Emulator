#ifndef FRAME_TIMER_H
#define FRAME_TIMER_H

#include <SDL2/SDL.h>
#include <cstdint>
#include <memory>

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

private:
    float m_actualFPS{};
    int m_targetFPS{};

    uint32_t m_startTimeMs{};
    uint32_t m_endTimeMs{};

    uint32_t m_frameTimeMs{};
    uint32_t m_targetFrameTimeMs{};
};

#endif
#ifndef FRAME_INFO_H
#define FRAME_INFO_H

#include <SDL2/SDL.h>
#include <cstdint>

struct FrameInfo
{
    int64_t startTimeMs{};
    int64_t endTimeMs{};
    float frameTimeMs{};

    float fps{};
    uint64_t numInstructionsExecuted{ 0 };
};

#endif
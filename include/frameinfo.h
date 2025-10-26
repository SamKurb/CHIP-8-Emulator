#ifndef FRAME_INFO_H
#define FRAME_INFO_H

#include <SDL2/SDL.h>
#include <cstdint>

struct FrameInfo
{
    uint32_t startTimeMs{};
    uint32_t endTimeMs{};
    uint32_t frameTimeMs{};

    float fps{};
    uint64_t numInstructionsExecuted{ 0 };
};

#endif
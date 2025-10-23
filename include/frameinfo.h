#ifndef FRAMEINFO_H
#define FRAMEINFO_H

#include <cstdint>

struct FrameInfo
{
    uint32_t startTimeMs{};
    uint32_t endTimeMs{};
    uint32_t timeElapsedMs{};

    float fps{};
    uint64_t numInstructionsExecuted{};
};

#endif
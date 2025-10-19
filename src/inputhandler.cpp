#include "inputhandler.h"
#include "chip8.h"
#include "imgui_impl_sdl2.h"

void InputHandler::readChipAndSystemInputs(Chip8& chip)
{
    SDL_Event event{};

    while (SDL_PollEvent(&event) != 0)
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        checkForChipInput(event, chip);
        checkForSystemInput(event);
    }
}


void InputHandler::checkForChipInput(const SDL_Event& event, Chip8& chip)
{
    if (event.type == SDL_KEYDOWN)
    {
        for (std::size_t i{ 0 }; i < numChipKeys; ++i)
        {
            if (event.key.keysym.scancode == chipKeyMap[i])
            {
                chip.setKeyDown(i);
            }
        }
    }
    else if (event.type == SDL_KEYUP)
    {
        for (std::size_t i{ 0 }; i < numChipKeys; ++i)
        {
            if (event.key.keysym.scancode == chipKeyMap[i])
            {
                chip.setKeyUp(i);
            }
        }
    }
}

void InputHandler::checkForSystemInput(const SDL_Event event)
{
    if (event.type == SDL_QUIT) { m_isSystemKeyPressed[K_QUIT] = true; }
    if (event.type != SDL_KEYDOWN) { return; }

    for (std::size_t i{ 0 }; i < numSystemKeys; ++i)
    {
        if (event.key.keysym.scancode == systemKeyMap[i])
        {
            m_isSystemKeyPressed[i] = true;
        }
    }
}


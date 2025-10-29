#include "inputhandler.h"
#include "chip8.h"
#include "imgui_impl_sdl2.h"
#include <algorithm>
void InputHandler::readSystemInputs()
{
    SDL_Event event{};

    while (SDL_PollEvent(&event) != 0)
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        checkForSystemInput(event);
    }
}

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
        auto iteratorAtValidKey { std::ranges::find(chipKeyMap.begin(), chipKeyMap.end(), event.key.keysym.scancode) };
        if (iteratorAtValidKey != chipKeyMap.end())
        {
            auto keyIndex{ std::distance(chipKeyMap.begin(), iteratorAtValidKey) };
            Chip8::KeyInputs keyInput { static_cast<Chip8::KeyInputs>(keyIndex) };

            chip.setKeyDown(keyInput);
        }
    }
    else if (event.type == SDL_KEYUP)
    {
        auto iteratorAtValidKey { std::ranges::find(chipKeyMap.begin(), chipKeyMap.end(), event.key.keysym.scancode) };
        if (iteratorAtValidKey != chipKeyMap.end())
        {
            auto keyIndex{ std::distance(chipKeyMap.begin(), iteratorAtValidKey) };
            Chip8::KeyInputs keyInput { static_cast<Chip8::KeyInputs>(keyIndex) };

            chip.setKeyUp(keyInput);
        }
    }
}

void InputHandler::checkForSystemInput(const SDL_Event event)
{
    if (event.type == SDL_QUIT) { m_isSystemKeyPressed[SystemKeyInputs::K_QUIT] = true; }
    if (event.type != SDL_KEYDOWN) { return; }

    auto iteratorAtValidKey { std::ranges::find(systemKeyMap.begin(), systemKeyMap.end(), event.key.keysym.scancode) };
    if (iteratorAtValidKey != systemKeyMap.end())
    {
        auto keyIndex{ std::distance(systemKeyMap.begin(), iteratorAtValidKey) };
        SystemKeyInputs keyInput { static_cast<SystemKeyInputs>(keyIndex) };

        m_isSystemKeyPressed[keyInput] = true;
    }
}


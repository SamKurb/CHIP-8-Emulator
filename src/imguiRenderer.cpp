#include <array>

#include "imguirenderer.h"
#include "settings.h"

void ImguiRenderer::drawGeneralInfoWindow(const float fps, const uint8_t soundTimer, const StateManager::State currentState, const uint64_t numInstructionsExecuted, const uint64_t numInstructionsExecutedThisFrame) const
{
    ImGui::Begin("Emulator Info");

    ImGui::Text("FPS: %.1f", fps);

    ImGui::Text("Sound timer: %d", soundTimer);
    ImGui::Text("Current state: %s", (currentState == StateManager::State::running) ? "Running" : "Debug");
    ImGui::Text("Instructions Executed: %d", numInstructionsExecuted);
    ImGui::Text("IPF: %d", numInstructionsExecutedThisFrame);
    ImGui::End();
}

void ImguiRenderer::drawMemoryViewerWindow(const std::array<uint8_t, 4096> memoryContents, const uint16_t currentPCAddress) const
{
    const int bytesPerRow{ 16 };

    ImGui::Begin("Memory Viewer");

    for (std::size_t i{ 0 }; i < memoryContents.size(); i += bytesPerRow)
    {
        switch (i)
        {
        case ChipConfig::startAddress:
            ImGui::TextColored(green, "0x%04x | ", i);
            break;
        case ChipConfig::fontsLocation:
            ImGui::TextColored(blue, "0x%04x | ", i);
            break;
        default:
            ImGui::Text("0x%04X | ", i);
        }


        for (std::size_t j{ 0 }; j < bytesPerRow; ++j)
        {
            ImGui::SameLine();
            std::size_t currentAddress{ i + j };
            const uint8_t memContentsAtCurrLocation{ memoryContents[currentAddress] };

            if (currentAddress == currentPCAddress || currentAddress == currentPCAddress + 1)
            {
                ImGui::TextColored(red, "%02X", memContentsAtCurrLocation);
            }
            else
            {
                ImGui::Text("%02X", memContentsAtCurrLocation);
            }

        }

        ImGui::SameLine();
        ImGui::Text(" | ");

        for (std::size_t j{ 0 }; j < bytesPerRow; ++j)
        {
            ImGui::SameLine();
            std::size_t currentAddress{ i + j };
            const uint8_t memContentsAtCurrLocation{ memoryContents[currentAddress] };

            if (currentAddress == currentPCAddress || currentAddress == currentPCAddress)
            {
                ImGui::TextColored(red, "%c", memContentsAtCurrLocation);
            }
            else
            {
                ImGui::Text("%c", memContentsAtCurrLocation);
            }

        }

    }

    ImGui::End();
}

    
#include <array>

#include "imguirenderer.h"
#include "settings.h"
#include "chip8.h"
#include "renderer.h"
#include "displaysettings.h"

void ImguiRenderer::drawGeneralInfoWindow(const float fps, const uint8_t soundTimer, const StateManager::State currentState, const uint64_t numInstructionsExecuted, const uint64_t numInstructionsExecutedThisFrame) const
{
    ImGui::Begin("Emulator Info");

    ImGui::Text("FPS: %.1f", fps);

    ImGui::Text("Sound timer: %d", soundTimer);
    ImGui::Text("Current state: %s", (currentState == StateManager::State::running) ? "Running" : "Debug");
    ImGui::Text("Instructions Executed: %ld", numInstructionsExecuted);
    ImGui::Text("IPF: %ld", numInstructionsExecutedThisFrame);
    ImGui::End();
}

void ImguiRenderer::printRowStartAddress(const std::size_t rowStartAddress, const uint16_t programStartAddress, const uint16_t programEndAddress, const uint16_t fontStartAddress, uint16_t fontEndAddress) const
{
    if (rowStartAddress >= fontStartAddress && rowStartAddress <= fontEndAddress)
    {
		ImGui::TextColored(blue, "0x%04lX | ", rowStartAddress);
    }
    else if(rowStartAddress >= programStartAddress && rowStartAddress <= programEndAddress)
    {
        ImGui::TextColored(green, "0x%04lX | ", rowStartAddress);
    }
    else
    {
        ImGui::Text("0x%04lX | ", rowStartAddress);
    }
}

void ImguiRenderer::printMemoryRow(const std::array<uint8_t, 4096>& memoryContents, const std::size_t rowStartPos,  const int numBytesToPrint, 
    const Chip8& chip) const
{
	const uint16_t currentPCAddress{ chip.getPCAddress() };
	const uint16_t programStartAddress{ chip.getProgramStartAddress() };
	const uint16_t programEndAddress{ chip.getProgramEndAddress() };

	const uint16_t fontStartAddress{ chip.getFontStartAddress() };
	const uint16_t fontEndAddress{ chip.getFontEndAddress() };


    printRowStartAddress(rowStartPos, programStartAddress, programEndAddress, fontStartAddress, fontEndAddress);

    for (std::size_t addressOffset{ 0 }; addressOffset < numBytesToPrint; ++addressOffset)
    {
        ImGui::SameLine();
        std::size_t currentAddress{ rowStartPos + addressOffset };
        const uint8_t memContentsAtCurrLocation{ memoryContents[currentAddress] };


        if (currentAddress >= fontStartAddress && currentAddress <= fontEndAddress)
        {
            ImGui::TextColored(blue, "%02X", memContentsAtCurrLocation);
        }
        else if (currentAddress >= programStartAddress && currentAddress <= programEndAddress)
        {
            if (currentAddress == currentPCAddress || currentAddress == currentPCAddress + 1)
            {
                ImGui::TextColored(red, "%02X", memContentsAtCurrLocation);
            }
            else
            {
                ImGui::TextColored(green, "%02X", memContentsAtCurrLocation);
            }
        }
        else
        {
            ImGui::Text("%02X", memContentsAtCurrLocation);
        } 
    }

    ImGui::SameLine();
	ImGui::Text("   ");

    for (std::size_t addressOffset{ 0 }; addressOffset < numBytesToPrint; ++addressOffset)
    {
        ImGui::SameLine();
        std::size_t currentAddress{ rowStartPos + addressOffset };
        const uint8_t memContentsAtCurrLocation{ memoryContents[currentAddress] };

		const uint8_t validAsciiStart{ 33 };
		const uint8_t validAsciiEnd{ 126 };

		const char placeHolderForInvalidChar{ '.' };

        if (memContentsAtCurrLocation >= validAsciiStart && memContentsAtCurrLocation <= validAsciiEnd)
        {
            ImGui::Text("%c", memContentsAtCurrLocation);
        }
        else
        {
            ImGui::Text("%c", placeHolderForInvalidChar);
        }

    }
}

void ImguiRenderer::drawMemoryViewerWindow(const Chip8& chip) const
{
    const int bytesPerRow{ 16};

    const std::array<uint8_t, 4096> memoryContents{ chip.getMemoryContents() };
    const uint16_t currentPCAddress{ chip.getPCAddress() };
	
    ImGui::Begin("Memory Viewer");

    ImGui::SeparatorText("Legend");

    ImGui::TextColored(red, "Next instruction");
    ImGui::TextColored(green, "Program code");
    ImGui::TextColored(blue, "Font data");
    ImGui::Text("Unused memory");

    ImGui::SeparatorText("Memory Contents");

    for (std::size_t rowStartAddress{ 0 }; rowStartAddress < memoryContents.size(); rowStartAddress += bytesPerRow)
    {
        printMemoryRow(memoryContents, rowStartAddress, bytesPerRow, chip);
    }

    ImGui::End();
}

void ImguiRenderer::drawRegisterViewerWindow(const Chip8& chip) const
{
    const std::array<uint8_t, 16> registerContents{ chip.getRegisterContents() };

    ImGui::Begin("Register Viewer");

    for(unsigned int i{ 0 } ; i < registerContents.size() ; ++i)
    {
        ImGui::Text("V%X", i);

        const uint8_t contentsAtRegister{ registerContents[i] };

        ImGui::SameLine();
        ImGui::Text(" ... %02X ", contentsAtRegister);
    }

    ImGui::End();
}

void ImguiRenderer::drawDisplaySettingsWindowAndApplyChanges() const
{
    ImGui::Begin("Settings Menu");

    if (ImGui::Button("Display Grid Toggle"))
    {
        m_displaySettings -> gridOn = !(m_displaySettings -> gridOn);
    }

    ImGui::End();
}
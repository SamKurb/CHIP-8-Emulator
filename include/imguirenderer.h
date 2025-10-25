#ifndef IMGUI_RENDERER_H
#define IMGUI_RENDERER_H

#include <memory>
#include <SDL.h>

#include "displaysettings.h"
#include "imgui.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui_impl_sdl2.h"

#include "statemanager.h"
#include <vector>
#include "chip8.h"

class Renderer;
class FrameInfo;
struct DisplaySettings;

class ImguiRenderer
{
public:
	ImguiRenderer(SDL_Window* window, SDL_Renderer* renderer, std::shared_ptr<DisplaySettings> displaySettings, const float displayScaleFactor);
    ~ImguiRenderer();

    void drawGeneralInfoWindow(const FrameInfo &frameInfo, uint8_t soundTimer, const StateManager& currentState, uint64_t numInstructionsExecuted) const;

    void printRowStartAddress(const std::size_t rowStartAddress, const uint16_t programStartAddress, const uint16_t programEndAddress, const uint16_t fontStartAddress, const uint16_t fontEndAddress) const;

    void printMemoryRow(const std::array<uint8_t, 4096>& memoryContents, const std::size_t rowStartPos, const int numBytesToPrint,
        const Chip8& chip) const;

    void printASCIIRepresentationOfMemoryRow(const std::array<uint8_t, 4096>& memoryContents, const std::size_t rowStartPos, const int numBytesToPrint) const;

    void drawMemoryViewerWindow(const Chip8& chip) const;

    void drawRegisterViewerWindow(const Chip8& chip) const;

    void drawDisplaySettingsWindowAndApplyChanges() const;

	void drawChipSettingsWindow(Chip8::QuirkFlags& chipQuirkFlags) const;

    void drawGameDisplayWindow(SDL_Texture* gameFrame) const;

    void drawStackDisplayWindow(const std::vector<uint16_t>& stackContents) const;

    void drawROMSelectWindow(Chip8& chip);

    void drawAllImguiWindows(std::shared_ptr<DisplaySettings> displaySettings, Renderer &renderer,
                             ImguiRenderer &imguiRenderer, Chip8 &chip, StateManager &stateManager,
                             const FrameInfo &frameInfo);

	void displayHelpMarker(const std::string_view helpInfo) const;

	void drawKeyboardInputWindow();


private:
    int m_windowWidth{};
    int m_windowHeight{};
	std::shared_ptr<DisplaySettings> m_displaySettings{};
    const float m_dpiScaleFactor{ 0 };

    static constexpr ImVec4 red{ 1.0f, 0.0f, 0.0f, 1.0f };
    static constexpr ImVec4 green{ 0.0f, 1.0f, 0.0f, 1.0f };
    static constexpr ImVec4 blue{ 0.0f, 0.0f, 1.0f, 1.0f };

	static constexpr ImVec4 defaultTextColour{ 1.0f, 1.0f, 1.0f, 1.0f };
};

#endif
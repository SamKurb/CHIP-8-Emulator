#ifndef IMGUI_RENDERER_H
#define IMGUI_RENDERER_H

#include <memory>
#include <SDL.h>

#include "displaysettings.h"
#include "imgui.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui_impl_sdl2.h"

#include "statemanager.h"

class Chip8;
class Renderer;
struct DisplaySettings;

class ImguiRenderer
{
public:
	ImguiRenderer(SDL_Window* window, SDL_Renderer* renderer, std::shared_ptr<DisplaySettings> displaySettings, const float displayScaleFactor)
	: m_displaySettings{ std::move(displaySettings) }
	, m_dpiScaleFactor{ displayScaleFactor }
	{
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui::StyleColorsDark();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        
        //ImGui::GetStyle().ScaleAllSizes(displayScaleFactor);
        io.FontGlobalScale = displayScaleFactor;

        ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
        ImGui_ImplSDLRenderer2_Init(renderer);
	}

    ~ImguiRenderer()
    {
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
	}

    void drawGeneralInfoWindow(const float fps, const uint8_t soundTimer, const StateManager::State currentState, const uint64_t numInstructionsExecuted, const uint64_t numInstructionsExecutedThisFrame) const;
    
    void print4BitHex(const uint16_t valueToPrint, const ImVec4 colour = ImguiRenderer::defaultTextColour) const;

    void printRowStartAddress(const std::size_t rowStartAddress, const uint16_t programStartAddress, const uint16_t programEndAddress, const uint16_t fontStartAddress, const uint16_t fontEndAddress) const;

    void printMemoryRow(const std::array<uint8_t, 4096>& memoryContents, const std::size_t rowStartPos, const int numBytesToPrint,
        const Chip8& chip) const;

    void drawMemoryViewerWindow(const Chip8& chip) const;

    void drawRegisterViewerWindow(const Chip8& chip) const;

    void drawDisplaySettingsWindowAndApplyChanges() const;

private:
    const float m_dpiScaleFactor{ 0 };

	std::shared_ptr<DisplaySettings> m_displaySettings{};

    static constexpr ImVec4 red{ 1.0f, 0.0f, 0.0f, 1.0f };
    static constexpr ImVec4 green{ 0.0f, 1.0f, 0.0f, 1.0f };
    static constexpr ImVec4 blue{ 0.0f, 0.0f, 1.0f, 1.0f };

	static constexpr ImVec4 defaultTextColour{ 1.0f, 1.0f, 1.0f, 1.0f };
};

#endif
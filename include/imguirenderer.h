#ifndef IMGUI_RENDERER_H
#define IMGUI_RENDERER_H

#include <memory>
#include <SDL.h>

#include "types/displaysettings.h"
#include "imgui.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui_impl_sdl2.h"

#include "statemanager.h"
#include <vector>
#include "chip8.h"
#include <format>

class Renderer;
struct FrameInfo;
struct DisplaySettings;

class ImguiRenderer
{
public:
	ImguiRenderer(SDL_Window* window, SDL_Renderer* renderer, std::shared_ptr<DisplaySettings> displaySettings,
		const float displayScaleFactor);
    ~ImguiRenderer();

	void drawAllImguiWindows(std::shared_ptr<DisplaySettings> displaySettings, Renderer &renderer,
						 Chip8 &chip, const StateManager &stateManager,
						 const FrameInfo &frameInfo, const bool isAudioLoaded);

    template <class... Args>
    void displayText(std::format_string<Args...> format, Args&&... args) const
    {
        std::string string{ std::format(format, std::forward<Args>(args)...) };
        ImGui::Text("%s", string.data());
    }


private:
	void displayTextCentredInBounds(std::string_view text,
				const float leftBoundX, const float rightBoundX) const;

	void drawColourPicker(std::string_view title, RGBA& colourToEdit) const;
	void drawIntNumEditor(std::string_view title, int& numToEdit, int minValInclusive, int maxValInclusive) const;
	void drawIntNumEditor(std::string_view title, int& numToEdit) const;
	void drawTextScaleEditor(const float minTextScale, const float maxTextScale);


	void drawIPSEditor(Chip8& chip) const;

	void displayHelpMarker(std::string_view) const;

	void drawCheckBoxWithDesc(std::string_view title, bool& valueToToggle, const std::string& description) const;

	void drawGeneralInfoWindow(const FrameInfo &frameInfo, uint8_t soundTimer,
	const StateManager& currentState, uint64_t numInstructionsExecuted, const bool isAudioLoaded) const;

	void printRowStartAddress(const std::size_t rowStartAddress,
	const uint16_t programStartAddress, const uint16_t programEndAddress,
	const uint16_t fontStartAddress, const uint16_t fontEndAddress) const;

	void printMemoryRow(const std::array<uint8_t, 4096>& memoryContents, const std::size_t rowStartPos, const int numBytesToPrint,
		const Chip8::RuntimeMetaData& runtimeData, const uint16_t chipPCValue) const;

	void printASCIIRepresentationOfMemoryRow(const std::array<uint8_t, 4096>& memoryContents,
		const std::size_t rowStartPos, const int numBytesToPrint) const;

	void drawMemoryViewerWindow(const Chip8& chip) const;

	void drawSpecialChipRegisterContents(const Chip8& chip) const;
	void drawRegisterViewerWindow(const Chip8& chip) const;

	void drawDisplaySettingsWindowAndApplyChanges();

	void drawChipSettingsWindow(Chip8::QuirkFlags& chipQuirkFlags, Chip8& chip) const;

	void drawGameDisplayWindow(SDL_Texture* gameFrame) const;

	void drawStackDisplayWindow(const std::vector<uint16_t>& stackContents) const;

	void drawROMSelectWindow(Chip8& chip);


    int m_windowWidth{};
    int m_windowHeight{};
	std::shared_ptr<DisplaySettings> m_displaySettings{};
    float m_dpiScaleFactor{ 0 };

    static constexpr ImVec4 red{ 1.0f, 0.0f, 0.0f, 1.0f };
    static constexpr ImVec4 green{ 0.0f, 1.0f, 0.0f, 1.0f };
    static constexpr ImVec4 blue{ 0.0f, 0.0f, 1.0f, 1.0f };

	static constexpr ImVec4 defaultTextColour{ 1.0f, 1.0f, 1.0f, 1.0f };
};

#endif
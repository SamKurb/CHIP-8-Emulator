#include <array>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imguirenderer.h"

#include <ranges>

#include "chip8.h"
#include "renderer.h"
#include "../include/types/displaysettings.h"
#include "../include/exceptions/fileinputexception.h"

#include "../include/types/frameinfo.h"

#include "ImGuiFileDialog.h"
#include "imgui_internal.h"
#include "../external/imgui_file_dialog/ImGuiFileDialog.h"

ImguiRenderer::ImguiRenderer(SDL_Window* window, SDL_Renderer* renderer, std::shared_ptr<DisplaySettings> displaySettings, const float displayScaleFactor)
: m_windowWidth(displaySettings->mainWindowWidth)
, m_windowHeight(displaySettings->mainWindowHeight)
, m_displaySettings{ std::move(displaySettings) }
, m_dpiScaleFactor{ displayScaleFactor }
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::GetStyle().ScaleAllSizes(displayScaleFactor*0.5f);
    io.FontGlobalScale = displayScaleFactor;

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
}

ImguiRenderer::~ImguiRenderer()
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void ImguiRenderer::drawGeneralInfoWindow(
    const FrameInfo& frameInfo, const uint8_t soundTimer, const StateManager& stateManager,
    const uint64_t numInstructionsExecuted,
    const bool isAudioLoaded
) const
{
    ImGui::Begin("Emulator Info");

    displayText("FPS: {:.1f}", frameInfo.fps);
    displayText("Frame Time: {}ms", frameInfo.frameTimeMs);

    displayText("Sound timer: {}", soundTimer);

    StateManager::State currentState { stateManager.getCurrentState() };

    displayText("Current state: {}", stateManager.getCurrentStateString());


    if (currentState == StateManager::State::debug)
    {
        ImGui::SameLine();
        displayText(" - {}", stateManager.getCurrentDebugModeString());
    }

    displayText("Instructions Executed: {}", numInstructionsExecuted);
    displayText("IPF: {}", frameInfo.numInstructionsExecuted);

    displayText("Audio status:");
    ImGui::SameLine();
    if (isAudioLoaded)
    {
        ImGui::TextColored(green, "OK");
    }
    else
    {
        ImGui::TextColored(red, "Failed to load!");
    }

    ImGui::End();
}

void ImguiRenderer::printRowStartAddress(const std::size_t rowStartAddress,
    const uint16_t programStartAddress, const uint16_t programEndAddress,
    const uint16_t fontStartAddress, const uint16_t fontEndAddress) const
{
    if (rowStartAddress >= fontStartAddress && rowStartAddress <= fontEndAddress)
    {
        ImGui::TextColored(blue, "0x%04lX |", rowStartAddress);
    }
    else if(rowStartAddress >= programStartAddress && rowStartAddress <= programEndAddress)
    {
        ImGui::TextColored(green, "0x%04lX |", rowStartAddress);
    }
    else
    {
        displayText("0x{:04X} |", rowStartAddress);
    }
}

bool isValidASCIIValue(uint8_t value)
{
    constexpr uint8_t validAsciiStart{ 33 };
    constexpr uint8_t validAsciiEnd{ 126 };

    return value >= validAsciiStart && value <= validAsciiEnd;
}


void ImguiRenderer::printASCIIRepresentationOfMemoryRow(const std::array<uint8_t, 4096> &memoryContents,
    const std::size_t rowStartPos, const int numBytesToPrint) const
{
    auto rowBegin { memoryContents.begin() + rowStartPos };
    auto rowEnd { rowBegin + numBytesToPrint };

    constexpr char placeHolderForInvalidChar{ '.' };
    for (uint8_t currMemContents : std::ranges::subrange(rowBegin, rowEnd))
    {
        ImGui::SameLine(0.0f, 0.0f);

        if (isValidASCIIValue(currMemContents))
        {
            displayText("{:c}", currMemContents);
        }
        else
        {
            displayText("{:c}", placeHolderForInvalidChar);
        }
    }
}

void ImguiRenderer::printMemoryRow(const std::array<uint8_t, 4096>& memoryContents, const std::size_t rowStartPos,
                                   const int numBytesToPrint,
                                   const Chip8::RuntimeMetaData& runtimeData, const uint16_t chipPCValue) const
{
    printRowStartAddress(rowStartPos, runtimeData.programStartAddress, runtimeData.programEndAddress,
                                      runtimeData.fontStartAddress, runtimeData.fontEndAddress);

    auto rowBegin { memoryContents.begin() + rowStartPos };
    auto rowEnd { rowBegin + numBytesToPrint };

    auto memoryRowView{ std::ranges::subrange(rowBegin, rowEnd) };
    for (auto [offset, currMemContents] : std::views::enumerate(memoryRowView))
    {
        ImGui::SameLine();
        uint16_t currAddress{ Utility::toU16(rowStartPos + Utility::toU16(offset)) };

        const bool currAddressInFontRange { currAddress >= runtimeData.fontStartAddress
                                         && currAddress <= runtimeData.fontEndAddress };
        const bool currAddressInProgramRange { currAddress >= runtimeData.programStartAddress
                                         && currAddress <= runtimeData.programEndAddress };

        if (currAddressInFontRange)
        {
            ImGui::TextColored(blue, "%02X", currMemContents);
            continue;
        }

        if (currAddressInProgramRange)
        {
            if (currAddress == chipPCValue || currAddress == chipPCValue + 1)
            {
                ImGui::TextColored(red, "%02X", currMemContents);
            }
            else
            {
                ImGui::TextColored(green, "%02X", currMemContents);
            }
            continue;
        }

        displayText("{:02X}", currMemContents);
    }

    ImGui::SameLine();
    displayText(" ");

    printASCIIRepresentationOfMemoryRow(memoryContents, rowStartPos, numBytesToPrint);
}

void ImguiRenderer::drawMemoryViewerWindow(const Chip8& chip) const
{
    const int bytesPerRow{ 16};

    const std::array<uint8_t, 4096> memoryContents{ chip.getMemoryContents() };

    ImGui::Begin("Memory Viewer");

    ImGui::SeparatorText("Legend");

    ImGui::TextColored(red, "Next instruction");
    ImGui::TextColored(green, "Program code");
    ImGui::TextColored(blue, "Font data");
    displayText("Unused memory");

    ImGui::SeparatorText("Memory Contents");

    for (std::size_t rowStartAddress{ 0 }; rowStartAddress < memoryContents.size(); rowStartAddress += bytesPerRow)
    {
        printMemoryRow(memoryContents, rowStartAddress, bytesPerRow, chip.getRuntimeMetaData(), chip.getPCAddress());
    }

    ImGui::End();
}

void ImguiRenderer::displayTextCentredInBounds(const std::string& text,
                const float leftBoundX, const float rightBoundX) const
{
    const float boundCentrePos{ (leftBoundX + rightBoundX) / 2  };
    const float textWidth{ ImGui::CalcTextSize(text.data()).x };

    const float drawCoordStartX{ boundCentrePos - (textWidth / 2.0f) };

    ImGui::SetCursorPosX(drawCoordStartX);

    displayText("{}", text);
}

void ImguiRenderer::drawSpecialChipRegisterContents(const Chip8& chip) const
{
    constexpr int numSpecialRegisters { 4 };
    std::array<uint16_t, numSpecialRegisters> otherRegisterContents {
        chip.getPCAddress(),
        chip.getIndexRegisterContents(),
        chip.getDelayTimer(),
        chip.getSoundTimer()
    };

    constexpr std::array<std::string_view, numSpecialRegisters> otherRegisterNames {
        "PC",
        "IR",
        "Delay",
        "Sound"
    };

    ImGui::PushID("Special_Registers");
    ImGui::Columns(numSpecialRegisters);
    const float columnWidth { ImGui::GetColumnWidth(-1) };

    for (auto [index, nameContentPair] : std::views::enumerate(
        std::views::zip(otherRegisterNames, otherRegisterContents)))
    {
        ImGui::PushID(Utility::toInt(index*2));

        const auto [currRegName, currRegContents] = nameContentPair;

        const float columnStartXPos { columnWidth * static_cast<float>(index) };
        const float columnEndXPos{ columnStartXPos + columnWidth };
        displayTextCentredInBounds(std::string(currRegName), columnStartXPos, columnEndXPos);

        displayTextCentredInBounds(std::format("0x{:02X}", currRegContents), columnStartXPos, columnEndXPos);

        displayTextCentredInBounds(std::format("{}", currRegContents), columnStartXPos, columnEndXPos);

        ImGui::NextColumn();

        ImGui::PopID();
    }
    ImGui::Columns(1);
    ImGui::PopID();
}

void ImguiRenderer::drawRegisterViewerWindow(const Chip8& chip) const
{
    const std::array<uint8_t, 16> registerContents{ chip.getRegisterContents() };
    constexpr int numColumns{ 4 };
    constexpr int maxRegistersPerColumn{ registerContents.size() / numColumns };

    ImGui::Begin("Register Viewer");
    ImGui::Columns(numColumns);
    const float columnWidth { ImGui::GetColumnWidth(-1) };
    float columnStartXPos{ 0 };

    for (auto [index, currContents] : std::views::enumerate(registerContents))
    {
        ImGui::PushID(Utility::toInt(index));
        if (index > 0 && index % maxRegistersPerColumn == 0)
        {
            ImGui::NextColumn();
            columnStartXPos += columnWidth;
        }

        const float columnEndXPos{ columnStartXPos + columnWidth };
        std::string textToDraw{ std::format("V{0:X} ..{1:2X}", index, currContents) };
        displayTextCentredInBounds(textToDraw, columnStartXPos, columnEndXPos);
        ImGui::PopID();
    }

    ImGui::Columns(1);

    ImGui::Separator();
    drawSpecialChipRegisterContents(chip);
    ImGui::Separator();

    ImGui::End();
}

void ImguiRenderer::drawColourPicker(std::string_view title, RGBA& colourToEdit) const
{
    displayText("{}", title);
    ImVec4 bufferedColour{ colourToEdit };
    ImGui::SameLine();
    if (ImGui::ColorEdit4(title.data(), &bufferedColour.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
    {
        colourToEdit = bufferedColour;
    }
}

void ImguiRenderer::drawIntNumEditor(std::string_view title, int& numToEdit,
                int minValInclusive, int maxValInclusive) const
{
    assert(minValInclusive <= maxValInclusive);

    displayText("{}", title);
    ImGui::SameLine();
    std::string windowID{ std::format("##{}", title) };
    if (ImGui::InputInt(windowID.data(), &numToEdit))
    {
        if (numToEdit < minValInclusive)
        {
            numToEdit = minValInclusive;
        }
        else if (numToEdit > maxValInclusive)
        {
            numToEdit = maxValInclusive;
        }
    }
}

void ImguiRenderer::drawTextScaleEditor(const float minTextScale, const float maxTextScale)
{
    if (ImGui::SliderFloat("##TextScale", &m_dpiScaleFactor, minTextScale, maxTextScale))
    {
        ImGui::GetIO().FontGlobalScale = m_dpiScaleFactor;
    }
}

void ImguiRenderer::drawDisplaySettingsWindowAndApplyChanges()
{
    ImGui::Begin("Display Settings Menu");

    drawCheckBoxWithDesc("Display Grid", m_displaySettings->gridOn,
            "Choose whether or not to display a grid onto the screen");

    drawCheckBoxWithDesc("Render Game Display to GUI Window", m_displaySettings->renderGameToImGuiWindow,
            "Choose whether or not to render the game onto a GUI window rather than the main window");

    drawColourPicker("Off Pixel Colour: ", m_displaySettings->offPixelColour);
    drawColourPicker("On Pixel Colour: ", m_displaySettings->onPixelColour);
    drawColourPicker("Grid Colour: ", m_displaySettings->gridColour);

    constexpr int minFPS { 30 };
    constexpr int maxFPS { 1000 };
    drawIntNumEditor("Target FPS: ", m_displaySettings->targetFPS, minFPS, maxFPS);

    displayText("UI Text Scale:");
    ImGui::SameLine();

    constexpr float minTextScale{ 0.5f };
    constexpr float maxTextScale{ 10.0f };
    drawTextScaleEditor(minTextScale, maxTextScale);

    ImGui::End();
}

void ImguiRenderer::displayHelpMarker(const std::string_view helpInfo) const
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(helpInfo.data());
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void ImguiRenderer::drawCheckBoxWithDesc(std::string_view title, bool& valueToToggle, const std::string& description = "") const
{
    ImGui::Checkbox(title.data(), &valueToToggle);
    if (description != "")
    {
        ImGui::SameLine();
        displayHelpMarker(description);
    }
}

void ImguiRenderer::drawIPSEditor(Chip8& chip) const
{
    int currIPS { chip.getTargetNumInstrPerSecond() };
    int newIPS { currIPS };
    drawIntNumEditor("IPS: ", newIPS);
    if (newIPS != currIPS)
    {
        chip.setTargetNumInstrPerSecond(newIPS);
    }
}

void ImguiRenderer::drawChipSettingsWindow(Chip8::QuirkFlags& chipQuirkFlags, Chip8& chip) const
{
    ImGui::Begin("Chip Settings");
    displayText("Quirk Flags");
    ImGui::Separator();

    static constexpr std::array quirkCheckboxTitles {
        "Reset VF Flag",
        "Index Register Flag",
        "Pixel Wrap Screen Flag",
        "Shift Flag",
        "JMP Instruction Flag",
        "Display Wait Flag",
        "Halt on OOB memory Access",
    };

    static constexpr std::array quirkDescriptions {
        "Description to be added",
        "Description to be added",
        "Description to be added",
        "Description to be added",
        "Description to be added",
        "Description to be added",
        "Description to be added",
    };

    std::array quirkFlags {
        std::ref(chipQuirkFlags.resetVF),
        std::ref(chipQuirkFlags.index),
        std::ref(chipQuirkFlags.wrapScreen),
        std::ref(chipQuirkFlags.shift),
        std::ref(chipQuirkFlags.jump),
        std::ref(chipQuirkFlags.displayWait),
        std::ref(chipQuirkFlags.haltOnOOBAccess),
    };

    for (auto [quirkTitle, quirkFlag, quirkDesc]
        : std::ranges::views::zip(quirkCheckboxTitles, quirkFlags, quirkDescriptions))
    {
        drawCheckBoxWithDesc(quirkTitle, quirkFlag, quirkDesc);
    }

    ImGui::Separator();
    drawIPSEditor(chip);

    ImGui::End();
}

void ImguiRenderer::drawGameDisplayWindow(SDL_Texture* gameFrame) const
{
    ImGui::Begin("Game Display Window");

    int gameFrameTextureWidth{};
    int gameFrameTextureHeight{};

    SDL_QueryTexture(gameFrame, nullptr, nullptr,
        &gameFrameTextureWidth, &gameFrameTextureHeight);

    ImVec2 gameFrameTextureDimensions { static_cast<float>(gameFrameTextureWidth),
                                        static_cast<float>(gameFrameTextureHeight) };
    ImGui::Image(gameFrame, gameFrameTextureDimensions);

    ImGui::End();
}

void ImguiRenderer::drawStackDisplayWindow(const std::vector<uint16_t>& stackContents) const
{
    ImGui::Begin("Stack Viewer");
    ImGui::Columns(2);

    displayText("Depth");
    for (const auto& i : std::views::iota(0u, stackContents.capacity()))
    {
        displayText("{:2} ... ", i);
    }

    ImGui::NextColumn();

    displayText("Contents");
    for (auto [index, stackItem] : std::views::enumerate(stackContents))
    {
        displayText("{}  0x{:04X}", stackItem, stackItem);
        if (Utility::toUZ(index) == stackContents.size() - 1)
        {
            ImGui::SameLine();
            displayText("<-- SP");
        }
    }

    const std::size_t numEmptyStackSlots{ stackContents.capacity() - stackContents.size() };
    for (std::size_t i{ 0 } ; i < numEmptyStackSlots; ++i)
    {
        displayText("{}  0x{:04X}", 0, 0);
    }

    ImGui::End();
}

void ImguiRenderer::drawROMSelectWindow(Chip8& chip)
{
    ImGui::Begin("ROM Select");
    if (ImGui::Button("Select ROM"))
    {
        IGFD::FileDialogConfig config;config.path = ".";
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".ch8", config);
    }

    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

            chip = Chip8{};
            try
            {
                chip.loadFile(filePathName);
            }
            catch (const FileInputException& exception)
            {
                ImGuiFileDialog::Instance()->Close();
                ImGui::End();
                throw;
            }
        }
        ImGuiFileDialog::Instance()->Close();

    }
    ImGui::End();
}

void ImguiRenderer::drawAllImguiWindows(
    std::shared_ptr<DisplaySettings> displaySettings,
    Renderer& renderer,
    Chip8& chip, const StateManager& stateManager,
    const FrameInfo& frameInfo,
    const bool isAudioLoaded)
{
    ImGui_ImplSDL2_NewFrame();
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui::NewFrame();

    drawGeneralInfoWindow (
        frameInfo,
        chip.getSoundTimer(),
        stateManager,
        chip.getRuntimeMetaData().numInstructionsExecuted,
        isAudioLoaded
    );

    drawMemoryViewerWindow(chip);
    drawRegisterViewerWindow(chip);
    drawStackDisplayWindow(chip.getStackContents());

    drawDisplaySettingsWindowAndApplyChanges();
    drawChipSettingsWindow(chip.getEnabledQuirks(), chip);

    if (displaySettings -> renderGameToImGuiWindow)
    {
        SDL_Texture* currGameFrame { renderer.getCurrentGameFrame() };
        drawGameDisplayWindow(currGameFrame);
    }

    //drawKeyboardInputWindow();
    try
    {
        drawROMSelectWindow(chip);
    }
    catch (const FileInputException& exception)
    {
        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer.getRenderer());
        throw;
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer.getRenderer());
}
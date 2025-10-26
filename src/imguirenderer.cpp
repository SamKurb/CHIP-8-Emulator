#include <array>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imguirenderer.h"
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

void ImguiRenderer::printASCIIRepresentationOfMemoryRow(const std::array<uint8_t, 4096> &memoryContents,
    const std::size_t rowStartPos, const int numBytesToPrint) const
{
    constexpr uint8_t validAsciiStart{ 33 };
    constexpr uint8_t validAsciiEnd{ 126 };
    constexpr char placeHolderForInvalidChar{ '.' };

    auto rowBegin { memoryContents.begin() + rowStartPos };
    auto rowEnd { rowBegin + numBytesToPrint };

    for (auto currMemLocation { rowBegin} ; currMemLocation != rowEnd ; ++currMemLocation)
    {
        ImGui::SameLine(0.0f, 0.0f);

        uint8_t memContentsAtCurrLocation { *currMemLocation };

        if (memContentsAtCurrLocation >= validAsciiStart && memContentsAtCurrLocation <= validAsciiEnd)
        {
            displayText("{:c}", memContentsAtCurrLocation);
        }
        else
        {
            displayText("{:c}", placeHolderForInvalidChar);
        }

    }
}

void ImguiRenderer::printMemoryRow(const std::array<uint8_t, 4096>& memoryContents, const std::size_t rowStartPos,
                                   const int numBytesToPrint, const Chip8& chip) const
{
    const uint16_t currentPCAddress{ chip.getPCAddress() };
    const uint16_t programStartAddress{ chip.getProgramStartAddress() };
    const uint16_t programEndAddress{ chip.getProgramEndAddress() };

    const uint16_t fontStartAddress{ chip.getFontStartAddress() };
    const uint16_t fontEndAddress{ chip.getFontEndAddress() };

    printRowStartAddress(rowStartPos, programStartAddress, programEndAddress, fontStartAddress, fontEndAddress);

    auto rowBegin { memoryContents.begin() + rowStartPos };
    auto rowEnd { rowBegin + numBytesToPrint };
    for (auto currMemLocation{ rowBegin }; currMemLocation != rowEnd; ++currMemLocation)
    {
        ImGui::SameLine();
        const uint8_t memContentsAtCurrLocation{ *currMemLocation };

        uint16_t currAddress{ Utility::toU16(std::distance(memoryContents.begin(), currMemLocation))  };

        const bool currAddressInFontRange { currAddress >= fontStartAddress && currAddress <= fontEndAddress };
        const bool currAddressInProgramRange { currAddress >= programStartAddress && currAddress <= programEndAddress };

        if (currAddressInFontRange)
        {
            ImGui::TextColored(blue, "%02X", memContentsAtCurrLocation);
        }
        else if (currAddressInProgramRange)
        {
            if (currAddress == currentPCAddress || currAddress == currentPCAddress + 1)
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
            displayText("{:02X}", memContentsAtCurrLocation);
        }
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
        printMemoryRow(memoryContents, rowStartAddress, bytesPerRow, chip);
    }

    ImGui::End();
}

void ImguiRenderer::drawRegisterViewerWindow(const Chip8& chip) const
{
    const std::array<uint8_t, 16> registerContents{ chip.getRegisterContents() };
    constexpr int numColums{ 4 };
    constexpr int maxRegistersPerColumn{ registerContents.size() / numColums };

    ImGui::Begin("Register Viewer");
    ImGui::Columns(numColums);

    int numRegistersPlacedInCurrentColumn{ 0 };
    for(unsigned int i{ 0 } ; i < registerContents.size() ; ++i)
    {
        displayText("V{:X}", i);

        const uint8_t contentsAtRegister{ registerContents[i] };

        ImGui::SameLine();
        displayText(".. {:02X}", contentsAtRegister);
        ++numRegistersPlacedInCurrentColumn;
        ImGui::Dummy(ImVec2(0, 5.0f));

        if (numRegistersPlacedInCurrentColumn >= maxRegistersPerColumn)
        {
            ImGui::NextColumn();
            numRegistersPlacedInCurrentColumn = 0;
        }
    }
    ImGui::Separator();

    constexpr int numOtherRegisters { 4 };
    std::array<uint16_t, numOtherRegisters> otherRegisterContents {
        chip.getPCAddress(),
        chip.getIndexRegisterContents(),
        chip.getDelayTimer(),
        chip.getSoundTimer()
    };

    constexpr std::array<std::string_view, numOtherRegisters> otherRegisterNames {
        "PC",
        "IR",
        "Delay",
        "Sound"
    };

    ImGui::Columns(numOtherRegisters);
    float columnWidth { ImGui::GetColumnWidth(-1) };

    for (std::size_t i{ 0 } ; i < numOtherRegisters ; ++i)
    {
        const std::string_view currRegName{ otherRegisterNames[i] };

        ImVec2 textDimensions { ImGui::CalcTextSize(currRegName.data()) };
        float textWidth{ textDimensions.x };

        float columnStartXPos { columnWidth * static_cast<float>(i) };
        float columnCentreXPos { columnStartXPos + (columnWidth / 2.0f) };

        ImGui::SetCursorPosX(columnCentreXPos - (textWidth / 2.0f));
        displayText("{}\n", currRegName);

        const uint16_t currRegContents{ otherRegisterContents[i] };

        ImGui::SetCursorPosX(columnCentreXPos - (textWidth / 2.0f));
        displayText("0x{:02X}\n", currRegContents);

        ImGui::SetCursorPosX(columnCentreXPos - (textWidth / 2.0f));
        displayText("{}", currRegContents);

        ImGui::NextColumn();
    }

    ImGui::Separator();

    ImGui::End();
}

void ImguiRenderer::drawDisplaySettingsWindowAndApplyChanges() const
{
    ImGui::Begin("Settings Menu");

    if (ImGui::Button("Display Grid Toggle"))
    {
        m_displaySettings -> gridOn = !(m_displaySettings -> gridOn);
    }

    if (ImGui::Button("Toggle Rendering Game \nDisplay to GUI Window"))
    {
        m_displaySettings -> renderGameToImGuiWindow = !(m_displaySettings -> renderGameToImGuiWindow);
    }

    displayText("Off Pixel Colour: ");

    // Unfortunately ColorEdit4 only takes a float array as input, so this is the only easy way to do this
    static ImVec4 bufferedOffPixelColour { m_displaySettings -> offPixelColour };
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Off Pixel", (float*) &(bufferedOffPixelColour), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
    {
        m_displaySettings -> offPixelColour = bufferedOffPixelColour;
    }

    displayText("On Pixel Colour: ");
    ImGui::SameLine();
    static ImVec4 bufferedOnPixelColour { m_displaySettings -> onPixelColour };
    if (ImGui::ColorEdit4("On Pixel", (float*) &(bufferedOnPixelColour), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
    {
        m_displaySettings -> onPixelColour = bufferedOnPixelColour;
    }

    displayText("Grid Colour: ");
    ImGui::SameLine();
    static ImVec4 bufferedGridColour { m_displaySettings -> gridColour };
    if (ImGui::ColorEdit4("Grid Colour", (float*) &(bufferedGridColour), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
    {
        m_displaySettings -> gridColour = bufferedGridColour;
    }

    displayText("Target FPS:   ");
    ImGui::SameLine();

    constexpr int minFPS { 30 };

    if (ImGui::InputInt("##TargetFPS", &(m_displaySettings->targetFPS)))
    {
        if (m_displaySettings->targetFPS < minFPS)
        {
            m_displaySettings->targetFPS = minFPS;
        }
    }

    displayText("UI Text Scale:");
    ImGui::SameLine();
    static float textScale{ m_dpiScaleFactor };
    if (ImGui::SliderFloat("##TextScale", &textScale, 0.5f, 10.0f))
    {
        ImGui::GetIO().FontGlobalScale = textScale;
    }

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

void ImguiRenderer::drawChipSettingsWindow(Chip8::QuirkFlags& chipQuirkFlags, Chip8& chip) const
{
    ImGui::Begin("Chip Settings");

    displayText("Quirk Flags");
    ImGui::Separator();

    ImGui::Checkbox("reset VF flag", &chipQuirkFlags.resetVF);
    ImGui::SameLine();
    displayHelpMarker("To be added");


    ImGui::Checkbox("index register flag", &chipQuirkFlags.index);
    ImGui::SameLine();
    displayHelpMarker("To be added");


    ImGui::Checkbox("pixel wrap screen flag", &chipQuirkFlags.wrapScreen);
    ImGui::SameLine();
    displayHelpMarker("To be added");


    ImGui::Checkbox("shift flag", &chipQuirkFlags.shift);
    ImGui::SameLine();
    displayHelpMarker("To be added");


    ImGui::Checkbox("jmp instruction flag", &chipQuirkFlags.jump);
    ImGui::SameLine();
    displayHelpMarker("To be added");


    ImGui::Checkbox("display wait flag", &chipQuirkFlags.displayWait);
    ImGui::SameLine();
    displayHelpMarker("To be added");

    ImGui::Separator();

    int instructionsPerSecond{ chip.getTargetNumInstrPerSecond() };
    displayText("IPS:");
    ImGui::SameLine();
    if (ImGui::InputInt("##", &instructionsPerSecond))
    {
        chip.setTargetNumInstrPerSecond(instructionsPerSecond);
    }

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
    for (std::size_t i { 0 } ; i < stackContents.capacity() ; ++i)
    {
        displayText("{:2} ... ", i);
    }

    ImGui::NextColumn();

    displayText("Contents");
    for (std::size_t i{ 0 } ; i < stackContents.capacity() ; ++i)
    {
        const uint16_t stackItem = { Utility::toU16((i < stackContents.size()) ? stackContents[i] : 0) };
        displayText("{}  0x{:04X}", stackItem, stackItem);

        if (i == stackContents.size() - 1)
        {
            ImGui::SameLine();
            displayText("<-- SP");
        }
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

            ImGuiFileDialog::Instance()->Close();
        }

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
        chip.getNumInstructionsExecuted(),
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
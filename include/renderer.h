#ifndef RENDERER_H
#define RENDERER_H

#include <SDL.h>

#include <iostream>
#include <cassert>
#include <SDL_ttf.h>
#include <memory>

#include <algorithm>

#include "utils/utility.h"
#include "types/rgba.h"

#include "types/displaysettings.h"

class Renderer
{
public:

    struct Pixel
    {
        SDL_Rect rect{};
        RGBA colour{};
    };

    Renderer();

    explicit Renderer(std::shared_ptr<DisplaySettings> displaySettings);
    ~Renderer() noexcept;

    // SDL objects dont support shallow copying, so copy/copy costruction isnt an option
    Renderer(Renderer&) = delete;
    Renderer& operator=(Renderer&) = delete;

    Renderer(Renderer&&) = default;
    Renderer& operator=(Renderer&&) = default;

    template<typename T, std::size_t R, std::size_t C>
    using Array2D = std::array<std::array<T, C>, R>;

    template<typename T, std::size_t R, std::size_t C>
    void drawChipScreenBufferToFrame(const Array2D<T, R, C>& screenBuffer)
    {
        int gameFrameWidth{};
        int gameFrameHeight{};

        if (m_displaySettings -> renderGameToImGuiWindow)
        {
            SDL_SetRenderTarget(m_renderer.get(), m_currentGameFrame.get());

            SDL_QueryTexture(m_currentGameFrame.get(), nullptr, nullptr,
                &gameFrameWidth, &gameFrameHeight);
        }
        else
        {
            gameFrameWidth = m_displaySettings -> mainWindowWidth;
            gameFrameHeight = m_displaySettings -> mainWindowHeight;
        }

        const int pixelWidth{ gameFrameWidth / Utility::toInt(C)};
        const int pixelHeight{ gameFrameHeight / Utility::toInt(R) };

        for (std::size_t y{ 0 }; y < R; ++y)
        {
            for (std::size_t x{ 0 }; x < C; ++x)
            {
                uint8_t pixelOn{ screenBuffer[y][x] };
                RGBA pixelColour{ pixelOn ? m_displaySettings -> onPixelColour : m_displaySettings -> offPixelColour};

                const int xCoordOnScreen{ Utility::toInt(x) * pixelWidth  };
                const int yCoordOnScreen{ Utility::toInt(y) * pixelHeight };

                assert(xCoordOnScreen <= m_displaySettings -> mainWindowWidth && xCoordOnScreen >= 0);
                assert(yCoordOnScreen <= m_displaySettings -> mainWindowHeight && yCoordOnScreen >= 0);

                Pixel pixel{ { xCoordOnScreen, yCoordOnScreen, pixelWidth, pixelHeight }
                            , pixelColour };

                renderPixel(pixel);
            }
        }
        if (m_displaySettings -> gridOn)
        {
            drawGrid(pixelWidth, pixelHeight, C, R);
        }

        if (m_displaySettings -> renderGameToImGuiWindow)
        {
            SDL_SetRenderTarget(m_renderer.get(), nullptr);
        }
    }

    void render()
    {
        SDL_RenderPresent(m_renderer.get());
    }

    void drawGrid(const int pixelWidth, const int pixelHeight, int horizontalPixelAmount, int verticalPixelAmount);

    void drawTextAt(const std::string_view text, const int xPos, const int yPos);

    SDL_Window* getWindow() { return m_window.get(); }
    SDL_Renderer* getRenderer() { return m_renderer.get(); }

    float getDisplayScaleFactor() { return m_displayScaleFactor; }

    SDL_Texture* getCurrentGameFrame() { return m_currentGameFrame.get(); };

    void clearDisplay() const;

    void clearDisplay(const RGBA colour) const;

private:
    float m_defaultDPI{ 72.0f };
    float m_displayScaleFactor{ 0 };

    std::shared_ptr<DisplaySettings> m_displaySettings{};

    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> m_currentGameFrame { nullptr, SDL_DestroyTexture };

    std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)> m_defaultFont{ nullptr, TTF_CloseFont };

	std::string m_windowTitle{ "CHIP-8 Emulator" };

    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> m_window { nullptr, SDL_DestroyWindow };
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> m_renderer{ nullptr, SDL_DestroyRenderer };


    void renderPixel(const Pixel& pixel) const;

    float calculateDisplayDPIScaleFactor()
    {
        float diagonalDPI{};
        float horizontalDPI{};
        float verticalDPI{};

        if (SDL_GetDisplayDPI(0, &diagonalDPI, &horizontalDPI, &verticalDPI))
        {
            std::cerr << "SDL failed to fetch display DPI properly. SDL_Error: " << SDL_GetError() << std::endl;
            std::exit(1);
        }

        const float dpiScaleFactor { diagonalDPI / m_defaultDPI };
        return dpiScaleFactor;
    }

    float calculateDisplayScaleFactorFromWindowSize()
    {
        const int referenceWidth{ 1920 };
        const int referenceHeight{ 1080 };

        int windowWidth{};
        int windowHeight{};
        
        SDL_GetWindowSize(m_window.get(), &windowWidth, &windowHeight); 

        const float widthScaleFactor{ static_cast<float>(windowWidth) / referenceWidth };
        const float heightScaleFactor{ static_cast<float>(windowHeight) / referenceHeight };
        
        const float displayScaleFactor{ std::min(widthScaleFactor, heightScaleFactor) };

        return displayScaleFactor;
    }
};

#endif
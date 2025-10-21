#ifndef RENDERER_H
#define RENDERER_H

#include <SDL.h>

#include <iostream>
#include <cassert>
#include <SDL_ttf.h>
#include <memory>

#include <algorithm>

#include "utility.h"
#include "settings.h"
#include "colour.h"

#include "displaysettings.h"

class Renderer
{
public:

    struct Pixel
    {
        SDL_Rect rect{};
        Colour::RGBA colour{};
    };

    Renderer(const std::shared_ptr<DisplaySettings> displaySettings);
    ~Renderer();

    template<typename T, std::size_t R, std::size_t C>
    using Array2D = std::array<std::array<T, C>, R>;

    template<typename T, std::size_t R, std::size_t C>
    void drawToScreen(const Array2D<T, R, C>& screenBuffer)
    {
        const int pixelWidth{ Utility::toInt(m_width / C)};
        const int pixelHeight{ Utility::toInt(m_height / R) };

        for (std::size_t y{ 0 }; y < R; ++y)
        {
            for (std::size_t x{ 0 }; x < C; ++x)
            {
                uint8_t pixelOn{ screenBuffer[y][x] };
                Colour::RGBA pixelColour{ pixelOn ? m_displaySettings -> onPixelColour : m_displaySettings -> offPixelColour};

                int xCoordOnScreen{ Utility::toInt(x * pixelWidth)  };
                int yCoordOnScreen{ Utility::toInt(y * pixelHeight) };

                assert(xCoordOnScreen <= m_width && xCoordOnScreen >= 0);
                assert(yCoordOnScreen <= m_height && yCoordOnScreen >= 0);

                Pixel pixel{ { xCoordOnScreen, yCoordOnScreen, pixelWidth, pixelHeight }
                            , pixelColour };

                renderPixel(pixel);
            }
        }
        if (m_displaySettings -> gridOn)
        {
            SDL_SetRenderDrawColor(m_renderer.get(), 0x00, 0x00, 0x00, 0xFF);
            drawGrid(pixelWidth, pixelHeight, C, R);
        }
    }

    void render()
    {
        SDL_RenderPresent(m_renderer.get());
        //clearDisplay();
    }

    void drawGrid(const int pixelWidth, const int pixelHeight, int horizontalPixelAmount, int verticalPixelAmount);

    void drawTextAt(const std::string_view text, const int xPos, const int yPos);

    SDL_Window* getWindow() { return m_window.get(); }
    SDL_Renderer* getRenderer() { return m_renderer.get(); }

    float getDisplayScaleFactor() { return m_displayScaleFactor; }

private:
    const float m_defaultDPI{ 72.0f };
    float m_displayScaleFactor{ 0 };

    std::shared_ptr<DisplaySettings> m_displaySettings{};

    const int m_width{};
    const int m_height{};
    const int m_pixelSize{};

    std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)> m_defaultFont{ nullptr, TTF_CloseFont };

	const std::string m_windowTitle{ "CHIP-8 Emulator" };

    //SDL_Window* m_window{};
    //SDL_Renderer* m_renderer{};

    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> m_window { nullptr, SDL_DestroyWindow };
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> m_renderer{ nullptr, SDL_DestroyRenderer };


    void clearDisplay() const;

    void clearDisplay(const Colour::RGBA colour) const
    {
        SDL_Renderer* renderer{ m_renderer.get() };
        SDL_SetRenderDrawColor(renderer, colour.red, colour.green, colour.blue, colour.alpha);
        SDL_RenderClear(renderer);
    }

    void renderPixel(const Pixel& pixel) const
    {
        SDL_Renderer* renderer{ m_renderer.get() };
        SDL_SetRenderDrawColor(renderer, pixel.colour.red, pixel.colour.green, pixel.colour.blue, pixel.colour.alpha);
        SDL_RenderFillRect(renderer, &pixel.rect);
    }

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

        float dpiScaleFactor { diagonalDPI / m_defaultDPI };
        return dpiScaleFactor;
    }

    void toggleFullScreen();

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
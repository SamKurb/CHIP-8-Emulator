#ifndef RENDERER_H
#define RENDERER_H

#include <SDL.h>

#include <iostream>
#include <cassert>
#include <SDL_ttf.h>

#include "utility.h"
#include "settings.h"
#include "colour.h"

class Renderer
{
public:

    struct Pixel
    {
        SDL_Rect rect{};
        Colour::RGBValues colour{};
    };


    Renderer(int width, int height, bool gridOn,
        Colour::RGBValues onPixelColour, Colour::RGBValues offPixelColour);
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
                Colour::RGBValues pixelColour{ pixelOn ? m_onPixelColour : m_offPixelColour};

                int xCoordOnScreen{ Utility::toInt(x * pixelWidth)  };
                int yCoordOnScreen{ Utility::toInt(y * pixelHeight) };

                assert(xCoordOnScreen <= m_width && xCoordOnScreen >= 0);
                assert(yCoordOnScreen <= m_height && yCoordOnScreen >= 0);

                Pixel pixel{ { xCoordOnScreen, yCoordOnScreen, pixelWidth, pixelHeight }
                              , pixelColour };

                renderPixel(pixel);
            }
        }

        if (m_gridOn)
        {
            SDL_SetRenderDrawColor(m_renderer, 0x00, 0x00, 0x00, 0xFF);
            drawGrid(pixelWidth, pixelHeight, C, R);
        }
    }


    void render()
    { 
        SDL_RenderPresent(m_renderer); 
        //clearDisplay();
    }

    void drawGrid(const int pixelWidth, const int pixelHeight, int horizontalPixelAmount, int verticalPixelAmount);

    void drawTextAt(const std::string_view text, const int xPos, const int yPos);

    SDL_Window* getWindow() { return m_window; }
    SDL_Renderer* getRenderer() { return m_renderer; }

private:
    const int m_width{};
    const int m_height{};
    const int pixelSize{};

    bool m_gridOn{};

    SDL_Window* m_window{};
    SDL_Renderer* m_renderer{};

    const Colour::RGBValues m_offPixelColour{};
    const Colour::RGBValues m_onPixelColour{};

    void clearDisplay() const
    {
        clearDisplay(m_offPixelColour);
    }

    void clearDisplay(const Colour::RGBValues colour) const
    {
        SDL_SetRenderDrawColor(m_renderer, colour.r, colour.b, colour.g, 0xFF);
        SDL_RenderClear(m_renderer);
    }

    void renderPixel(const Pixel& p) const
    {
        SDL_SetRenderDrawColor(m_renderer, p.colour.r, p.colour.g, p.colour.b, 0xFF);
        SDL_RenderFillRect(m_renderer, &p.rect);
    }
};

#endif
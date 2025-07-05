#ifndef RENDERER_H
#define RENDERER_H

#include <SDL.h>
#include <SDL_events.h>
#include <iostream>
#include <cassert>

#include "utility.h"
#include "settings.h"

class Renderer
{
public:

    enum Colour : uint8_t
    {
        offColour = DisplayConfig::offColour,
        onColour = DisplayConfig::onColour,
    };

    struct Pixel
    {
        SDL_Rect rect{};
        Colour colour{};
    };


    Renderer(int width, int height, bool gridOn);
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

                Colour pixelColour{ pixelOn ? onColour : offColour};
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
            drawGrid(pixelWidth, pixelHeight, C, R);
        }
    }


    void render() { SDL_RenderPresent(m_renderer); }

    void drawGrid(const int pixelWidth, const int pixelHeight, int horizontalPixelAmount, int verticalPixelAmount);

private:

    const int m_width{};
    const int m_height{};
    const int pixelSize{};

    bool m_gridOn{};

    SDL_Window* m_window{};
    SDL_Surface* m_screenSurface{};
    SDL_Renderer* m_renderer{};

    void clearDisplay(Colour colour = offColour)
    {
        SDL_SetRenderDrawColor(m_renderer, colour, colour, colour, 0xFF);
        SDL_RenderClear(m_renderer);
    }

    void renderPixel(const Pixel& p)
    {
        SDL_SetRenderDrawColor(m_renderer, p.colour, p.colour, p.colour, 0xFF);
        SDL_RenderFillRect(m_renderer, &p.rect);
    }
};

#endif
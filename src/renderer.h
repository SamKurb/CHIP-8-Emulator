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

    void drawText(std::string_view text, int xPos, int yPos)
    {
        // Font by: Mark Simonsom. Name: "Anonymous". Source: https://www.fontsquirrel.com/fonts/list/classification/monospaced
        TTF_Font* font = TTF_OpenFont("assets/fonts/anonymous.ttf", 24);

        if (!font) 
        {
            std::cerr << "Font error: " << TTF_GetError() << std::endl;
            std::exit(1);
        }

        // as TTF_RenderText_Solid could only be used on
        // SDL_Surface then you have to create the surface first

        SDL_Color textColour = Colour::colours[Colour::darkGreen];

        SDL_Surface* textSurface{ TTF_RenderText_Solid(font, text.data(), {0,0xff,0})};

        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);

        // For some reason this function makes use of out parameters. Passing in a pointer to our
        // width and height will alter their values to represent the "intended" size of the text. Why is this library so esoteric? 
        int textWidth{};
        int textHeight{};
        TTF_SizeText(font, text.data(), &textWidth, &textHeight);

        SDL_Rect textRect{ xPos, yPos, textWidth, textHeight }; 

        SDL_RenderCopy(m_renderer, textTexture, nullptr, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

private:
    const int m_width{};
    const int m_height{};
    const int pixelSize{};

    bool m_gridOn{};

    SDL_Window* m_window{};
    SDL_Surface* m_screenSurface{};
    SDL_Renderer* m_renderer{};

    const Colour::RGBValues m_offPixelColour{};
    const Colour::RGBValues m_onPixelColour{};

    void clearDisplay()
    {
        clearDisplay(m_offPixelColour);
    }

    void clearDisplay(Colour::RGBValues colour)
    {
        SDL_SetRenderDrawColor(m_renderer, colour.r, colour.b, colour.g, 0xFF);
        SDL_RenderClear(m_renderer);
    }

    void renderPixel(const Pixel& p)
    {
        SDL_SetRenderDrawColor(m_renderer, p.colour.r, p.colour.g, p.colour.b, 0xFF);
        SDL_RenderFillRect(m_renderer, &p.rect);
    }

    
};

#endif
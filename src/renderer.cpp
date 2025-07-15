#include "renderer.h"


Renderer::Renderer(int width, int height, bool gridOn,
    Colour::RGBValues onPixelColour, Colour::RGBValues offPixelColour)
    : m_width{width}
    , m_height{height}
    , m_gridOn{gridOn}
    , m_onPixelColour{onPixelColour}
    , m_offPixelColour{offPixelColour}
{
    bool success{ true };

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL Failed to initialise. SDL_Error: " << SDL_GetError() << '\n';
        success = false;
    }
    else
    {
        m_window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            m_width, m_height, SDL_WINDOW_SHOWN);
        if (m_window == nullptr)
        {
            std::cout << "Failed to create window. SDL_Error: " << SDL_GetError() << '\n';
        }
        else
        {
            m_screenSurface = SDL_GetWindowSurface(m_window);
            m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
            if (m_renderer == nullptr)
            {
                std::cout << "Failed to create renderer. SDL_Error: " << SDL_GetError() << '\n';
                success = false;
            }
            else
            {
                SDL_SetRenderDrawColor(m_renderer, 0x00, 0x00, 0x00, 0xFF);
            }
        }
    }
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
    }

	if (!success)
	{
        std::cout << "Renderer failed to initialise properly.\n";
        std::exit(1);
	}
}

Renderer::~Renderer()
{

    SDL_DestroyWindow(m_window);
    m_window = nullptr;

    SDL_Quit();
}

void Renderer::drawGrid(const int pixelWidth, const int pixelHeight, int horizontalPixelAmount, int verticalPixelAmount)
{
    // Draw vertical lines
    for (int i{ 0 }; i < horizontalPixelAmount ; ++i)
    {
        int xCoord{ i * pixelWidth };
        SDL_RenderDrawLine(m_renderer, xCoord, 0, xCoord, m_height);
    }

    // Draw horizontal lines
    for (int i{ 0 }; i < verticalPixelAmount; ++i)
    {
        int yCoord{ i * pixelHeight };
        SDL_RenderDrawLine(m_renderer, 0, yCoord, m_width, yCoord);
    }
}

void Renderer::drawTextAt(const std::string_view text, const int xPos, const int yPos)
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

    SDL_Surface* textSurface{ TTF_RenderText_Solid(font, text.data(), {0,0xff,0}) };

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



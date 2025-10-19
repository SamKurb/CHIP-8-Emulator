#include "renderer.h"
#include "displaysettings.h"

Renderer::Renderer(std::shared_ptr<DisplaySettings> displaySettings)
    : m_defaultDPI { 120.0f }
    , m_displaySettings{ std::move(displaySettings) }
    , m_width { m_displaySettings -> userDesiredWidth }
    , m_height { m_displaySettings -> userDesiredHeight }
{
    bool success{ true };

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL Failed to initialise. SDL_Error: " << SDL_GetError() << std::endl;
        success = false;
    }
    else
    {
        m_window.reset(
            SDL_CreateWindow(m_windowTitle.data(),
                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                m_width, m_height, 
                SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI)
        );

        if (m_window == nullptr)
        {
            std::cerr << "Failed to create window. SDL_Error: " << SDL_GetError() << std::endl;
        }
        else
        {
            m_renderer.reset( SDL_CreateRenderer(m_window.get(), -1, SDL_RENDERER_ACCELERATED));

            if (m_renderer == nullptr)
            {
                std::cerr << "Failed to create renderer. SDL_Error: " << SDL_GetError() << std::endl;
                success = false;
            }
            else
            {
                SDL_SetRenderDrawColor(m_renderer.get(), 0x00, 0x00, 0x00, 0xFF);
            }
        }
    }
    if (TTF_Init() == -1)
    {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        success = false;
    }

    m_defaultFont.reset(TTF_OpenFont("assets/fonts/anonymous.ttf", 24));

    if (!m_defaultFont)
    {
        std::cerr << "Font error: " << TTF_GetError() << std::endl;
        success = false;
    }

	if (!success)
	{
        std::cerr << "Renderer failed to initialise properly." << std::endl;
        std::exit(1);
	}

    m_displayScaleFactor = calculateDisplayDPIScaleFactor() * calculateDisplayScaleFactorFromWindowSize();
}

Renderer::~Renderer()
{
    SDL_Quit();
}

void Renderer::clearDisplay() const
{
    clearDisplay(m_displaySettings -> offPixelColour);
}

void Renderer::drawGrid(const int pixelWidth, const int pixelHeight, int horizontalPixelAmount, int verticalPixelAmount)
{
    SDL_Renderer* renderer{ m_renderer.get() };
    for (int i{ 0 }; i < horizontalPixelAmount ; ++i)
    {
        int xCoord{ i * pixelWidth };
        SDL_RenderDrawLine(renderer, xCoord, 0, xCoord, m_height);
    }

    for (int i{ 0 }; i < verticalPixelAmount; ++i)
    {
        int yCoord{ i * pixelHeight };
        SDL_RenderDrawLine(renderer, 0, yCoord, m_width, yCoord);
    }
}

void Renderer::toggleFullScreen()
{
    const bool isFullScreenEnabled{};
}

void Renderer::drawTextAt(const std::string_view text, const int xPos, const int yPos)
{
    SDL_Renderer* renderer{ m_renderer.get() };

    // as TTF_RenderText_Solid could only be used on
    // SDL_Surface then you have to create the surface first

    SDL_Surface* textSurface{ TTF_RenderText_Solid(m_defaultFont.get(), text.data(), {0,0xFF,0}) };

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    // For some reason this function makes use of out parameters. Passing in a pointer to our
    // width and height will alter their values to represent the "intended" size of the text. Why is this library so esoteric? 
    int textWidth{};
    int textHeight{};
    TTF_SizeText(m_defaultFont.get(), text.data(), &textWidth, &textHeight);

    SDL_Rect textRect{ xPos, yPos, textWidth, textHeight };

    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}



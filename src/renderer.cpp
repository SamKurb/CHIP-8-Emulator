#include "renderer.h"
#include "displaysettings.h"

Renderer::Renderer(std::shared_ptr<DisplaySettings> displaySettings)
    : m_defaultDPI { 120.0f }
    , m_displaySettings{ std::move(displaySettings) }
{
    bool success{ true };

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL Failed to initialise. SDL_Error: " << SDL_GetError() << std::endl;
        success = false;
    }
    else
    {
        const int defaultWindowWidth{ 1920 };
        const int defaultWindowHeight{ 1200 };
        m_window.reset(
            SDL_CreateWindow(
                m_windowTitle.data(),
                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                defaultWindowWidth,defaultWindowHeight,
                SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN_DESKTOP
            )
        );

        if (m_window == nullptr)
        {
            std::cerr << "Failed to create window. SDL_Error: " << SDL_GetError() << std::endl;
            std::exit(1);
        }
        else
        {
            m_renderer.reset( SDL_CreateRenderer(m_window.get(), -1, SDL_RENDERER_ACCELERATED));

            int actualWindowWidthPixels{};
            int actualWindowHeightPixels{};

            SDL_GetRendererOutputSize(m_renderer.get(), &actualWindowWidthPixels, &actualWindowHeightPixels);

            std::cout << actualWindowWidthPixels << " x " << actualWindowHeightPixels << std::endl;

            int logicalWindowWidthPixels{};
            int logicalWindowHeightPixels{};

            SDL_GetWindowSize(m_window.get(), &logicalWindowWidthPixels, &logicalWindowHeightPixels);

            std::cout << logicalWindowWidthPixels << " x " << logicalWindowHeightPixels << std::endl;

            m_displaySettings -> mainWindowWidth = actualWindowWidthPixels;
            m_displaySettings -> mainWindowHeight = actualWindowHeightPixels;

            SDL_DisplayMode displayMode{};
            SDL_GetDisplayMode(0, 0, &displayMode);

            std::cout << displayMode.w << "x" << displayMode.h << " @" << displayMode.refresh_rate << std::endl;

            if (m_renderer == nullptr)
            {
                std::cerr << "Failed to create renderer. SDL_Error: " << SDL_GetError() << std::endl;
                success = false;
            }
            else
            {
                SDL_SetRenderDrawColor(m_renderer.get(), 0x00, 0x00, 0x00, 0xFF);
            }

            // int actualWidthPixels{};
            // int actualHeightPixels{};

            const int placeHolderWidth{ 1920 };
            const int placeHolderHeight{ 960 };

            m_currentGameFrame.reset(
                SDL_CreateTexture(
                    m_renderer.get(),
                    SDL_PIXELFORMAT_ARGB8888,
                    SDL_TEXTUREACCESS_TARGET,
                    placeHolderWidth,
                    placeHolderHeight
                )
            );

            if (m_currentGameFrame == nullptr)
            {
                std::cerr << "Failed to create texture currentGameFrame. SDL_Error: " << SDL_GetError() << std::endl;
                success = false;
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
    Colour::RGBA gridColour{ m_displaySettings -> gridColour };
    SDL_SetRenderDrawColor(m_renderer.get(), gridColour.red, gridColour.green, gridColour.blue, gridColour.alpha);

    int frameWidth{ m_displaySettings -> mainWindowWidth };
    int frameHeight{ m_displaySettings -> mainWindowHeight };

    if (m_displaySettings->renderGameToImGuiWindow)
    {
        SDL_SetRenderTarget(m_renderer.get(), m_currentGameFrame.get());
        frameWidth = m_displaySettings -> gameDisplayTextureWidth;
        frameHeight = m_displaySettings -> gameDisplayTextureHeight;
    }

    SDL_Renderer* renderer{ m_renderer.get() };
    for (int i{ 0 }; i < horizontalPixelAmount ; ++i)
    {
        int xCoord{ i * pixelWidth };
        SDL_RenderDrawLine(renderer, xCoord, 0, xCoord, frameHeight);
    }

    for (int i{ 0 }; i < verticalPixelAmount; ++i)
    {
        int yCoord{ i * pixelHeight };
        SDL_RenderDrawLine(renderer, 0, yCoord, frameWidth, yCoord);
    }

    if (m_displaySettings->renderGameToImGuiWindow)
    {
        SDL_SetRenderTarget(m_renderer.get(), nullptr);
    }
}

void Renderer::toggleFullScreen()
{
    const bool isFullScreenEnabled{};
}

void Renderer::drawTextAt(const std::string_view text, const int xPos, const int yPos)
{
    if (m_displaySettings->renderGameToImGuiWindow)
    {
        SDL_SetRenderTarget(m_renderer.get(), m_currentGameFrame.get());
    }

    SDL_Renderer* renderer{ m_renderer.get() };

    // as TTF_RenderText_Solid could only be used on
    // SDL_Surface then you have to create the surface first

    SDL_Surface* textSurface{ TTF_RenderText_Solid(m_defaultFont.get(), text.data(), {0,0xFF,0}) };

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    int textWidth{};
    int textHeight{};
    TTF_SizeText(m_defaultFont.get(), text.data(), &textWidth, &textHeight);

    SDL_Rect textRect{ xPos, yPos, textWidth, textHeight };

    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    if (m_displaySettings->renderGameToImGuiWindow)
    {
        SDL_SetRenderTarget(m_renderer.get(), nullptr);
    }
}




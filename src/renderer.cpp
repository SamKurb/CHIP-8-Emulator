#include "renderer.h"
#include "../include/types/displaysettings.h"
#include "../include/exceptions/sdlinitexception.h"

Renderer::Renderer()
: m_defaultDPI(92.0f)
, m_displaySettings{ std::make_unique<DisplaySettings>() }
{
}

Renderer::Renderer(std::shared_ptr<DisplaySettings> displaySettings)
    : m_defaultDPI { 92.0f }
    , m_displaySettings{ std::move(displaySettings) }
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::string errorMsg{ SDL_GetError() };
        throw SDLInitException("SDL Failed to initialise. SDL_Error: " + errorMsg);
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
            std::string errorMsg{ SDL_GetError() };
            throw SDLInitException("Failed to create window. SDL_Error: " + errorMsg);
        }
        else
        {
            m_renderer.reset( SDL_CreateRenderer(m_window.get(), -1, SDL_RENDERER_ACCELERATED));

            int actualWindowWidthPixels{};
            int actualWindowHeightPixels{};

            SDL_GetRendererOutputSize(m_renderer.get(), &actualWindowWidthPixels, &actualWindowHeightPixels);

            int logicalWindowWidthPixels{};
            int logicalWindowHeightPixels{};

            SDL_GetWindowSize(m_window.get(), &logicalWindowWidthPixels, &logicalWindowHeightPixels);

            m_displaySettings -> mainWindowWidth = actualWindowWidthPixels;
            m_displaySettings -> mainWindowHeight = actualWindowHeightPixels;

            SDL_DisplayMode displayMode{};
            SDL_GetDisplayMode(0, 0, &displayMode);

            if (m_renderer == nullptr)
            {
                std::string errorMsg{ SDL_GetError() };
                throw SDLInitException("Failed to create renderer. SDL_Error: " + errorMsg);
            }
            else
            {
                SDL_SetRenderDrawColor(m_renderer.get(), 0x00, 0x00, 0x00, 0xFF);
            }
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
                std::string errorMsg{ SDL_GetError() };
                throw SDLInitException("Failed to create texture currentGameFrame. SDL_Error: " + errorMsg);
            }
        }
    }
    if (TTF_Init() == -1)
    {
        std::string errorMsg{ TTF_GetError() };
        throw SDLInitException("SDL_ttf could not initialize! TTF_Error: " + errorMsg);
    }

    m_defaultFont.reset(TTF_OpenFont("assets/fonts/anonymous.ttf", 24));

    if (!m_defaultFont)
    {
        std::string errorMsg{ TTF_GetError() };
        throw SDLInitException("Font error: " + errorMsg);
    }
    m_displayScaleFactor = calculateDisplayDPIScaleFactor() * calculateDisplayScaleFactorFromWindowSize();
}

Renderer::~Renderer() noexcept
{
    m_currentGameFrame.reset();
    m_defaultFont.reset();
    
    m_renderer.reset();
    m_window.reset();

    TTF_Quit();
    SDL_Quit();
}



void Renderer::clearDisplay() const
{
    SDL_SetRenderTarget(m_renderer.get(), m_currentGameFrame.get());
    clearDisplay(m_displaySettings -> offPixelColour);

    SDL_SetRenderTarget(m_renderer.get(), nullptr);
    clearDisplay(m_displaySettings -> offPixelColour);
}

void Renderer::clearDisplay(const RGBA colour) const
{
    SDL_Renderer* renderer{ m_renderer.get() };
    SDL_SetRenderDrawColor(renderer, colour.red, colour.green, colour.blue, colour.alpha);
    SDL_RenderClear(renderer);
}

void Renderer::drawGrid(const int pixelWidth, const int pixelHeight, int horizontalPixelAmount, int verticalPixelAmount)
{
    RGBA gridColour{ m_displaySettings -> gridColour };
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
    SDL_RenderDrawLine(renderer, 0, frameHeight-1, frameWidth, frameHeight-1);

    for (int i{ 0 }; i < verticalPixelAmount; ++i)
    {
        int yCoord{ i * pixelHeight };
        SDL_RenderDrawLine(renderer, 0, yCoord, frameWidth, yCoord);
    }
    SDL_RenderDrawLine(renderer, frameWidth-1, 0, frameWidth-1, frameHeight);

    if (m_displaySettings->renderGameToImGuiWindow)
    {
        SDL_SetRenderTarget(m_renderer.get(), nullptr);
    }
}

void Renderer::drawTextAt(const std::string_view text, const int xPos, const int yPos)
{
    if (m_displaySettings->renderGameToImGuiWindow)
    {
        SDL_SetRenderTarget(m_renderer.get(), m_currentGameFrame.get());
    }

    const RGBA textColour{ RGBA::pureGreen() };
    std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> textSurface{
        TTF_RenderText_Solid(m_defaultFont.get(), text.data(), textColour),
        &SDL_FreeSurface
    };

    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> textTexture{
        SDL_CreateTextureFromSurface(m_renderer.get(), textSurface.get()),
        &SDL_DestroyTexture
    };

    int textWidth{};
    int textHeight{};
    TTF_SizeText(m_defaultFont.get(), text.data(), &textWidth, &textHeight);

    SDL_Rect textRect{ xPos, yPos, textWidth, textHeight };

    SDL_RenderCopy(m_renderer.get(), textTexture.get(), nullptr, &textRect);

    if (m_displaySettings->renderGameToImGuiWindow)
    {
        SDL_SetRenderTarget(m_renderer.get(), nullptr);
    }
}

void Renderer::renderPixel(const Pixel &pixel) const
{
    if (m_displaySettings->renderGameToImGuiWindow)
    {
        SDL_SetRenderTarget(m_renderer.get(), m_currentGameFrame.get());
    }

    SDL_SetRenderDrawColor(m_renderer.get(),
        pixel.colour.red, pixel.colour.green, pixel.colour.blue, pixel.colour.alpha);

    SDL_RenderFillRect(m_renderer.get(), &pixel.rect);

    if (m_displaySettings->renderGameToImGuiWindow)
    {
        SDL_SetRenderTarget(m_renderer.get(), nullptr);
    }
}



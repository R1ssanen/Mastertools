#include "context.hpp"

#include "color.hpp"
#include "settings.hpp"
#include "srpch.hpp"
#include "vertex.hpp"

#include "glm/gtx/color_space.hpp"

namespace {
    constexpr uint8_t BloomIntensity = 160;
    constexpr int BloomDownsamplePasses = 4;
}

namespace core
{

void Context::Update()
{
    SDL_RenderClear(Renderer);
    SDL_UpdateTexture(RenderTexture, nullptr, ColorBuffer, GetWidth() * sizeof(uint32_t));
    SDL_RenderCopy(Renderer, RenderTexture, nullptr, &m_Viewport);


    {  // bloom pass 
        SDL_Texture* LuminosityTexture;

        SDL_Surface* LuminosityMask = SDL_CreateRGBSurfaceWithFormat(0, GetWidth(), GetHeight(), sizeof(uint32_t), SDL_PIXELFORMAT_RGBA8888);
        uint32_t* LuminosityPixels = static_cast<uint32_t*>(LuminosityMask->pixels);
        std::fill_n(LuminosityPixels, GetWidth() * GetHeight(), 0);

        for (size_t i = 0; i < static_cast<size_t>(GetWidth() * GetHeight()); i++) {
            glm::vec3 Pixel = UnpackToVec4(ColorBuffer[i]) * INVERSE_MAX_UINT8;
            if (glm::luminosity(Pixel) >= 0.95f) {
                LuminosityPixels[i] = ColorBuffer[i];
            }
        }

        LuminosityTexture = SDL_CreateTextureFromSurface(Renderer, LuminosityMask);
        SDL_SetTextureScaleMode(LuminosityTexture, SDL_ScaleModeLinear);
  
        SDL_Texture* BlurTexture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, GetWidth(), GetHeight());
        
        for (int i = 1; i < BloomDownsamplePasses; i++) {
            SDL_Texture* LayerTexture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                                                          GetWidth() / std::pow(2, i), GetHeight() / std::pow(2, i));
            SDL_SetRenderTarget(Renderer, LayerTexture);
            SDL_RenderCopy(Renderer, LuminosityTexture, nullptr, nullptr);

            SDL_SetTextureScaleMode(LayerTexture, SDL_ScaleModeLinear);
            SDL_SetTextureBlendMode(LayerTexture, SDL_BLENDMODE_BLEND);

            SDL_SetRenderTarget(Renderer, BlurTexture);
            SDL_RenderCopy(Renderer, LayerTexture, nullptr, nullptr);

            SDL_DestroyTexture(LayerTexture);
        }

        SDL_SetTextureBlendMode(BlurTexture, SDL_BLENDMODE_ADD);
        SDL_SetTextureAlphaMod(BlurTexture, BloomIntensity);
        SDL_SetRenderTarget(Renderer, nullptr);
        SDL_RenderCopy(Renderer, BlurTexture, nullptr, &m_Viewport);

        SDL_DestroyTexture(BlurTexture);
        SDL_DestroyTexture(LuminosityTexture);
    }
  
    SDL_RenderPresent(Renderer);
}

void Context::Clear()
{
    std::fill_n(ColorBuffer, m_Spec.w * m_Spec.h, 0x00000000);
    std::fill_n(DepthBuffer, m_Spec.w * m_Spec.h, INFINITY);
}

void Context::Delete()
{
    delete[] ColorBuffer;
    delete[] DepthBuffer;

    SDL_DestroyWindow(Window);
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyTexture(RenderTexture);
}

Context CreateContext()
{
    if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO) != 0)
    {
        throw std::runtime_error("SDL init failure.");
    }

    glm::ivec2 ViewResolution = GetSettingViewResolution(), RenderResolution = GetSettingRenderResolution();

    SDL_Rect Viewport = {0, 0, ViewResolution.x, ViewResolution.y};
    SDL_DisplayMode Spec = {SDL_PIXELFORMAT_RGBA8888, RenderResolution.x, RenderResolution.y, GetSettingRefreshRate(), 0};

    Context Context = core::Context(Spec, Viewport);

    Context.Window = SDL_CreateWindow(("Mastertools Softengine" + GetAppName()).c_str(), SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED, ViewResolution.x, ViewResolution.y, SDL_WINDOW_RESIZABLE);

    if (!Context.Window)
    {
        throw std::runtime_error("Could not create SDL window.");
    }

    SDL_SetWindowDisplayMode(Context.Window, &Spec);

    Context.Renderer = SDL_CreateRenderer(Context.Window, -1, SDL_RENDERER_ACCELERATED);
    Context.RenderTexture = SDL_CreateTexture(Context.Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                                              RenderResolution.x, RenderResolution.y);

    Context.ColorBuffer = new uint32_t[RenderResolution.x * RenderResolution.y];
    Context.DepthBuffer = new float[RenderResolution.x * RenderResolution.y];

    return Context;
}

} // namespace core

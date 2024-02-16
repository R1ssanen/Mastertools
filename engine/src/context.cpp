#include "context.hpp"

#include "color.hpp"
#include "settings.hpp"
#include "srpch.hpp"
#include "vertex.hpp"
#include "buffer.hpp"

namespace {
    constexpr uint8_t BloomIntensity = 200;
    constexpr int BloomDownsamplePasses = 4;
}

namespace core
{

void Context::Update()
{
    SDL_UpdateTexture(RenderTexture, nullptr, m.Buffer->GetColorBuffer(), GetWidth() * sizeof(uint32_t));
    SDL_SetRenderTarget(Renderer, nullptr);
    SDL_RenderCopy(Renderer, RenderTexture, nullptr, &m.Viewport);

#if BLOOM_ENABLED
    {  // bloom pass 
        SDL_Texture* LuminosityTexture;

        SDL_Surface* LuminosityMask = SDL_CreateRGBSurfaceWithFormat(0, GetWidth(), GetHeight(), sizeof(uint32_t), SDL_PIXELFORMAT_RGBA8888);
        uint32_t* LuminosityPixels = static_cast<uint32_t*>(LuminosityMask->pixels);
        std::fill_n(LuminosityPixels, GetWidth() * GetHeight(), 0);

        for (size_t i = 0; i < static_cast<size_t>(GetWidth() * GetHeight()); i++) {
            glm::vec3 Pixel = UnpackToVec4(ColorBuffer[i]) * INVERSE_MAX_UINT8;
            if (Luminosity(Pixel) >= 0.85f) {
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
#endif

    SDL_RenderCopy(Renderer, DebugTexture, nullptr, &m.Viewport);

    SDL_RenderPresent(Renderer);
    SDL_SetRenderTarget(Renderer, DebugTexture);
    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);
    SDL_RenderClear(Renderer);
}

void Context::Clear()
{
    std::fill_n(m.Buffer->GetColorBuffer(), GetWidth() * GetHeight(), 0x00000000);
    std::fill_n(m.Buffer->GetDepthBuffer(), GetWidth() * GetHeight(), INFINITY);
}

void Context::Delete()
{
    SDL_DestroyWindow(Window);
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyTexture(RenderTexture);
}

Context Context::New()
{
    if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO) != 0)
    {
        throw std::runtime_error("SDL init failure.");
    }

    glm::ivec2 ViewResolution = GetSettingViewResolution(), RenderResolution = GetSettingRenderResolution();

    SDL_Rect Viewport = {0, 0, ViewResolution.x, ViewResolution.y};
    SDL_DisplayMode Spec = {SDL_PIXELFORMAT_RGBA8888, RenderResolution.x, RenderResolution.y, GetSettingRefreshRate(), 0};

    Context Context(
        _M{
            .Buffer = Buffer::New(Spec.w, Spec.h),
            .Spec = Spec,
            .Viewport = Viewport
        }
    );

    Context.Window = SDL_CreateWindow(("Mastertools Softengine" + GetAppName()).c_str(), SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED, ViewResolution.x, ViewResolution.y, SDL_WINDOW_FULLSCREEN_DESKTOP);

    if (!Context.Window)
    {
        throw std::runtime_error("Could not create SDL window.");
    }

    SDL_SetWindowDisplayMode(Context.Window, &Spec);

    Context.Renderer = SDL_CreateRenderer(Context.Window, -1, SDL_RENDERER_ACCELERATED);
    
    Context.RenderTexture = SDL_CreateTexture(Context.Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                                              RenderResolution.x, RenderResolution.y);
    
    Context.DebugTexture = SDL_CreateTexture(Context.Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                                              ViewResolution.x, ViewResolution.y);
    SDL_SetTextureBlendMode(Context.DebugTexture, SDL_BLENDMODE_ADD);
    SDL_SetRenderDrawBlendMode(Context.Renderer, SDL_BLENDMODE_BLEND);

    return Context;
}

} // namespace core

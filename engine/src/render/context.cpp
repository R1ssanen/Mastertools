#include "context.hpp"

#include "../core/buffer.hpp"
#include "../core/color.hpp"
#include "../core/settings.hpp"
#include "../core/vertex.hpp"
#include "../mtpch.hpp"

namespace {
    constexpr mt::u8  BloomIntensity        = 120;
    constexpr mt::i32 BloomDownsamplePasses = 6;
    constexpr mt::u8  MinBloomlayerSize     = 3;
    constexpr mt::f32 BloomThreshold        = 0.7f;
} // namespace

namespace mt {

    void Context::Update() {
        SDL_SetRenderTarget(Renderer, nullptr);
        SDL_UpdateTexture(
            RenderTexture, nullptr, m.Buffer->GetColorBuffer(), m.Buffer->GetWidth() * sizeof(u32)
        );
        SDL_RenderCopy(Renderer, RenderTexture, nullptr, &m.Viewport);

        if (GetBloomEnabled()) { // bloom pass

            static u32* LuminousPixels = new u32[GetWidth() * GetHeight()];
            std::fill_n(LuminousPixels, GetWidth() * GetHeight(), 0U);

            // The mask texture containing the pixels which exceeded the bloom threshold
            static SDL_Texture* BrightnessMask = SDL_CreateTexture(
                Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, GetWidth(),
                GetHeight()
            );
            SDL_SetTextureScaleMode(BrightnessMask, SDL_ScaleModeLinear);

            // The final bloom texture layer to be composited over the image
            static SDL_Texture* BloomTexture = SDL_CreateTexture(
                Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, GetWidth(),
                GetHeight()
            );
            SDL_SetTextureBlendMode(BloomTexture, SDL_BLENDMODE_ADD);
            SDL_SetTextureAlphaMod(BloomTexture, BloomIntensity);

            auto ColorBuffer = m.Buffer->GetColorBuffer();
            for (u64 i = 0; i < static_cast<u64>(GetWidth() * GetHeight()); ++i) {
                glm::vec3 Pixel = UnpackToVec4(ColorBuffer[i]) * INVERSE_MAX_UINT8;

                if (Luminosity(Pixel) >= BloomThreshold) { LuminousPixels[i] = ColorBuffer[i]; }
            }

            // Update the pre-existing brightness mask with the new set of pixels
            SDL_UpdateTexture(BrightnessMask, nullptr, LuminousPixels, GetWidth() * sizeof(u32));

            // Iteratively downscaling the brightness mask and upscaling it to achieve cheap
            // blurring
            for (u32 i = 0; i < BloomDownsamplePasses; ++i) {
                f32 InvDeterminant = 1.f / glm::fastPow(2U, i);
                u32 Width          = static_cast<u32>(GetWidth() * InvDeterminant);
                u32 Height         = static_cast<u32>(GetHeight() * InvDeterminant);

                // Breakoff point, the bloom layer texture size has reached it's minimum size.
                if (Width < MinBloomlayerSize || Height < MinBloomlayerSize) { break; }

                SDL_Texture* LayerTexture = SDL_CreateTexture(
                    Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Width, Height
                );

                SDL_SetRenderTarget(Renderer, LayerTexture);
                SDL_RenderCopy(Renderer, BrightnessMask, nullptr, nullptr);

                SDL_SetTextureScaleMode(LayerTexture, SDL_ScaleModeLinear);
                SDL_SetTextureBlendMode(LayerTexture, SDL_BLENDMODE_BLEND);

                SDL_SetRenderTarget(Renderer, BloomTexture);
                SDL_RenderCopy(Renderer, LayerTexture, nullptr, nullptr);

                SDL_DestroyTexture(LayerTexture);
            }

            SDL_SetRenderTarget(Renderer, nullptr);
            SDL_RenderCopy(Renderer, BloomTexture, nullptr, &m.Viewport);

            SDL_SetRenderTarget(Renderer, BloomTexture);
            SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
            SDL_RenderClear(Renderer);
        }

        SDL_RenderCopy(Renderer, DebugTexture, nullptr, &m.Viewport);

        SDL_RenderPresent(Renderer);

        SDL_SetRenderTarget(Renderer, DebugTexture);
        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);

        SDL_RenderClear(Renderer);
    }

    void Context::Clear() {
        std::fill_n(m.Buffer->GetColorBuffer(), GetWidth() * GetHeight(), 0);
        std::fill_n(m.Buffer->GetDepthBuffer(), GetWidth() * GetHeight(), 1.f);
    }

    void Context::Delete() {
        SDL_DestroyWindow(Window);
        SDL_DestroyRenderer(Renderer);
        SDL_DestroyTexture(RenderTexture);
        SDL_DestroyTexture(DebugTexture);
    }

    Context Context::New() {
        if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO) != 0) {
            throw std::runtime_error("SDL init failure.");
        }

        glm::ivec2 WindowResolution = GetSettingWindowResolution(),
                   RenderResolution = GetSettingRenderResolution();

        SDL_Rect        Viewport    = { 0, 0, WindowResolution.x, WindowResolution.y };
        SDL_DisplayMode Spec = { SDL_PIXELFORMAT_RGBA8888, RenderResolution.x, RenderResolution.y,
                                 GetSettingRefreshRate(), nullptr };

        Context         Context(_M{
                    .Buffer = Buffer::New(Spec.w, Spec.h), .Spec = Spec, .Viewport = Viewport });

        Context.Window = SDL_CreateWindow(
            ("Mastertools Softengine" + GetAppName()).c_str(), SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, WindowResolution.x, WindowResolution.y,
            GetFullscreen() ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0
        );

        if (!Context.Window) { throw std::runtime_error("Could not create SDL window."); }

        SDL_SetWindowDisplayMode(Context.Window, &Spec);

        Context.Renderer      = SDL_CreateRenderer(Context.Window, -1, SDL_RENDERER_ACCELERATED);

        Context.RenderTexture = SDL_CreateTexture(
            Context.Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
            RenderResolution.x, RenderResolution.y
        );

        Context.DebugTexture = SDL_CreateTexture(
            Context.Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
            WindowResolution.x, WindowResolution.y
        );

        SDL_SetTextureBlendMode(Context.DebugTexture, SDL_BLENDMODE_ADD);
        SDL_SetRenderDrawBlendMode(Context.Renderer, SDL_BLENDMODE_BLEND);

        return Context;
    }

} // namespace mt

#include "context.hpp"

#include "color.hpp"
#include "settings.hpp"
#include "srpch.hpp"
#include "vertex.hpp"

namespace core
{

void Context::Update()
{
    SDL_UpdateTexture(RenderTexture, nullptr, ColorBuffer, GetWidth() * sizeof(uint32_t));
    SDL_RenderCopy(Renderer, RenderTexture, nullptr, &m_Viewport);
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

    SDL_Rect Viewport = SDL_Rect{0, 0, ViewResolution.x, ViewResolution.y};
    SDL_DisplayMode Spec = SDL_DisplayMode{SDL_PIXELFORMAT_RGBA8888, RenderResolution.x, RenderResolution.y, 144, 0};

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

#include "context.hpp"

#include "color.hpp"
#include "srpch.hpp"
#include "vertex.hpp"

namespace core {

void Context::Update() {
  SDL_UpdateTexture(RenderTexture, nullptr, ColorBuffer,
                    GetWidth() * sizeof(uint32_t));
  SDL_RenderCopy(Renderer, RenderTexture, nullptr, &m_Viewport);

  SDL_RenderPresent(Renderer);
}

void Context::Clear() {
  std::fill_n(ColorBuffer, m_Spec.w * m_Spec.h, 0x00000000);
  std::fill_n(DepthBuffer, m_Spec.w * m_Spec.h, INFINITY);
}

Context CreateContext() {
  if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO) != 0) {
    throw std::runtime_error("SDL init failure.");
  }

  constexpr int ViewportWidth{1920}, ViewportHeight{1080};
  constexpr int TargetWidth{static_cast<int>(ViewportWidth * 0.5f)},
      TargetHeight{static_cast<int>(ViewportHeight * 0.5f)};

  SDL_Rect Viewport = SDL_Rect{0, 0, ViewportWidth, ViewportHeight};
  SDL_DisplayMode Spec = SDL_DisplayMode{SDL_PIXELFORMAT_RGBA8888, TargetWidth,
                                         TargetHeight, 144, 0};

  Context Context = core::Context(Spec, Viewport);

  Context.Window = SDL_CreateWindow(
      "Mastertools Softengine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      ViewportWidth, ViewportHeight, SDL_WINDOW_RESIZABLE);

  SDL_SetWindowDisplayMode(Context.Window, &Spec);

  Context.Renderer = SDL_CreateRenderer(
      Context.Window, -1, SDL_RENDERER_ACCELERATED);
  Context.RenderTexture =
      SDL_CreateTexture(Context.Renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_STREAMING, TargetWidth, TargetHeight);

  Context.ColorBuffer = new uint32_t[TargetWidth * TargetHeight];
  Context.DepthBuffer = new float[TargetWidth * TargetHeight];

  return Context;
}

}  // namespace core

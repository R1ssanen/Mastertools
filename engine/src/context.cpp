#include "context.hpp"

#include "color.hpp"
#include "srpch.hpp"
#include "vertex.hpp"

namespace core {

Context::Context() {
  if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO) != 0) {
    throw std::runtime_error("SDL init failure.");
  }

  constexpr int ViewportWidth{1920}, ViewportHeight{1080};
  constexpr int TargetWidth{static_cast<int>(ViewportWidth * 0.5f)},
      TargetHeight{static_cast<int>(ViewportHeight * 0.5f)};

  m_Viewport = SDL_Rect{0, 0, ViewportWidth, ViewportHeight};
  m_Spec = SDL_DisplayMode{SDL_PIXELFORMAT_RGBA8888, TargetWidth, TargetHeight,
                           144, 0};

  Window = SDL_CreateWindow("Mastertools Softengine", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, ViewportWidth,
                            ViewportHeight, SDL_WINDOW_RESIZABLE);

  SDL_SetWindowDisplayMode(Window, &m_Spec);

  Renderer = SDL_CreateRenderer(
      Window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
  RenderTexture =
      SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_STREAMING, TargetWidth, TargetHeight);

  ColorBuffer = new uint32_t[TargetWidth * TargetHeight];
  DepthBuffer = new double[TargetWidth * TargetHeight];
}

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

}  // namespace core

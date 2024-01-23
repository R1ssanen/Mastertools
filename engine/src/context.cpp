#include "context.hpp"

#include "glm/gtc/random.hpp"
#include "srpch.hpp"
#include "vertex.hpp"

namespace core {

Context::Context() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    throw std::runtime_error("SDL init failure.");
  }

  constexpr int ViewportWidth{1440}, ViewportHeight{900};
  constexpr int TargetWidth{ViewportWidth * 1},
      TargetHeight{ViewportHeight * 1};

  m_Viewport = SDL_Rect{0, 0, ViewportWidth, ViewportHeight};
  m_Spec = SDL_DisplayMode{SDL_PIXELFORMAT_RGBA8888, TargetWidth, TargetHeight,
                           144, nullptr};

  Window = SDL_CreateWindow("Software Test", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, ViewportWidth,
                            ViewportHeight, SDL_WINDOW_RESIZABLE);

  SDL_SetWindowDisplayMode(Window, &m_Spec);

  Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
  TargetTexture =
      SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STREAMING, TargetWidth, TargetHeight);

  ColorBuffer = new uint32_t[TargetWidth * TargetHeight];
  DepthBuffer = new double[TargetWidth * TargetHeight];
}

void Context::Update() {
  SDL_UpdateTexture(TargetTexture, nullptr, ColorBuffer,
                    m_Spec.w * sizeof(uint32_t));
  SDL_RenderCopy(Renderer, TargetTexture, nullptr, &m_Viewport);
  SDL_RenderPresent(Renderer);

  std::fill_n(ColorBuffer, m_Spec.w * m_Spec.h, 0);
  std::fill_n(DepthBuffer, m_Spec.w * m_Spec.h, INFINITY);
}

}  // namespace core

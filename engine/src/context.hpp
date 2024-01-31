#pragma once

#include "srpch.hpp"

namespace core {

class Context {
 public:
  Context() = default;
  ~Context() {
    delete[] ColorBuffer;
    delete[] DepthBuffer;

    SDL_DestroyWindow(Window);
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyTexture(RenderTexture);
  }

  Context(const SDL_DisplayMode& t_Spec, const SDL_Rect& t_Viewport)
      : m_Spec{t_Spec}, m_Viewport{t_Viewport} {}

  SDL_Window* Window{nullptr};
  SDL_Renderer* Renderer{nullptr};
  SDL_Texture* RenderTexture{nullptr};

  uint32_t* ColorBuffer{nullptr};
  float* DepthBuffer{nullptr};

  const int& GetWidth() const { return m_Spec.w; }
  const int& GetHeight() const { return m_Spec.h; }

  const SDL_DisplayMode& GetSpec() const { return m_Spec; }
  void SetSpec(const SDL_DisplayMode& t_Value) { m_Spec = t_Value; }

  const SDL_Rect& GetViewport() const { return m_Viewport; }
  void SetViewport(int x, int y, int t_Width, int t_Height) {
    m_Viewport = SDL_Rect{x, y, t_Width, t_Height};
  }

  void Update();
  void Clear();

 private:
  SDL_DisplayMode m_Spec;
  SDL_Rect m_Viewport;
};

Context CreateContext();

}  // namespace core

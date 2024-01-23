#pragma once

#include "srpch.hpp"

namespace core {

class Context {
 public:
  SDL_Window* Window{nullptr};
  SDL_Renderer* Renderer{nullptr};
  SDL_Texture* TargetTexture{nullptr};

  uint32_t* ColorBuffer{nullptr};
  double* DepthBuffer{nullptr};

  const int& GetWidth() const { return m_Spec.w; }
  const int& GetHeight() const { return m_Spec.h; }

  const SDL_DisplayMode& GetSpec() const { return m_Spec; }
  void SetSpec(const SDL_DisplayMode& value) { m_Spec = value; }

  const SDL_Rect& GetViewport() const { return m_Viewport; }
  void SetViewport(int x, int y, int width, int height) {
    m_Viewport = SDL_Rect{x, y, width, height};
  }

  void Update();

  Context();
  ~Context() {
    delete[] ColorBuffer;
    delete[] DepthBuffer;
    SDL_DestroyWindow(Window);
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyTexture(TargetTexture);
  };

 private:
  SDL_DisplayMode m_Spec;
  SDL_Rect m_Viewport;
};

}  // namespace core

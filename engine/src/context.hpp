#pragma once

#include "srpch.hpp"

namespace core {

class Context {
 public:
  SDL_Window* Window{nullptr};
  SDL_Renderer* Renderer{nullptr};
  SDL_Texture* RenderTexture{nullptr};

  uint32_t* ColorBuffer{nullptr};
  double* DepthBuffer{nullptr};

  const int& GetWidth() const { return m_Spec.w; }
  const int& GetHeight() const { return m_Spec.h; }

  const SDL_DisplayMode& GetSpec() const { return m_Spec; }
  void SetSpec(const SDL_DisplayMode& value) { m_Spec = value; }

  const SDL_Rect& GetViewport() const { return m_Viewport; }
  void SetViewport(int t_X, int t_Y, int t_Width, int t_Height) {
    m_Viewport = SDL_Rect{t_X, t_Y, t_Width, t_Height};
  }

  void Update();
  void Clear();

  Context();
  ~Context() {
    delete[] ColorBuffer;
    delete[] DepthBuffer;

    SDL_DestroyWindow(Window);
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyTexture(RenderTexture);
  };

 private:
  SDL_DisplayMode m_Spec;
  SDL_Rect m_Viewport;
};

}  // namespace core

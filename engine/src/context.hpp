#pragma once

#include "srpch.hpp"
#include "buffer.hpp"

namespace core
{

class Context
{
  public:
    ~Context() { Delete(); }

    SDL_Window* Window;
    SDL_Renderer* Renderer;
    SDL_Texture* RenderTexture;
    SDL_Texture* DebugTexture;

    buffer_t GetBuffer() const { return m.Buffer; }
    const int& GetWidth() const { return m.Spec.w; }
    const int& GetHeight() const { return m.Spec.h; }
    const SDL_DisplayMode& GetSpec() const { return m.Spec; }
    const SDL_Rect& GetViewport() const { return m.Viewport; }

    void Update();
    void Clear();
    void Delete();

    //Context(const Context& t_Other) { m = t_Other.m; }
    static Context New();

  private:
    struct _M {
      buffer_t Buffer;
      SDL_DisplayMode Spec;
      SDL_Rect Viewport;
    } m;

    explicit Context(_M&& t_Data) : m{std::move(t_Data)} {}
};  

} // namespace core

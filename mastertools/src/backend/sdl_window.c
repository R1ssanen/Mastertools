#include "window.h"

#include <SDL3/SDL.h>

#include "allocator.h"
#include "logging.h"
#include "utility/mstring.h"

struct mt_window
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *target;
};

mt_window *mt_window_create(mt_allocator *alloc, mstring title, int w, int h, int flags)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_VIDEO))
    {
        LERROR("Could not initialize SDL3: %s", SDL_GetError());
        return NULL;
    }

    mt_window *window = allocate(alloc, sizeof(mt_window));

    int sdl_flags = (flags & MT_WINDOW_RESIZABLE) ? SDL_WINDOW_RESIZABLE : 0;
    sdl_flags |= (flags & MT_WINDOW_FULLSCREEN) ? SDL_WINDOW_FULLSCREEN : 0;

    window->window = SDL_CreateWindow(title.str, w, h, sdl_flags);
    if (!window->window)
    {
        LERROR("Could not create SDL3 window: %s", SDL_GetError());
        return NULL;
    }

    window->renderer = SDL_CreateRenderer(window->window, NULL);
    if (!window->renderer)
    {
        LERROR("Could not create SDL3 renderer: %s", SDL_GetError());
        return NULL;
    }

    window->target = SDL_CreateTexture(window->renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, w, h);
    if (!window->target)
    {
        LERROR("Could not create SDL3 texture: %s", SDL_GetError());
        return NULL;
    }

    return window;
}

void mt_window_free(mt_window *window)
{
    SDL_DestroyRenderer(window->renderer);
    SDL_DestroyWindow(window->window);
    SDL_PumpEvents();
    SDL_Quit();
}

void mt_window_resize(mt_window *window, int w, int h)
{
    SDL_SetWindowSize(window->window, w, h);
}

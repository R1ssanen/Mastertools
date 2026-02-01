#include "window.h"

#include <SDL3/SDL.h>

#include "logging.h"
#include "utility/mtstring.h"

struct mt_window
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *target;
};

mt_window *mt_window_create(char *title, int w, int h, enum mt_window_flag flags)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_VIDEO))
    {
        LERROR("Could not initialize SDL3: %s", SDL_GetError());
        return NULL;
    }

    mt_window *window = malloc(sizeof(mt_window));
    if (!window)
    {
        LFATAL("Memory allocation for window failed");
    }

    int sdl_flags = (flags & MT_WINDOW_RESIZABLE) ? SDL_WINDOW_RESIZABLE : 0;
    sdl_flags |= (flags & MT_WINDOW_FULLSCREEN) ? SDL_WINDOW_FULLSCREEN : 0;

    window->window = SDL_CreateWindow(title, 1920, 1080, sdl_flags);
    if (!window->window)
    {
        LERROR("Could not create SDL3 window: %s", SDL_GetError());
        return NULL;
    }

    SDL_SetWindowRelativeMouseMode(window->window, true);

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

    SDL_SetTextureBlendMode(window->target, SDL_BLENDMODE_NONE);
    SDL_SetTextureScaleMode(window->target, SDL_SCALEMODE_NEAREST);

    return window;
}

void mt_window_free(mt_window *window)
{
    SDL_DestroyRenderer(window->renderer);
    SDL_DestroyWindow(window->window);
    SDL_PumpEvents();
    SDL_Quit();

    free(window);
}

void mt_window_resize(mt_window *window, int w, int h)
{
    SDL_SetWindowSize(window->window, w, h);
}

void mt_window_render(mt_window *window, int *pixels, int w)
{
    SDL_UpdateTexture(window->target, NULL, pixels, w * sizeof(int));
    SDL_RenderTexture(window->renderer, window->target, NULL, NULL);
    SDL_RenderPresent(window->renderer);
}

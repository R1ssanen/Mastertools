#include "callbacks.hpp"

#include "application.hpp"
#include "camera.hpp"
#include "keys.hpp"
#include "settings.hpp"
#include "srpch.hpp"

namespace core
{

int ActionCallback(void* t_UserData, SDL_Event* t_Event)
{
    auto App{static_cast<core::App*>(t_UserData)};

    if (GetKey(SDL_SCANCODE_Q) || t_Event->type == SDL_QUIT)
    {
        App->SetRunning(false);
    }

    if (GetKey(SDL_SCANCODE_RALT))
    {
        if (GetKey(SDL_SCANCODE_RSHIFT))
        {
            SDL_SetRelativeMouseMode(SDL_TRUE);
        }

        else
        {
            SDL_SetRelativeMouseMode(SDL_FALSE);
        }
    }

    if (GetKey(SDL_SCANCODE_LCTRL) && GetKey(SDL_SCANCODE_S))
    {
        App->LoadMap();
    }

    if (GetKey(SDL_SCANCODE_RETURN))
    {
        SetWireframe(!GetWireframe());
    }

    if (GetKey(SDL_SCANCODE_DELETE))
    {
        SetShowMipmaps(!GetShowMipmaps());
    }

    return 0;
}

} // namespace core

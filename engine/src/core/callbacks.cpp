#include "callbacks.hpp"

#include "../mtpch.hpp"
#include "application.hpp"
#include "camera.hpp"
#include "keys.hpp"
#include "settings.hpp"

namespace mt {

    i32 ActionCallback(void* UserData, SDL_Event* Event) {
        auto App{ static_cast<mt::App*>(UserData) };

        if (GetKey(SDL_SCANCODE_Q) || Event->type == SDL_QUIT) { App->SetRunning(false); }

        if (GetKey(SDL_SCANCODE_RALT)) {
            if (GetKey(SDL_SCANCODE_RSHIFT)) {
                SDL_SetRelativeMouseMode(SDL_TRUE);
            }

            else {
                SDL_SetRelativeMouseMode(SDL_FALSE);
            }
        }

        if (GetKey(SDL_SCANCODE_RETURN)) { SetWireframe(!GetWireframe()); }

        if (GetKey(SDL_SCANCODE_DELETE)) { SetShowMipmaps(!GetShowMipmaps()); }

        if (GetKey(SDL_SCANCODE_BACKSPACE)) { SetVisualizeTiles(!GetVisualizeTiles()); }

        return 0;
    }

} // namespace mt

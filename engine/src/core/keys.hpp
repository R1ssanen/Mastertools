#pragma once

#include "../mtpch.hpp"

namespace {
    inline const mt::u8* s_Keyboard = SDL_GetKeyboardState(nullptr);
}

namespace mt {

    inline b8                 GetKey(const SDL_Scancode& Key) { return s_Keyboard[Key]; }

    inline std::array<i32, 2> GetDeltaMouse() {
        i32 mx, my;
        SDL_GetRelativeMouseState(&mx, &my);
        return std::array<i32, 2>{ mx, my };
    }

} // namespace mt

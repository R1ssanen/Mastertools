#pragma once

#include "srpch.hpp"

namespace {
inline const uint8_t* s_Keyboard = SDL_GetKeyboardState(nullptr);
}

namespace core {

inline bool GetKey(const SDL_Scancode& t_Key) {
  return s_Keyboard[t_Key];
}

inline std::array<int, 2> GetDeltaMouse() {
  int mx, my;
  SDL_GetRelativeMouseState(&mx, &my);
  return std::array<int, 2>{mx, my};
}

}  // namespace core

#pragma once

#include "srpch.hpp"

namespace core {

int KeyCallback(void* t_UserData, SDL_Event* t_Event);
int MovementCallback(void* t_UserData, SDL_Event* t_Event);
int ActionCallback(void* t_UserData, SDL_Event* t_Event);

}  // namespace core

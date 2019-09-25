#pragma once

#include <SDL2/SDL.h>
#include "global.h"

void r_statusbar(SDL_Renderer* renderer);
void statusbar_useTextConstant(int id, int hasTwoLines);
void statusbar_useTextDynamic(char* str);

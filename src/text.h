#pragma once

#include <SDL2/SDL.h>

#define TEXT_CONSTANT_DEFAULT 31
#define TEXT_CONSTANT_STATUSBARDELETE 32
#define TEXT_CONSTANT_VERTEX 0
#define TEXT_CONSTANT_LINE   1
#define TEXT_CONSTANT_TRIS   2
#define TEXT_CONSTANT_ENTITY   3
#define TEXT_CONSTANT_VERTEX_TOOLTIP  10
#define TEXT_CONSTANT_VERTEX_TOOLTIP2 20
#define TEXT_CONSTANT_LINE_TOOLTIP    11
#define TEXT_CONSTANT_LINE_TOOLTIP2   21
#define TEXT_CONSTANT_TRIS_TOOLTIP    12
#define TEXT_CONSTANT_TRIS_TOOLTIP2   22
#define TEXT_CONSTANT_ENTITY_TOOLTIP    13
#define TEXT_CONSTANT_ENTITY_TOOLTIP2   23
#define TEXT_CONSTANT_TO_TOOLTIP(x) (x + 10)
#define TEXT_CONSTANT_IS_TOOLTIP(x) ((x) >= TEXT_CONSTANT_VERTEX && (x) < 30)

void text_init(SDL_Renderer* renderer);
void text_quit(void);
void r_text_drawDynamic(SDL_Renderer* ren, const char* str, int x, int y);
void r_text_drawConstant(SDL_Renderer* ren, int id, int x, int y, int numoffsetlines);
#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define TEXTURE_NONE       (0)
#define TEXTURE_DRAWLINE   (1)
#define TEXTURE_LINE       (2)
#define TEXTURE_TRIS       (3)
#define TEXTURE_ENTITY     (4)
#define TEXTURE_VERTEX     (5)
#define TEXTURE_ENTITY0    (6)
extern SDL_Texture* textures[8];

void textures_init(SDL_Renderer* renderer);
void r_textures_quit(void);

#include "textures.h"

SDL_Texture* textures[8];

#include <SDL2/SDL_image.h>
void textures_init(SDL_Renderer* renderer)
{
	int i;
	for (i = 0; i < 8; i++)
	{
		char* file = "data/drawline.png";
		switch(i)
		{
			case TEXTURE_DRAWLINE:   file = "data/drawline.png"; break;
			case TEXTURE_LINE:       file = "data/drawline.png"; break;
			case TEXTURE_TRIS:       file = "data/tris.png"; break;
			case TEXTURE_ENTITY:     file = "data/entity.png"; break;
			case TEXTURE_VERTEX:     file = "data/vertex.png"; break;
			case TEXTURE_ENTITY0:    file = "data/ent_q.png"; break;
			default: break;
		}
		SDL_Surface* surf = IMG_Load(file);
		textures[i] = SDL_CreateTextureFromSurface(renderer, surf);
		SDL_FreeSurface(surf);
	}
}

void r_textures_quit(void)
{
	int i;
	for (i = 0; i < 8; i++)
	{
		SDL_DestroyTexture(textures[i]);
		textures[i] = 0;
	}
}

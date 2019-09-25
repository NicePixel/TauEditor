#include <stdio.h>
#include <SDL2/SDL_ttf.h>
#include "text.h"

#define CONSTANT_TEXTS_MAX 64
static TTF_Font* font_small = NULL;
static SDL_Texture* constantTexts[CONSTANT_TEXTS_MAX];

// Using the given font and text, create a texture.
// Returns the pointer to the created texture.
SDL_Texture* createTexture(SDL_Renderer* renderer, const char* str, TTF_Font* font)
{
	SDL_Texture* tex = 0;
	SDL_Surface* surf;
	SDL_Color color = {0xFF, 0xFF, 0xFF, 0xFF};
	
	surf = TTF_RenderText_Solid(font, str, color);
	tex  = SDL_CreateTextureFromSurface(renderer, surf);
	
	SDL_FreeSurface(surf);
	return tex;
}

// Initialize the program for text drawing.
void text_init(SDL_Renderer* renderer)
{
	int i;
	font_small = TTF_OpenFont("data/main.ttf", 16);
	if (!font_small)
	{
		printf("Failed to create font data/main.ttf! %s\n", TTF_GetError());
		return;
	}
	
	// Create textures for constant text
	for (i = 0; i < CONSTANT_TEXTS_MAX; i++)
		constantTexts[i] = NULL;
	constantTexts[TEXT_CONSTANT_DEFAULT]         = createTexture(renderer, "Thank you for running Editor! :)", font_small);
	constantTexts[TEXT_CONSTANT_STATUSBARDELETE] = createTexture(renderer, "><' Why would you delete me?", font_small);
	constantTexts[TEXT_CONSTANT_VERTEX]          = createTexture(renderer, "vertex manip.", font_small);
	constantTexts[TEXT_CONSTANT_LINE]            = createTexture(renderer, "line manip.",   font_small);
	constantTexts[TEXT_CONSTANT_TRIS]            = createTexture(renderer, "triangle tool",   font_small);
	constantTexts[TEXT_CONSTANT_ENTITY]          = createTexture(renderer, "entity tool",   font_small);
	constantTexts[TEXT_CONSTANT_VERTEX_TOOLTIP]  = createTexture(renderer, "Vertex manipulation: LMB to add a vertex;", font_small);
	constantTexts[TEXT_CONSTANT_VERTEX_TOOLTIP2] = createTexture(renderer, "DEL removes the pointed vertex. M to start/stop moving a vertex.", font_small);
	constantTexts[TEXT_CONSTANT_LINE_TOOLTIP]    = createTexture(renderer, "Line manipulation: Select two vertices to form a line.", font_small);
	constantTexts[TEXT_CONSTANT_LINE_TOOLTIP2]   = createTexture(renderer, "S - select; DEL - remove line; F - flip normal.", font_small);
	constantTexts[TEXT_CONSTANT_TRIS_TOOLTIP]    = createTexture(renderer, "Select three vertices to form a triangle.", font_small);
	constantTexts[TEXT_CONSTANT_TRIS_TOOLTIP2]   = createTexture(renderer, "DEL removes the pointed triangle.", font_small);
	constantTexts[TEXT_CONSTANT_ENTITY_TOOLTIP]    = createTexture(renderer, "[ and ] to decrement or increment the placing entity ID.", font_small);
	constantTexts[TEXT_CONSTANT_ENTITY_TOOLTIP2]   = createTexture(renderer, "DEL to delete the pointed entity; M moves the entity.", font_small);
}

void r_text_drawDynamic(SDL_Renderer* renderer, const char* str, int x, int y)
{
	SDL_Rect r;
	SDL_Texture* tex = createTexture(renderer, str, font_small);
	r.x = x; r.y = y;
	SDL_QueryTexture(tex, NULL, NULL, &r.w, &r.h);
	SDL_RenderCopy(renderer, tex, NULL, &r);
	SDL_DestroyTexture(tex);
}

// Draw one of the preloaded textures.
// Use TEXT_CONSTANT_* for parameter 'id'.
void r_text_drawConstant(SDL_Renderer* renderer, int id, int x, int y, int numoffsetlines)
{
	SDL_Rect r;
	SDL_Texture* tex = constantTexts[id];
	
	// Offset Y coorinate by the number of lines we want to skip
	SDL_QueryTexture(tex, NULL, NULL, &r.w, &r.h);
	r.x = x;
	r.y = y + (r.h * numoffsetlines);
	
	SDL_RenderCopy(renderer, tex, NULL, &r);
}

// Free memory used by any created texture.
// Passing NULL to SDL_DestroyTexture() is fine,
// even though SDL_GetError() will return "Invalid texture".
void text_quit(void)
{
	register int i;
	for (i = 0; i < CONSTANT_TEXTS_MAX; i++)
	{
		SDL_DestroyTexture(constantTexts[i]);
		constantTexts[i] = NULL;
	}
}

#include <stdio.h>
#include "statusbar.h"
#include "text.h"
#include "theme.h"

int useConstant     = 1;
int constantId      = TEXT_CONSTANT_DEFAULT;
int tooltipTwoLines = 0;
char* dynamicText   = 0;

void r_statusbar(SDL_Renderer* renderer)
{
	SDL_Rect statusbar;
	statusbar.x = STATUSBAR_START_X;
	statusbar.y = STATUSBAR_START_Y;
	statusbar.w = STATUSBAR_WIDTH;
	statusbar.h = STATUSBAR_HEIGHT;
	
	// Background
	SDL_SetRenderDrawColor(renderer, STATUSBAR_COLOR_BACKGROUND, 0x00);
	SDL_RenderFillRect(renderer, &statusbar);
	
	// Outline
	SDL_SetRenderDrawColor(renderer, STATUSBAR_COLOR_OUTLINE, 0x00);
	SDL_RenderDrawLine(
		renderer,
		STATUSBAR_START_X,
		STATUSBAR_START_Y,
		WINDOW_WIDTH,
		STATUSBAR_START_Y
	);
	
	if (useConstant)
	{
		r_text_drawConstant(renderer,     constantId   , STATUSBAR_START_X, STATUSBAR_START_Y, 0);
		if (tooltipTwoLines)
			r_text_drawConstant(renderer, constantId+10, STATUSBAR_START_X, STATUSBAR_START_Y, 1);
	}
	else
		r_text_drawDynamic(renderer, dynamicText, STATUSBAR_START_X, STATUSBAR_START_Y);
}

void statusbar_useTextConstant(int id, int hasTwoLines)
{
	useConstant     = 1;
	constantId      = id;
	tooltipTwoLines = hasTwoLines;
}

void statusbar_useTextDynamic(char* str)
{
	useConstant = 0;
	dynamicText = str;
}

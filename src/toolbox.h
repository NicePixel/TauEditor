#pragma once

#include <SDL2/SDL.h>
#include "global.h"

typedef struct _BUTTON
{
	SDL_Rect rect;
	int state;
	int texture_small;
	int changesStateTo;
} BUTTON;

BUTTON buttons[TOOLBOX_BUTTON_AMOUNT];

void toolbox_init();
void r_toolbox(SDL_Renderer* renderer);
int  toolbox_getMouseButtonChangeState(int mx, int my);
void toolbox_setButtonSelected(int index);

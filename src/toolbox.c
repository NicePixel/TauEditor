#include "toolbox.h"
#include "theme.h"
#include "textures.h"
#include "global.h"
#include "text.h"

BUTTON buttons[4];

void toolbox_setButtonSelected(int index)
{
	register int i;
	
	for (i = 0; i < TOOLBOX_BUTTON_AMOUNT; i++)
		if (i == index)
			buttons[i].state = TOOLBOX_BUTTON_STATE_ON;
		else
			buttons[i].state = TOOLBOX_BUTTON_STATE_NONE;
	
}


void toolbox_init(void)
{
	buttons[0].rect.x = TOOLBOX_BUTTON_MARGIN;
	buttons[0].rect.y = TOOLBOX_BUTTON_MARGIN;
	buttons[0].rect.w = TOOLBOX_BUTTON_WIDTH;
	buttons[0].rect.h = TOOLBOX_BUTTON_HEIGHT;
	buttons[0].state  = TOOLBOX_BUTTON_STATE_NONE;
	buttons[0].texture_small  = TEXTURE_VERTEX;
	buttons[0].changesStateTo = STATE_ADDVERTEX;
	
	buttons[1].rect.x = TOOLBOX_BUTTON_MARGIN;
	buttons[1].rect.y = TOOLBOX_BUTTON_MARGIN + buttons[0].rect.y + TOOLBOX_BUTTON_HEIGHT;
	buttons[1].rect.w = TOOLBOX_BUTTON_WIDTH;
	buttons[1].rect.h = TOOLBOX_BUTTON_HEIGHT;
	buttons[1].state  = TOOLBOX_BUTTON_STATE_NONE;
	buttons[1].texture_small  = TEXTURE_LINE;
	buttons[1].changesStateTo = STATE_ADDLINE;
	
	buttons[2].rect.x = TOOLBOX_BUTTON_MARGIN;
	buttons[2].rect.y = TOOLBOX_BUTTON_MARGIN + buttons[1].rect.y + TOOLBOX_BUTTON_HEIGHT;
	buttons[2].rect.w = TOOLBOX_BUTTON_WIDTH;
	buttons[2].rect.h = TOOLBOX_BUTTON_HEIGHT;
	buttons[2].state  = TOOLBOX_BUTTON_STATE_NONE;
	buttons[2].texture_small = TEXTURE_TRIS;
	buttons[2].changesStateTo = STATE_TRIS;

	buttons[3].rect.x = TOOLBOX_BUTTON_MARGIN;
	buttons[3].rect.y = TOOLBOX_BUTTON_MARGIN + buttons[2].rect.y + TOOLBOX_BUTTON_HEIGHT;
	buttons[3].rect.w = TOOLBOX_BUTTON_WIDTH;
	buttons[3].rect.h = TOOLBOX_BUTTON_HEIGHT;
	buttons[3].state  = TOOLBOX_BUTTON_STATE_NONE;
	buttons[3].texture_small  = TEXTURE_ENTITY;
	buttons[3].changesStateTo = STATE_ENTITY;
}

#include "mapview.h" // For MAPVIEW_START_X
void r_toolbox(SDL_Renderer* renderer)
{
	register int i;
	
	// Draw background
	for (i = WINDOW_HEIGHT/2; i < WINDOW_HEIGHT; i++)
	{
		int a = ((float)(i-WINDOW_HEIGHT/2) / WINDOW_HEIGHT) * 256;
		SDL_SetRenderDrawColor(renderer, 0, a/3, a, 0xFF);
		SDL_RenderDrawLine(renderer, 0, i, MAPVIEW_START_X, i);
	}
	
	// Draw every button
	for (i = 0; i < TOOLBOX_BUTTON_AMOUNT; i++)
	{
		SDL_Rect icon = buttons[i].rect;
		icon.x += TOOLBOX_BUTTON_ICON_MARGIN;
		icon.y += TOOLBOX_BUTTON_ICON_MARGIN;
		icon.w = 16;
		icon.h = 16;
		
		// Button background
		if (buttons[i].state == TOOLBOX_BUTTON_STATE_NONE)
			SDL_SetRenderDrawColor(renderer, TOOLBOX_COLOR_BUTTON_BACKGROUND, 0x00);
		else if (buttons[i].state == TOOLBOX_BUTTON_STATE_ON)
			SDL_SetRenderDrawColor(renderer, TOOLBOX_COLOR_BUTTON_BACKGROUND_ON, 0x00);
		SDL_RenderFillRect(
			renderer,
			&buttons[i].rect
		);
		
		// Button icon
		if (buttons[i].texture_small != TEXTURE_NONE)
		{
			SDL_RenderCopy(
				renderer,
				textures[buttons[i].texture_small],
				0,
				&icon
			);
		}
		
		// Button text
		// Assuming text's texture ID is same as 'i'.
		r_text_drawConstant(renderer, i, icon.x + icon.w, icon.y, 0);
	}
}

int toolbox_getMouseButtonChangeState(int mx, int my)
{
	int i;
	int newstate = 0;
	
	// Check if the coordinates are inside the button
	for (i = 0; i < TOOLBOX_BUTTON_AMOUNT; i++)
	{
		SDL_Rect r = buttons[i].rect;
		if (mx < r.x + r.w && mx > r.x && my < r.y + r.h && my > r.y)
		{
			buttons[i].state = TOOLBOX_BUTTON_STATE_ON;
			newstate         = buttons[i].changesStateTo;
		}
	}
	
	// If the coordintes are inside a button, other buttons should
	// change state.
	if (newstate)
		for (i = 0; i < TOOLBOX_BUTTON_AMOUNT; i++)
			if (buttons[i].changesStateTo != newstate)
				buttons[i].state = TOOLBOX_BUTTON_STATE_NONE;
	
	return newstate;
}

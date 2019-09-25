#pragma once

#include <SDL2/SDL.h>
#include "map.h"
#include "global.h"

extern int grid_offset_x;
extern int grid_offset_y;
extern int grid_state_divby;

void mapview_snaptogrid(int* x, int* y);
void mapview_snaptogrid_center(int* x, int* y);
int  mapview_relativeCoord(int x);
void mapview_line_newVertex(VERTEX v);
void mapview_line_reset(void);
void mapview_line_select(int line_id);
void mapview_line_deselect(void);
int  mapview_line_getSelectedID(void);
void mapview_movevertex_start(int id);
void mapview_movevertex(int mx, int my);
void mapview_movevertex_stop(void);
int mapview_movevertex_running(void);
void mapview_tri_newVertex(int v_id);
void mapview_tri_reset(void);
void mapview_moveentity_start(int id);
void mapview_moveentity(int mx, int my);
void mapview_moveentity_stop(void);
int mapview_moveentity_running(void);

void r_mapview(SDL_Renderer* renderer, int state);
void r_vertex(SDL_Renderer* renderer, int mx, int my, int color);
void r_line(SDL_Renderer* renderer, int vid0, int vid1, int color);
void r_mapview_info_entity(SDL_Renderer* renderer, int selected_eid);
void r_mapview_info_vertex(SDL_Renderer* renderer);
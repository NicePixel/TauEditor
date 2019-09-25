#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "mapview.h"
#include "theme.h"

static VERTEX line_v0, line_v1;        // Vertices used for drawing a line
static int tri_v0, tri_v1, tri_v2;     // Vertices used for drawing a triangle
static int line_c, tri_c;              // Current vertex we are adding
static int movingvertex_id = -1;       // The vertex we should be moving with the mouse. -1 if we are not moving anything.
static int selectedline_id = -1;
static int movingentity_id = -1;
int grid_offset_x = 0;
int grid_offset_y = 0;
int grid_state_divby = 1;

void mapview_moveentity_start(int id)
{
	movingentity_id = id;
}
void mapview_moveentity(int mx, int my)
{
	ENTITY* e = NULL;
	if (movingentity_id < 0)
	{
		puts("mapview_moveentity() called but movingentity_id is < 0");
		return;
	}
	mx -= MAPVIEW_START_X;
	mapview_snaptogrid(&mx, &my);
	
	e = map_entity_getPointerFromID(movingentity_id);
	e->x = mx;
	e->y = my;
}
void mapview_moveentity_stop(void)
{
	movingentity_id = -1;
	
}

int mapview_moveentity_running(void)
{
	return (movingentity_id != -1);
}

void mapview_tri_reset(void)
{
	tri_v0 = tri_v1 = tri_v2 = tri_c = 0;
}

void mapview_tri_newVertex(int v_id)
{
	tri_c++;
	switch(tri_c)
	{
		case 1: tri_v0 = v_id; break;
		case 2: tri_v1 = v_id; break;
		case 3:
			tri_v2 = v_id; 
			{
				TRI t;
				t.v_id0 = tri_v0;
				t.v_id1 = tri_v1;
				t.v_id2 = tri_v2;
				t.id    = map_triNextID;
				map_tri_push(t);
			}
			mapview_tri_reset();
		default:
			break;
	}
}

int  mapview_line_getSelectedID(void)
{
	return selectedline_id;
}

// Snap the given coordinates to the grid.
void mapview_snaptogrid(int* x, int* y)
{
	int sx = MAPVIEW_GRID_SIZE_X / grid_state_divby;
	int sy = MAPVIEW_GRID_SIZE_Y / grid_state_divby;
	*x = (((*x)+sx/2)/sx)*sx;
	*y = (((*y)+sy/2)/sy)*sy;
}
void mapview_snaptogrid_center(int* x, int* y)
{
	int sx = MAPVIEW_GRID_SIZE_X / grid_state_divby;
	int sy = MAPVIEW_GRID_SIZE_Y / grid_state_divby;
	*x = (((*x)+sx/2)/sx)*sx - 0;
	*y = (((*y)+sy/2)/sy)*sy - 0;
}

// 
void mapview_movevertex_start(int id)
{
	movingvertex_id = id;
}

void mapview_movevertex(int mx, int my)
{
	VERTEX* v = NULL;
	if (movingvertex_id < 0)
	{
		puts("mapview_movevertex() called but movingvertex_id is < 0");
		return;
	}
	mx -= MAPVIEW_START_X;
	mapview_snaptogrid(&mx, &my);
	
	v = map_vertex_getPointerFromID(movingvertex_id);
	v->x = mx;
	v->y = my;
}

void mapview_movevertex_stop(void)
{
	movingvertex_id = -1;
}

int mapview_movevertex_running(void)
{
	return (movingvertex_id != -1);
}

// Since the mapview is offset by MAPVIEW_START_X, this returns the coordinate 
// relative to the mapview.
int mapview_relativeCoord(int x)
{
	return x - MAPVIEW_START_X;
}

void mapview_line_reset(void)
{
	line_c = 0;
}

void mapview_line_newVertex(VERTEX v)
{
	if (line_c == 0)
		line_v0 = v;
	else
		line_v1 = v;
	
	// Check if we have selected two verices
	line_c++;
	if (line_c == 2)
	{
		LINE newline;
		newline.v_id0 = line_v0.id;
		newline.v_id1 = line_v1.id;
		newline.id    = map_lineNextID;
		newline.selected = 0;
		line_c        = 0;
		map_line_push(newline);
	}
}

void mapview_line_select(int line_id)
{
	LINE* l = map_line_getFromID(line_id);
	l->selected = 1;
	selectedline_id = line_id;
}

void mapview_line_deselect(void)
{
	if (selectedline_id == -1)
		return;
	LINE* l = map_line_getFromID(selectedline_id);
	if(l)
		l->selected = 0;
	selectedline_id = -1;
}

// r_grid
// Draw map view's grid
void r_grid(SDL_Renderer* renderer)
{
	int i;
	SDL_SetRenderDrawColor(renderer, MAPVIEW_COLOR_GRID, 0x00);
	
	// Horizontals
	for (i = 0; i < MAPVIEW_HEIGHT; i += MAPVIEW_GRID_SIZE_Y/grid_state_divby)
	{
		SDL_RenderDrawLine(
			renderer,
			MAPVIEW_START_X,
			i,
			WINDOW_WIDTH,
			i
		);
	}
	
	// Verticals
	for (i = MAPVIEW_START_X; (i - MAPVIEW_START_X) < MAPVIEW_WIDTH; i += MAPVIEW_GRID_SIZE_X/grid_state_divby)
	{
		SDL_RenderDrawLine(
			renderer,
			i,
			0,
			i,
			MAPVIEW_HEIGHT
		);
	}
}

#include "text.h"
#include "textures.h"
// r_mapview
// Render the whole map view.
void r_mapview(SDL_Renderer* renderer, int thestate)
{
	int i;
	SDL_Rect background;
	background.x = MAPVIEW_START_X;
	background.y = 0;
	background.w = MAPVIEW_WIDTH;
	background.h = MAPVIEW_HEIGHT;

	// Background
	SDL_SetRenderDrawColor(renderer, MAPVIEW_COLOR_BACKGROUND, 0x00);
	SDL_RenderFillRect(renderer, &background);
	
	// Grid
	r_grid(renderer);
	
	// Triangles
	for (i = 0; i < map_triAmount; i++)
	{
		TRI t      = map_tris[i];
		VERTEX v0  = map_vertex_getFromID(t.v_id0);
		VERTEX v1  = map_vertex_getFromID(t.v_id1);
		VERTEX v2  = map_vertex_getFromID(t.v_id2);
		Sint16* vx = malloc(sizeof(Sint16) * 3);
		Sint16* vy = malloc(sizeof(Sint16) * 3);
		vx[0] = v0.x + MAPVIEW_START_X; vx[1] = v1.x + MAPVIEW_START_X; vx[2] = v2.x + MAPVIEW_START_X;
		vy[0] = v0.y;                   vy[1] = v1.y;                   vy[2] = v2.y;
		filledPolygonColor(renderer, vx, vy, 3, 0x800000FF);
		free(vx);
		free(vy);
	}
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	
	// Entities
	for (i = 0; i < map_entityAmount; i++)
	{
		char label[4];
		ENTITY e = map_entities[i];
		int eAngleMulX, eAngleMulY;
		SDL_Rect r;
		r.x = e.x + MAPVIEW_START_X;
		r.y = e.y;
		r.w = 8;
		r.h = 8;
		sprintf(label, "%d", e.eid);
		SDL_RenderCopy(
			renderer,
			textures[TEXTURE_ENTITY0],
			0,
			&r
		);
		if (thestate == STATE_ENTITY)
			r_text_drawDynamic(renderer, label, r.x, r.y);
		
		SDL_SetRenderDrawColor(renderer, MAPVIEW_COLOR_ENTITYANGLE, 0x00);
		SDL_RenderDrawLine(
			renderer,
			r.x + 4,
			r.y + 4,
			r.x + 4 + cos(3.1415f / 180.0f * (float)(e.angle      )) * 8,
			r.y + 4 + sin(3.1415f / 180.0f * (float)(e.angle - 180)) * 8
		);
	}
	
	// The X coordinate is relative to the map view, to render it properly
	// we add the offset of the mapview.
	// Vertices
	for (i = 0; i < map_vertexAmount; i++)
	{
		char label[4];
		VERTEX v = map_vertices[i];
		sprintf(label, "%d", v.id);
		r_vertex(renderer, v.x + MAPVIEW_START_X, v.y, 0);
		if (thestate == STATE_ADDVERTEX)
			r_text_drawDynamic(renderer, label, v.x + MAPVIEW_START_X, v.y);
	}
	
	// Lines
	for (i = 0; i < map_lineAmount; i++)
	{
		LINE l = map_lines[i];
		r_line(renderer, l.v_id0, l.v_id1, l.selected);
	}
	
	// Outline
	SDL_SetRenderDrawColor(renderer, MAPVIEW_COLOR_OUTLINE, 0x00);
	SDL_RenderDrawLine(
		renderer,
		MAPVIEW_START_X,
		0,
		MAPVIEW_START_X,
		WINDOW_HEIGHT
	);
}

// Primary for rendering LINE.
// 
// We have to add MAPVIEW_START_X to the X coordinate since
// the mapview is offset by that amount.
// 
// if color is not 0, we choose the color for drawing a line,
// else we use color for drawn line.
void r_line(SDL_Renderer* renderer, int vid0, int vid1, int color)
{
	VERTEX v0  = map_vertex_getFromID(vid0);
	VERTEX v1  = map_vertex_getFromID(vid1);
	// Calculate the normal vector components
	double dx  = v0.x - v1.x;
	double dy  = v0.y - v1.y;
	double mag = sqrt(dx*dx + dy*dy);
	double nx  = dy/mag;
	double ny  = -dx/mag;
	// The center of the line
	int cx     = (v0.x + v1.x) / 2;
	int cy     = (v0.y + v1.y) / 2;

	if (color)
		SDL_SetRenderDrawColor(renderer, MAPVIEW_COLOR_LINESELECTION, 0x00);
	else
		SDL_SetRenderDrawColor(renderer, MAPVIEW_COLOR_LINE, 0x00);
	SDL_RenderDrawLine(
		renderer,
		v0.x + MAPVIEW_START_X,
		v0.y,
		v1.x + MAPVIEW_START_X,
		v1.y
	);
	
	SDL_SetRenderDrawColor(renderer, MAPVIEW_COLOR_LINENORMAL, 0x00);
	SDL_RenderDrawLine(
		renderer,
		cx + MAPVIEW_START_X,
		cy,
		cx + nx*MAPVIEW_LINE_NORMAL_LENGTH + MAPVIEW_START_X,
		cy + ny*MAPVIEW_LINE_NORMAL_LENGTH
	);
	
	
}

// Primary for rendering a VERTEX.
// Vertex is represented as a small square in the view.
//
// if color is not 0, we choose the color for placing a vertex,
// else we use color for placed vertex.
void r_vertex(SDL_Renderer* renderer, int mx, int my, int color)
{
	SDL_Rect selection;
	
	if (!COORD_ON_MAPVIEW(mx, my))
		return;
	
	mapview_snaptogrid(&mx, &my);
	selection.x = mx-2;
	selection.y = my-2;
	selection.w = 4;
	selection.h = 4;
	
	if (color)
		SDL_SetRenderDrawColor(renderer, MAPVIEW_COLOR_VERTEXSELECTION, 0x00);
	else
		SDL_SetRenderDrawColor(renderer, MAPVIEW_COLOR_VERTEX, 0x00);
	SDL_RenderFillRect(
		renderer,
		&selection
	);
}

void r_mapview_info_entity(SDL_Renderer* renderer, int selected_eid)
{
	char label[32];
	SDL_Rect r;
	r.x = MAPVIEW_START_X + 2;
	r.y = 2;
	r.w = 32;
	r.h = 16;
	
	sprintf(label, "%s%d", "Selected entity ID:", selected_eid);
	r_text_drawDynamic(renderer, label, r.x, r.y);
}

void r_mapview_info_vertex(SDL_Renderer* renderer)
{
	
}

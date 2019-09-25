#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "global.h"
#include "mapview.h"
#include "toolbox.h"
#include "textures.h"
#include "map.h"
#include "text.h"
#include "statusbar.h"
#include "io.h"

SDL_Window*   window;
SDL_Renderer* renderer;
SDL_Surface* surface;
SDL_Cursor* cursor_cross;
SDL_Cursor* cursor_arrow;
int state = STATE_NONE;
int selected_entity_eid = 0;
int angle_entity_change = 0;

// Called when the state changes
void resetState(void)
{
	mapview_line_reset();
	mapview_movevertex_stop();
	mapview_moveentity_stop();
	mapview_line_deselect();
	mapview_tri_reset();
	if (state == STATE_ENTITY)
		grid_state_divby = 2;
	else 
		grid_state_divby = 1;
}

// alt_function = 0 for STATE_ADDVERTEX is adding a vertex.
// alt_function = 1 for STATE_ADDVERTEX is deleting a vertex.
// alt_function = 2 for STATE_ADDVERTEX is moving a vertex.
// alt_function = 0 for STATE_ADDLINE   is drawing a line (adding a vertex if no vertex is selected).
// alt_function = 1 for STATE_ADDLINE   is deleting the selected line.
// alt_function = 2 for STATE_ADDLINE   is line selection.
// alt_function = 3 for STATE_ADDLINE   is flipping normal.
// alt_function = 0 for STATE_TRIS      is selecting vertices for creating a triangle.
// alt_function = 1 for STATE_TRIS      is deleting the pointing triangle.
// alt_function = 0 for STATE_ENTITY    is creating an entity.
// alt_function = 1 for STATE_ENTITY    is moving an entity
// alt_function = 2 for STATE_ENTITY    is deleting the selected entity.
void handleMouse_mapview(int mx, int my, int alt_function)
{
	int wasMoving    = mapview_movevertex_running();
	int wasMovingEnt = mapview_moveentity_running();
	int snap_x  = mx, snap_y  = my;
	int previouslySelectedLine = mapview_line_getSelectedID();
	mapview_snaptogrid(&snap_x, &snap_y);
	
	// If we are not deleting the selected line...
	if (!(alt_function == 3 && state == STATE_ADDLINE))
		mapview_line_deselect();
	
	// If we were moving a vertex/entity...
	if (wasMoving)
		mapview_movevertex_stop();	
	if (wasMovingEnt)
		mapview_moveentity_stop();
	
	switch(state)
	{
		case STATE_ADDVERTEX:
		{
			int isDuplicate;
			VERTEX v;
			v.id = map_vertexNextID;
			v.x  = mapview_relativeCoord(snap_x);
			v.y  = snap_y;
			
			isDuplicate = map_vertex_isduplicate(v, &v.id);
			
			if (alt_function == 1 && isDuplicate && !wasMoving)
				map_vertex_delete(v.id);
			else if (alt_function == 0 && !isDuplicate)
				map_vertex_push(v);
			else if (alt_function == 2 && isDuplicate)
				mapview_movevertex_start(v.id);
			break;
		}
		case STATE_ADDLINE:
		{
			// The selected vertex
			VERTEX v;
			v.id = 0;
			v.x  = mapview_relativeCoord(snap_x);
			v.y  = snap_y;
			
			// Vertex already exists for the new line
			if (alt_function == 0 && map_vertex_isduplicate(v, &v.id))
			{
				VERTEX p = map_vertex_getFromID(v.id);
				mapview_line_newVertex(p);
			}
			// Add vertex for new the line
			else if (alt_function == 0)
			{
				v.id = map_vertexNextID;
				map_vertex_push(v);
				mapview_line_newVertex(v);
			}
			// Delete selected line
			else if (alt_function == 1)
			{
				int lid = previouslySelectedLine;
				if (lid != -1)
				{
					LINE* l  = map_line_getFromID(lid);
					map_line_delete(l->id);
				}
			}
			// Attempt to find the selected line and mark it for selection.
			else if (alt_function == 2)
			{
				LINE* sel_line = map_line_getLineFromPoint(mapview_relativeCoord(mx), my);
				if(sel_line)
					mapview_line_select(sel_line->id);
			}
			// Attempt to find the selected line and mark it for selection.
			else if (alt_function == 3)
			{
				if (previouslySelectedLine > -1)
				{
					LINE* sel_line = map_line_getFromID(previouslySelectedLine);
					if (sel_line)
					{
						int v = sel_line->v_id0;
						sel_line->v_id0 = sel_line->v_id1;
						sel_line->v_id1 = v;
					}
				}
			}
			
			break;
		}
		case STATE_TRIS:
		{
			// The selected vertex
			VERTEX v;
			v.id = 0;
			v.x  = mapview_relativeCoord(snap_x);
			v.y  = snap_y;
			
			// Check existance of a vertex at the given point
			if (alt_function == 0 && map_vertex_isduplicate(v, &v.id))
			{
				// Push the original vertex as a point for the triangle
				mapview_tri_newVertex(v.id);
			}
			if (alt_function == 1)
			{
				int tid = map_tri_getTriIDByCoord(mapview_relativeCoord(mx) , my);
				if (tid)
					map_tri_deleteByID(tid);
			}
			break;
		}
		case STATE_ENTITY:
			{
				int grid_x = mx;
				int grid_y = my;
				mapview_snaptogrid_center(&grid_x, &grid_y);
				if (alt_function == 0)
				{
					ENTITY e;
					e.x     = mapview_relativeCoord(snap_x);
					e.y     = snap_y;
					e.id    = map_entityNextID;
					e.eid   = selected_entity_eid;
					e.flags = 0;
					e.angle = 0;
					
					if (!map_entity_existsAtPos(e.x, e.y, NULL))
						map_entity_push(e);
				}
				else if (alt_function == 1)
				{
					ENTITY e;
					e.x     = mapview_relativeCoord(snap_x);
					e.y     = snap_y;
					e.id    = map_entityNextID;
					e.eid   = selected_entity_eid;
					e.flags = 0;
					e.angle = 0;
					
					if (map_entity_existsAtPos(e.x, e.y, &e))
						mapview_moveentity_start(e.id);
				}
				else if (alt_function == 2)
				{
					ENTITY e;
					e.x     = mapview_relativeCoord(snap_x);
					e.y     = snap_y;
					e.id    = map_entityNextID;
					e.eid   = selected_entity_eid;
					e.flags = 0;
					e.angle = 0;
					if (map_entity_existsAtPos(e.x, e.y, &e))
						map_entity_delete(e.id);
				}
				else if (alt_function == 3)
				{
					ENTITY e;
					e.x     = mapview_relativeCoord(snap_x);
					e.y     = snap_y;
					e.id    = map_entityNextID;
					e.eid   = selected_entity_eid;
					e.flags = 0;
					e.angle = 0;
					if (map_entity_existsAtPos(e.x, e.y, &e))
						map_entity_changeAngle(e.id, angle_entity_change);
				}
			}
		break;
		default:
			printf("State %d is not implemented!\n", state);
		break;
	}
}

void changeStateTo(int newstate)
{
	if (state == newstate)
		return;
	
	state = newstate;
	resetState();
	toolbox_setButtonSelected(newstate - 1);
	statusbar_useTextConstant(TEXT_CONSTANT_TO_TOOLTIP(newstate - 1), 1);
	r_statusbar(renderer);
}

// Knot
// ===================
void knot(void)
{
	int running = 1;            // Is the software running?
	int old_mx = 0, old_my = 0; // Old mouse position
	int old_state = state;      // Old state;
	
	// Set up renderer
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(renderer);
	
	// Render toolbox since it's only rendered when its state changes.
	// Render statusbar for same reason
	r_toolbox(renderer);
	r_statusbar(renderer);
	
	// Setup (testing) stuff
	puts("--------------");
	puts("START OF TESTS");
	{
		// Put something here, lol
	}
	puts("END  OF  TESTS");
	puts("--------------");
	
	while (running)
	{
		int mx, my;
		SDL_Event e;
		
		// Events
		SDL_GetMouseState(&mx, &my);
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				running = 0;
			// Mouse press
			else if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				// Mouse is on mapview.
				if (COORD_ON_MAPVIEW(mx, my))
					handleMouse_mapview(mx, my, 0);
				// Mouse is on toolbox (or statusbar).
				else
				{
					register int tmp;
					tmp = toolbox_getMouseButtonChangeState(mx, my);
					if (tmp)
					{
						state = tmp;
						resetState();
						statusbar_useTextConstant(TEXT_CONSTANT_TO_TOOLTIP(tmp - 1), 1);
						r_statusbar(renderer);
					}
				}
			}
			// Key press
			else if (e.type == SDL_KEYDOWN)
			{
				switch(e.key.keysym.sym)
				{
					case SDLK_UP:
						angle_entity_change = 90;
						if (state == STATE_ENTITY && COORD_ON_MAPVIEW(mx, my))
							handleMouse_mapview(mx, my, 3);
						break;
					case SDLK_DOWN:
						angle_entity_change = 270;
						if (state == STATE_ENTITY && COORD_ON_MAPVIEW(mx, my))
							handleMouse_mapview(mx, my, 3);
						break;
					case SDLK_LEFT:
						angle_entity_change = 180;
						if (state == STATE_ENTITY && COORD_ON_MAPVIEW(mx, my))
							handleMouse_mapview(mx, my, 3);
						break;
					case SDLK_RIGHT:
						angle_entity_change = 0;
						if (state == STATE_ENTITY && COORD_ON_MAPVIEW(mx, my))
							handleMouse_mapview(mx, my, 3);
						break;
					case SDLK_F2:
						io_write_start();
					break;
					case SDLK_1:
						changeStateTo(STATE_ADDVERTEX);
					break;
					case SDLK_2:
						changeStateTo(STATE_ADDLINE);
					break;
					case SDLK_3:
						changeStateTo(STATE_TRIS);
					break;
					case SDLK_LEFTBRACKET:
						if (state == STATE_ENTITY)
							selected_entity_eid--;
					break;
					case SDLK_RIGHTBRACKET:
						if (state == STATE_ENTITY)
							selected_entity_eid++;
					break;
					case SDLK_m:
					if (state == STATE_ADDVERTEX)
					{
						if (COORD_ON_MAPVIEW(mx, my))
							handleMouse_mapview(mx, my, 2);
					}
					else if (state == STATE_ENTITY)
					{
						if (COORD_ON_MAPVIEW(mx, my))
							handleMouse_mapview(mx, my, 1);
					}
					break;
					case SDLK_s:
					if (state == STATE_ADDLINE)
					{
						if (COORD_ON_MAPVIEW(mx, my))
							handleMouse_mapview(mx, my, 2);
					}
					break;
					case SDLK_f:
					if (state == STATE_ADDLINE)
					{
						if (COORD_ON_MAPVIEW(mx, my))
							handleMouse_mapview(mx, my, 3);
					}
					break;
					case SDLK_DELETE:
					if ((state == STATE_ADDVERTEX || state == STATE_ADDLINE) && COORD_ON_MAPVIEW(mx, my))
						handleMouse_mapview(mx, my, 1);
					else if (state == STATE_ENTITY && COORD_ON_MAPVIEW(mx, my))
						handleMouse_mapview(mx, my, 2);
					else if (state == STATE_TRIS && COORD_ON_MAPVIEW(mx, my))
						handleMouse_mapview(mx, my, 1);
					else if (COORD_ON_STATUSBAR(mx, my))
					{
						statusbar_useTextConstant(TEXT_CONSTANT_STATUSBARDELETE, 0);
						r_statusbar(renderer);
					}
					default:
					break;
				}
			}
		}
		
		// If the mouse doesn't move, we don't do anything.
		// Yield the software for some time, dont use resources.
		if (old_mx == mx && old_my == my)
			SDL_Delay(40);
		
		if (COORD_ON_MAPVIEW(mx ,my))
			SDL_SetCursor(cursor_cross);
		else
			SDL_SetCursor(cursor_arrow);
		
		// Render
		// ------
		// Redraw toolbox if its state has changed.
		if (old_state != state)
			r_toolbox(renderer);
		
		// Render the map view
		r_mapview(renderer, state);
		if (state == STATE_ENTITY)
			r_mapview_info_entity(renderer, selected_entity_eid);
		
		// State specific
		switch(state)
		{
			case STATE_ADDVERTEX:
				if (mapview_movevertex_running() && COORD_ON_MAPVIEW(mx, my))
					mapview_movevertex(mx, my);
			case STATE_ADDLINE:
			case STATE_TRIS:
				r_vertex(renderer, mx, my, 1);
				break;
			case STATE_ENTITY:
				if (mapview_moveentity_running() && COORD_ON_MAPVIEW(mx, my))
					mapview_moveentity(mx, my);
			default:
				break;
		}
		
		SDL_UpdateWindowSurface(window);
		SDL_Delay(40);
		
		old_mx    = mx;
		old_my    = my;
		old_state = state;
	}
}

// Main
// ===================
int main(int argc, char** argv)
{
	puts("!!! Editor !!!");
	puts("==============");
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	
	puts("Creating the main window...");
	window = SDL_CreateWindow(
		SOFTWARE_TITLE,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN
	);
	if (!window)
	{
		printf("Error creating window! %s\n", SDL_GetError());
		return 1;
	}
	
	puts("Initializing the renderer...");
	surface  = SDL_GetWindowSurface(window);
	renderer = SDL_CreateSoftwareRenderer(surface);
	if (!renderer)
	{
		printf("Error creating renderer! %s\n", SDL_GetError());
		return 2;
	}
	
	// Set up the cursors
	cursor_cross = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
	cursor_arrow  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	
	puts("Initializing textures...");
	textures_init(renderer);
	puts("Initializing fonts...");
	text_init(renderer);
	puts("Initializing toolbox...");
	toolbox_init();
	puts("Initializing map...");
	map_init();
	if (argc > 1)
		io_read_start(argv[1]);
	puts("Entering knot() subroutine!!!");
	knot();
	
	puts("Freeing memory...");
	map_vertex_deleteAll();
	map_line_deleteAll();
	map_sector_deleteAll();
	text_quit();
	r_textures_quit();
	SDL_FreeCursor(cursor_cross);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	puts("Goodbye.");
	return 0;
}
#pragma once

#define SECTOR_MAX_LINES  32

typedef struct _VERTEX
{
	int id;
	int y;
	int x;
} VERTEX;

typedef struct _LINE
{
	int v_id0;
	int v_id1;
	int id;
	int selected;
} LINE;

typedef struct _TRI
{
	int v_id0;
	int v_id1;
	int v_id2;
	int id;
} TRI;

typedef struct _SECTOR
{
	LINE* lines[SECTOR_MAX_LINES];
	int lineamount;
	int id;
} SECTOR;

typedef struct _ENTITY
{
	int x, y;
	int id;
	int eid;
	int flags;
	int angle;
} ENTITY;

// Cyclical data structure
typedef struct _CONNECTED_VERTICES
{
	VERTEX* v;
	VERTEX* other;
} CONNECTED_VERTICES;

extern CONNECTED_VERTICES* map_connectedVertices;
extern SECTOR* map_sectors;
extern LINE*   map_lines;
extern VERTEX* map_vertices;
extern TRI*    map_tris;
extern ENTITY* map_entities;
extern int     map_vertexNextID;
extern int     map_vertexAmount;
extern int     map_lineNextID;
extern int     map_lineAmount;
extern int     map_triNextID;
extern int     map_triAmount;
extern int     map_entityAmount;
extern int     map_entityNextID;

void map_init(void);
// Vertices
void map_vertex_push(VERTEX v);
int  map_vertex_isduplicate(VERTEX v, int* original_id);
void map_vertex_delete(int id);
VERTEX map_vertex_getFromID(int id);
VERTEX* map_vertex_getPointerFromID(int id);
// Lines
void map_line_push(LINE l);
LINE* map_line_getFromID(int id);
void map_line_deleteByVertex(int vertexId);
void map_line_delete(int id);
int  map_line_pointIsOneLine(int x, int y, int line_id);
LINE* map_line_getLineFromPoint(int x, int y);
// Tris
void map_tri_push(TRI t);
void map_tri_deleteByVertex(int vertexId);
void map_tri_deleteByID(int id);
int  map_tri_getTriIDByCoord(int mx, int my);
// Entities
void map_entity_push(ENTITY e);
void map_entity_delete(int id);
int map_entity_existsAtPos(int x, int y, ENTITY* originalEnt);
ENTITY* map_entity_getPointerFromID(int id);
void map_entity_changeAngle(int id, int angle_entity_change);
// Free memory
void map_vertex_deleteAll(void);
void map_line_deleteAll(void);
void map_sector_deleteAll(void);
// Debug
void d_map_printVertices(void);

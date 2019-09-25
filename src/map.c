#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "map.h"

CONNECTED_VERTICES* map_connectedVertices = NULL;
SECTOR* map_sectors                       = NULL;
LINE*   map_lines                         = NULL;
VERTEX* map_vertices                      = NULL;
TRI*    map_tris                          = NULL;
ENTITY* map_entities                      = NULL;
int map_vertexNextID                      = 1;
int map_triNextID                         = 1;
int map_vertexAmount                      = 0;
int map_connectedVerticesSize             = 0;
int     map_lineNextID = 1;
int     map_lineAmount = 0;
int     map_triAmount  = 0;
int     map_entityAmount = 0;
int     map_entityNextID = 1;

void map_entity_push(ENTITY e)
{
	map_entities = realloc(map_entities,    sizeof(ENTITY) * (map_entityAmount + 1));
	memcpy(&map_entities[map_entityAmount], &e, sizeof(ENTITY));
	map_entityAmount++;
	map_entityNextID++;
}

void map_entity_delete(int id)
{
	ENTITY* newBuffer = NULL;
	int i, bufferIndex = 0;
	
	newBuffer = malloc(sizeof(ENTITY) * (map_entityAmount - 1));
	for (i = 0; i < map_entityAmount; i++)
	{
		ENTITY e = map_entities[i];
		if (e.id != id)
		{
			newBuffer[bufferIndex] = e;
			bufferIndex++;
		}
	}
	
	free(map_entities);
	map_entities = newBuffer;
	map_entityAmount -= 1;
}


int map_entity_existsAtPos(int x, int y, ENTITY* originalEnt)
{
	register int i;
	for (i = 0; i < map_entityAmount; i++)
		if (map_entities[i].x == x && map_entities[i].y == y)
		{
			if (originalEnt)
				*originalEnt = map_entities[i];
			return 1;
		}
	return 0;
}

void map_entity_changeAngle(int id, int angle)
{
	ENTITY* e = map_entity_getPointerFromID(id);
	if (e)
		e->angle = angle;
	else
		puts("map_entity_changeAngle ID not found!");
}


/*
function ptInTriangle(p, p0, p1, p2) {
    var A = 1/2 * (-p1.y * p2.x + p0.y * (-p1.x + p2.x) + p0.x * (p1.y - p2.y) + p1.x * p2.y);
    var sign = A < 0 ? -1 : 1;
    var s = (p0.y * p2.x - p0.x * p2.y + (p2.y - p0.y) * p.x + (p0.x - p2.x) * p.y) * sign;
    var t = (p0.x * p1.y - p0.y * p1.x + (p0.y - p1.y) * p.x + (p1.x - p0.x) * p.y) * sign;
    
    return s > 0 && t > 0 && (s + t) < 2 * A * sign;
}
*/
int pointIsInTriangle(float x, float y, float p0x, float p0y, float p1x, float p1y, float p2x, float p2y)
{
	float a = 1.0/2.0 * (-p1y * p2x + p0y * (-p1x + p2x) + p0x * (p1y - p2y) + p1x * p2y);
	float sgn = 1.0;
	if (a < 0.0)
		sgn = -1.0;
	float s = (p0y * p2x - p0x * p2y + (p2y - p0y) * x + (p0x - p2x) * y) * sgn;
	float t = (p0x * p1y - p0y * p1x + (p0y - p1y) * x + (p1x - p0x) * y) * sgn;
	
	return (s > 0.0 && t > 0.0 && (s+t) < 2 * a*sgn);
}

int map_tri_getTriIDByCoord(int mx, int my)
{
	register int i;
	for (i = 0; i < map_triAmount; i++)
	{
		TRI t     = map_tris[i];
		VERTEX v0 = map_vertex_getFromID(t.v_id0);
		VERTEX v1 = map_vertex_getFromID(t.v_id1);
		VERTEX v2 = map_vertex_getFromID(t.v_id2);
		if (pointIsInTriangle(mx, my, v0.x, v0.y, v1.x, v1.y, v2.x, v2.y))
			return t.id;
	}
	return 0;
}

/* This fucking works, don't every bloody touch it!
 * It was 1:57 in the morning.
 * On monday.
 * I'm so angry
 */
double distance_to_line(int x, int y, int line_id)
{
	LINE *l   = map_line_getFromID(line_id);
	VERTEX v0 = map_vertex_getFromID(l->v_id0);
	VERTEX v1 = map_vertex_getFromID(l->v_id1);
	double a = (double)v0.y - (double)v1.y;
	double b = (double)v1.x - (double)v0.x;
	double c = (double)(v0.x * v1.y) - (double)(v1.x * v0.y);
	double dist = abs(a*x + b*y + c)/sqrt(a*a+b*b);
	return dist;
}

void map_tri_deleteByID(int id)
{
	
	TRI* newBuffer = NULL;
	int i, bufferIndex = 0;
	
	newBuffer = malloc(sizeof(TRI) * (map_triAmount - 1));
	for (i = 0; i < map_triAmount; i++)
	{
		TRI t = map_tris[i];
		if (t.id != id)
		{
			puts("PUSH");
			newBuffer[bufferIndex] = t;
			bufferIndex++;
		}
	}
	
	free(map_tris);
	map_tris       = newBuffer;
	map_triAmount -= 1;
}

int  map_line_pointIsOneLine(int x, int y, int line_id)
{
	LINE *l    = map_line_getFromID(line_id);
	VERTEX v0 = map_vertex_getFromID(l->v_id0);
	VERTEX v1 = map_vertex_getFromID(l->v_id1);

	double dist = distance_to_line(x, y, line_id);
	if (dist < 8)
	{
		if ((x < v0.x && x < v1.x) || (x > v0.x && x > v1.x))
		{
			if ((y < v0.y && y < v1.y) || (y > v0.y && y > v1.y))
			{
				return 0;
			}
		}
	}
	return (dist < 8);
}

/* Return the ppointer to the line that approximately has point (x, y) */
LINE* map_line_getLineFromPoint(int x, int y)
{
	LINE* outLine = NULL;
	register int i;
	
	for (i = 0; i < map_lineAmount; i++)
	{
		if (map_line_pointIsOneLine(x, y, map_lines[i].id))
		{
			outLine = &map_lines[i];
			break;
		}
	}
	
	return outLine;
}

void map_init(void)
{
	map_sectors      = NULL;
	map_vertices     = malloc(sizeof(VERTEX) * 64 * 64);
	map_vertexAmount = 0;
}

void map_vertex_push(VERTEX v)
{	
	//map_vertices = realloc(map_vertices,    sizeof(VERTEX) * (map_vertexAmount + 1));
	memcpy(&map_vertices[map_vertexAmount], &v, sizeof(VERTEX));
	map_vertexAmount++;
	map_vertexNextID++;
}

void map_vertex_delete(int id)
{
	register int i;
	
	// Firstly remove all lines and tris connected to this vertex
	map_line_deleteByVertex(id);
	map_tri_deleteByVertex(id);
	
	// For deletion, we move the given ID of vertex to the end of the array.
	for (i = 0; i < map_vertexAmount; i++)
	{
		if (id == map_vertices[i].id)
		{
			while (i < map_vertexAmount)
			{
				VERTEX c          = map_vertices[i+1];
				map_vertices[i+1] = map_vertices[i];
				map_vertices[i]   = c;
				i++;
			}
			map_vertexAmount--;
			break;
		}
	}
}

// Returns non-zero value if the given vertex is a duplicate.
// If original_id is not NULL, and we prove the vertex it is a duplicate,
// it will store the value of the orignal's ID.
int map_vertex_isduplicate(VERTEX v, int* original_id)
{
	register int i;
	for (i = 0; i < map_vertexAmount; i++)
		if (map_vertices[i].x == v.x && map_vertices[i].y == v.y)
		{
			if (original_id)
				*original_id = map_vertices[i].id;
			return 1;
		}
	return 0;
}

// Get the value of wanted vertex in the array of all vertices.
VERTEX map_vertex_getFromID(int id)
{
	VERTEX v;
	register int i;
	
	for (i = 0; i < map_vertexAmount; i++)
		if (map_vertices[i].id == id)
			v = map_vertices[i];
	
	return v;
}

ENTITY* map_entity_getPointerFromID(int id)
{
	register int i;
	
	for (i = 0; i < map_entityAmount; i++)
		if (map_entities[i].id == id)
			return &map_entities[i];
	
	return NULL;
}

VERTEX* map_vertex_getPointerFromID(int id)
{
	register int i;
	
	for (i = 0; i < map_vertexAmount; i++)
		if (map_vertices[i].id == id)
			return &map_vertices[i];
	
	return NULL;
}

void map_line_push(LINE l)
{
	map_lines = realloc(map_lines,    sizeof(LINE) * (map_lineAmount + 1));
	memcpy(&map_lines[map_lineAmount], &l, sizeof(LINE));
	map_lineAmount++;
	map_lineNextID++;
}

void map_line_delete(int id)
{
	LINE* newBuffer = NULL;
	int i, bufferIndex = 0;
	
	// Create the new buffer for lines that don't use the given vertex ID.
	newBuffer = malloc(sizeof(LINE) * (map_lineAmount - 1));
	for (i = 0; i < map_lineAmount; i++)
	{
		LINE l = map_lines[i];
		if (l.id != id)
		{
			newBuffer[bufferIndex] = l;
			bufferIndex++;
		}
	}
	
	free(map_lines);
	map_lines = newBuffer;
	map_lineAmount -= 1;
}

// Deletes line(s) which use a vertex of specific ID.
void map_line_deleteByVertex(int vertexId)
{
	// Find the amount of lines that have this specific vertex ID.
	LINE* newBuffer = NULL;
	int i, bufferIndex = 0;
	int delAmount = 0;
	for (i = 0; i < map_lineAmount; i++)
	{
		LINE l = map_lines[i];
		if (l.v_id0 == vertexId || l.v_id1 == vertexId)
			delAmount++;
	}
	
	// Create the new buffer for lines that don't use the given vertex ID.
	newBuffer = malloc(sizeof(LINE) * (map_lineAmount - delAmount));
	for (i = 0; i < map_lineAmount; i++)
	{
		LINE l = map_lines[i];
		if (l.v_id0 != vertexId && l.v_id1 != vertexId)
		{
			newBuffer[bufferIndex] = l;
			bufferIndex++;
		}
	}
	
	free(map_lines);
	map_lines = newBuffer;
	map_lineAmount -= delAmount;
}

LINE* map_line_getFromID(int id)
{
	LINE* l = NULL;
	register int i;
	
	for (i = 0; i < map_lineAmount; i++)
	{
		if (map_lines[i].id == id)
		{
			l = &map_lines[i];
			break;
		}
	}
	
	return l;
}


void map_tri_push(TRI t)
{
	map_tris = realloc(map_tris,    sizeof(TRI) * (map_triAmount + 1));
	memcpy(&map_tris[map_triAmount], &t, sizeof(TRI));
	map_triAmount++;
	map_triNextID++;
}

void map_tri_deleteByVertex(int vertexId)
{
	TRI* newBuffer = NULL;
	int i, bufferIndex = 0;
	int delAmount = 0;
	for (i = 0; i < map_triAmount; i++)
	{
		TRI t = map_tris[i];
		if (t.v_id0 == vertexId || t.v_id1 == vertexId || t.v_id2 == vertexId)
			delAmount++;
	}
	
	newBuffer = malloc(sizeof(TRI) * (map_triAmount - delAmount));
	for (i = 0; i < map_triAmount; i++)
	{
		TRI t = map_tris[i];
		if (t.v_id0 != vertexId && t.v_id1 != vertexId && t.v_id2 != vertexId)
		{
			newBuffer[bufferIndex] = t;
			bufferIndex++;
		}
	}
	
	free(map_tris);
	map_tris       = newBuffer;
	map_triAmount -= delAmount;
}

void map_vertex_deleteAll(void) { free(map_connectedVertices); free(map_vertices); }
void map_line_deleteAll  (void) { puts("map_line_deleteAll STUB");                 }
void map_sector_deleteAll(void) { free(map_sectors);                               }

void d_map_printVertices(void)
{
	int i;
	puts("Map vertices:");
	for (i = 0; i < map_vertexAmount; i++)
	{
		VERTEX v = map_vertices[i];
		printf("%d: VERTEX {%d %d %d};\n", i, v.id, v.x, v.y);
	}
}

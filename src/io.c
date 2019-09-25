#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "map.h"
#include "io.h"

void io_export_ascii(char* filepath)
{
	FILE* f      = fopen(filepath, "w");
	char* buffer = NULL;
	char buff0[8];
	char buff1[8];
	char buff2[8];
	char buff3[8];
	char buff4[8];
	char buff5[8];
	int i;
	if (!f)
		puts("File is NULL");
	sprintf(buff0, "%d", map_vertexAmount);
	sprintf(buff1, "%d", map_lineAmount);
	sprintf(buff2, "%d", map_triAmount);
	sprintf(buff3, "%d", map_entityAmount);
	// 1. write byte sizes of all structs
	puts("Writing structure type byte lengths...");
	fprintf(f, "%s;%s;%s;%s;\n", buff0, buff1, buff2, buff3);
	// 2. write structs
	puts("Writing data...");
	for (i = 0; i < map_vertexAmount; i++)
	{
		sprintf(buff0, "%d", map_vertices[i].id);
		sprintf(buff1, "%d", map_vertices[i].x);
		sprintf(buff2, "%d", map_vertices[i].y);
		fprintf(f, "%s,%s,%s;", buff0, buff1, buff2);
	}
	
	for (i = 0; i < map_lineAmount; i++)
	{
		sprintf(buff0, "%d", map_lines[i].v_id0);
		sprintf(buff1, "%d", map_lines[i].v_id1);
		sprintf(buff2, "%d", map_lines[i].id);
		fprintf(f, "%s,%s,%s;", buff0, buff1, buff2);
	}
	
	for (i = 0; i < map_triAmount; i++)
	{
		sprintf(buff0, "%d", map_tris[i].v_id0);
		sprintf(buff1, "%d", map_tris[i].v_id1);
		sprintf(buff2, "%d", map_tris[i].v_id2);
		sprintf(buff3, "%d", map_tris[i].id);
		fprintf(f, "%s,%s,%s,%s;", buff0, buff1, buff2, buff3);
	}
	
	for (i = 0; i < map_entityAmount; i++)
	{
		sprintf(buff0, "%d", map_entities[i].x);
		sprintf(buff1, "%d", map_entities[i].y);
		sprintf(buff2, "%d", map_entities[i].id);
		sprintf(buff3, "%d", map_entities[i].eid);
		sprintf(buff4, "%d", map_entities[i].flags);
		sprintf(buff5, "%d", map_entities[i].angle);
		fprintf(f, "%s,%s,%s,%s,%s,%s;", buff0, buff1, buff2, buff3, buff4, buff5);
	}
	/*fwrite(map_vertices, sizeof(VERTEX), map_vertexAmount, f);
	fwrite(map_lines,    sizeof(LINE),   map_lineAmount, f);
	fwrite(map_tris,     sizeof(TRI),    map_triAmount, f);
	fwrite(map_entities, sizeof(ENTITY), map_entityAmount, f);*/
	fclose(f);
	
	free(buffer);
	puts("----------------");
}

void io_write_start(void)
{
	char* map_name      = NULL;
	char  file_out[256] = "maps/\0";
	puts("io_write_start()");
	puts("----------------");
	
	printf("Map name (less than 250 characters)? >");
	map_name = malloc(250*sizeof(char));
	scanf("%s", map_name);
	
	strcpy(&file_out[5], map_name);
	printf("Writing to file \"%s\"\n", file_out);
	
	{
		FILE* f      = fopen(file_out, "wb");
		char* buffer = NULL;
		int sizes[4] =
		{
			sizeof(VERTEX) * map_vertexAmount,
			sizeof(LINE)   * map_lineAmount,
			sizeof(TRI)    * map_triAmount,
			sizeof(ENTITY) * map_entityAmount
		};
		printf("About to write %d bytes to disk...\n", sizes[0]+sizes[1]+sizes[2]+sizes[3]);
		if (!f)
			puts("File is NULL");
		// 1. write byte sizes of all structs
		puts("Writing structure type byte lengths...");
		fwrite(&sizes, sizeof(int), 4, f);
		// 2. write structs
		puts("Writing data...");
		fwrite(map_vertices, sizeof(VERTEX), map_vertexAmount, f);
		fwrite(map_lines,    sizeof(LINE),   map_lineAmount, f);
		fwrite(map_tris,     sizeof(TRI),    map_triAmount, f);
		fwrite(map_entities, sizeof(ENTITY), map_entityAmount, f);
		fclose(f);
		
		free(buffer);
	}
	
	puts("----------------");
	strcat(file_out, ".input");
	io_export_ascii(file_out);
	free(map_name);
}

void io_read_start(char* file)
{
	FILE* f = fopen(file, "rb");
	int len_v, len_l, len_t, len_e;
	int i;
	printf("Reading map file \"%s\"...\n", file);
	
	// Read length
	// The read data is size of each segemnt in bytes...
	fread(&len_v, sizeof(int), 1, f); // Vertices
	fread(&len_l, sizeof(int), 1, f); // Lines
	fread(&len_t, sizeof(int), 1, f); // Tris
	fread(&len_e, sizeof(int), 1, f); // Entities
	len_v = len_v / sizeof(VERTEX);
	len_l = len_l / sizeof(LINE);
	len_t = len_t / sizeof(TRI);
	len_e = len_e / sizeof(ENTITY);
	printf("Vertices length: %d\nLines length: %d\nTris length: %d\nEntities length: %d\n", len_v, len_l, len_t, len_e);
	
	puts("Push vertices.");
	{
		VERTEX* v = malloc(sizeof(VERTEX) * len_v);
		fread(v, sizeof(VERTEX), len_v, f);
		for (i = 0; i < len_v; i++)
			map_vertex_push(v[i]);
		free(v);
	}
	puts("Push lines.");
	{
		LINE* v = malloc(sizeof(LINE) * len_l);
		fread(v, sizeof(LINE), len_l, f);
		for (i = 0; i < len_l; i++)
			map_line_push(v[i]);
		free(v);
	}
	puts("Push triangles.");
	{
		TRI* v = malloc(sizeof(TRI) * len_t);
		fread(v, sizeof(TRI), len_t, f);
		for (i = 0; i < len_t; i++)
			map_tri_push(v[i]);
		free(v);
	}
	puts("Push entities.");
	{
		ENTITY* v = malloc(sizeof(ENTITY) * len_e);
		fread(v, sizeof(ENTITY), len_e, f);
		for (i = 0; i < len_e; i++)
			map_entity_push(v[i]);
		free(v);
	}
	fclose(f);
}

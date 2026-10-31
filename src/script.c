#include <stdio.h>
#include <stdlib.h>




typedef struct
{
float x;
float y;
float z;
} vec;

typedef struct
{
vec vector1;
vec vector2;
vec vector3;
} STL_triangle;

typedef struct
{
char header[80];
uint32_t triangle_count;
STL_triangle triangles;
} STL_model;

STL load_binary_model(char *file_path)
{
FILE *stl_file = fopen(file_path, "rb");
if (stl_file== NULL)
	{
	fprintf(stderr, "Error opening file");
	return NULL;
	}

STL *model = malloc(sizeof(STL_model));
if (model == NULL)
	{
	fprintf(stderr, "Memory allocation failed");
	fclose(stl_file);
	return NULL;
	}
}



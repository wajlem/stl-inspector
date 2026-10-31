#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#pragma pack(push,1)
typedef struct
{
float x;
float y;
float z;
} vector;

typedef struct
{
vector normal;
vector v1;
vector v2;
vector v3;
uint16_t added;
} triangle;
#pragma pack(pop)


typedef struct
{
char header[80];
uint32_t triangle_count;
triangle *triangles;
} STL_model;

STL_model *load_binary_model(char *file_path)
{
FILE *stl_file = fopen(file_path, "rb");
if (stl_file== NULL)
	{
	fprintf(stderr, "Error opening file\n");
	return NULL;
	}

STL_model *model = malloc(sizeof(STL_model));
if (model == NULL)
	{
	fprintf(stderr, "Memory allocation failed\n");
	fclose(stl_file);
	return NULL;
	}


if (fread(model->header, 1, 80, stl_file) != 80)
  {
  fprintf(stderr, "Error reading header\n");
  free(model);
  fclose(stl_file);
  return NULL;
  }

if (fread(&model->triangle_count, sizeof(uint32_t), 1, stl_file) != 1)
  {
  fprintf(stderr, "Error reading triangle count\n");
  free(model);
  fclose(stl_file);
  return NULL;
  }

model->triangles = malloc(model->triangle_count * sizeof(triangle));
if (model->triangles == NULL)
  {
  fprintf(stderr, "Memory allocation failed\n");
  free(model);
  fclose(stl_file);
  return NULL;
  }

if (fread(model->triangles, sizeof(triangle), model->triangle_count, stl_file)
    != model->triangle_count)
  {
  fprintf(stderr, "Error reading triangles\n");
  free(model->triangles);
  free(model);
  fclose(stl_file);
  return NULL;
  }

fclose(stl_file);
return model;
}

double volume_calc(STL_model *model)
{
	double volume = 0.0;

	for (uint32_t i = 0; i < model->triangle_count; i++)
	{
		vector a = model->triangles[i].v1;
		vector b = model->triangles[i].v2;
		vector c = model->triangles[i].v3;

// Normal vector 
  	double new_vector_YZ = (double)b.y * c.z - (double)b.z * c.y;
  	double new_vector_XZ = (double)b.z * c.x - (double)b.x * c.z;
  	double new_vector_XY = (double)b.x * c.y - (double)b.y * c.x;

		volume += (a.x * new_vector_YZ + a.y * new_vector_XZ + a.z * new_vector_XY)/6.0;
	}
	return volume;

}

int main(int argc, char **argv)
{
if (argc != 2)
	{
		fprintf(stderr, "Missing name of file\n");
		fprintf(stdout, "Should look like this\n%s (absolute way to file)\n", argv[0]);
		return -1;
	}
STL_model *model = load_binary_model(argv[1]);
if (model == NULL)
	return -1;
double volume = volume_calc(model);
printf("Volume is %f\n", volume);
printf("There are %u triangles\n", model->triangle_count);
free(model->triangles);
free(model);
return 0;

}




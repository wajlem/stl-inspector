#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

/*
STL format is:
80 bytes (text)
4 bytes (num of triangles "uint32_t")
50 bytes per triangle (normal (12), v1(12), v2(12), v3(12), some_attribute(2))
*/

#pragma pack(push,1)

// point in 3d space
typedef struct
{
	float x;
	float y;
	float z;
} vector;

// ONE triangle 
typedef struct
{
	vector normal;
	vector v1;
	vector v2;
	vector v3;
	uint16_t added; //that random attribute, noneone uses it
} triangle;
#pragma pack(pop)

// whole model in memory
typedef struct
{
	char header[80];
	uint32_t triangle_count; 
	triangle *triangles; //dynamical array of triangles
} STL_model;


//loads whole binary model into memory
STL_model *load_binary_model(char *file_path)
{
//opening file
	FILE *stl_file = fopen(file_path, "rb");
	if (stl_file== NULL)
		{
			fprintf(stderr, "Error opening file\n");
			return NULL;
		}

//allocating memory for model
	STL_model *model = malloc(sizeof(STL_model));
	if (model == NULL)
	{
		fprintf(stderr, "Memory allocation failed\n");
		fclose(stl_file);
		return NULL;
	}

//checking header
	if (fread(model->header, 1, 80, stl_file) != 80)
  {
  	fprintf(stderr, "Error reading header\n");
  	free(model);
  	fclose(stl_file);
  	return NULL;
  }

//checking triangles count
	if (fread(&model->triangle_count, sizeof(uint32_t), 1, stl_file) != 1)
  {
  	fprintf(stderr, "Error reading triangle count\n");
  	free(model);
  	fclose(stl_file);
  	return NULL;
  }

//allocating array for all triangles
	model->triangles = malloc(model->triangle_count * sizeof(triangle));
	if (model->triangles == NULL)
  {
  	fprintf(stderr, "Memory allocation failed\n");
  	free(model);
  	fclose(stl_file);
  	return NULL;
  }

//reading all triangles 
	if (fread(model->triangles, sizeof(triangle), model->triangle_count, stl_file)
    != model->triangle_count) //magic thingy
  {
  fprintf(stderr, "Error reading triangles\n");
  free(model->triangles);
  free(model);
  fclose(stl_file);
  return NULL;
  }

//closes if it succeded
	fclose(stl_file);
	return model;
}

/*
calculate volume of model (NEEDS to be watertight!)
caltulating every triangle by some magic https://en.wikipedia.org/wiki/Tetrahedron
*/

double volume_calc(STL_model *model)
{
//double for precision in high polygon models
	double volume = 0.0;

	for (uint32_t i = 0; i < model->triangle_count; i++)
	{

//current vertice of triangle
		vector a = model->triangles[i].v1;
		vector b = model->triangles[i].v2;
		vector c = model->triangles[i].v3;

// normal vector for point b and c
  	double new_vector_YZ = (double)b.y * c.z - (double)b.z * c.y;
  	double new_vector_XZ = (double)b.z * c.x - (double)b.x * c.z;
  	double new_vector_XY = (double)b.x * c.y - (double)b.y * c.x;

//volume of each tetrahedron
		volume += (a.x * new_vector_YZ + a.y * new_vector_XZ + a.z * new_vector_XY)/6.0;
	}
	return volume;

}

//defining vectors for bounding box function
typedef struct
{
	vector min;
	vector max;
} bounding_box;

bounding_box bounding_box_calc(STL_model *model)
{
//adding box with 2 vector
	bounding_box box = {model->triangles[0].v1, model->triangles[0].v1};
	
//looking for every vector that is smallest and biggest
	for(uint32_t i =0; i < model->triangle_count; i++)
	{
		vector a = model->triangles[i].v1;
		vector b = model->triangles[i].v2;
		vector c = model->triangles[i].v3;
	
		box.min.x = fminf(box.min.x, a.x);
		box.min.x = fminf(box.min.x, b.x);
		box.min.x = fminf(box.min.x, c.x);

		box.min.y = fminf(box.min.y, a.y);
		box.min.y = fminf(box.min.y, b.y);
		box.min.y = fminf(box.min.y, c.y);

		box.min.z = fminf(box.min.z, a.z);
		box.min.z = fminf(box.min.z, b.z);
		box.min.z = fminf(box.min.z, c.z);

	
		box.max.x = fmaxf(box.max.x, a.x);
		box.max.x = fmaxf(box.max.x, b.x);
		box.max.x = fmaxf(box.max.x, c.x);

		box.max.y = fmaxf(box.max.y, a.y);
		box.max.y = fmaxf(box.max.y, b.y);
		box.max.y = fmaxf(box.max.y, c.y);

		box.max.z = fmaxf(box.max.z, a.z);
		box.max.z = fmaxf(box.max.z, b.z);
		box.max.z = fmaxf(box.max.z, c.z);
	}
	return box;
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "Missing name of file\n");
		fprintf(stdout, "Should look like this\n%s (absolute way to file)\n", argv[0]);
		return -1;
	}

//load model + error if it doesnt load
	STL_model *model = load_binary_model(argv[1]);
	if (model == NULL)
		return -1;

//calculating a printing results
	double volume = volume_calc(model);
	printf("Volume is %f\n\n", volume);
	printf("There are %u triangles\n\n", model->triangle_count);

// max - min --> cuz stl can be in negative coordinates
	bounding_box box = bounding_box_calc(model);
	printf("Lenght X: %f\nLenght Y: %f\nLenght Z: %f\n", (fabs)(box.max.x - box.min.x), (fabs)(box.max.y - box.min.y), (fabs)(box.max.z - box.min.z));


//we dont want memory leak
	free(model->triangles);
	free(model);
	return 0;
}




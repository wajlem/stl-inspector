#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#define BASE_DIR "stl-inspector"
#define DIFFERENCE 1e-04

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

double surface_calc(STL_model *model)
{
	double surface_area = 0.0;
	for (uint32_t i = 0; i < model->triangle_count; i++)
	{
		vector a = model->triangles[i].v1;
		vector b = model->triangles[i].v2;
		vector c = model->triangles[i].v3;

//edges of triangles		
		double b_a_x = (double)(b.x - a.x);
		double b_a_y = (double)(b.y - a.y);
		double b_a_z = (double)(b.z - a.z);		

		double c_a_x = (double)(c.x - a.x);
		double c_a_y = (double)(c.y - a.y);
		double c_a_z = (double)(c.z - a.z);		

//normal vectors
		double cross_x = (double)(b_a_y*c_a_z - b_a_z*c_a_y);
		double cross_y = (double)(b_a_z*c_a_x - b_a_x*c_a_z);
		double cross_z = (double)(b_a_x*c_a_y - b_a_y*c_a_x);
		
		surface_area += sqrt(cross_x*cross_x + cross_y*cross_y + cross_z*cross_z)/2;
	}
	return surface_area;
}

//calculating overhang triangles cuz it can affect 3d printing
double overhang_calc(STL_model *model, double limit_angle)
{
	double overhang_area = 0.0;

//from degree to radians
	double limit = -cos(limit_angle * M_PI / 180.0);

	for (uint32_t i = 0; i < model->triangle_count; i++)
	{
		vector a = model->triangles[i].v1;
		vector b = model->triangles[i].v2;
		vector c = model->triangles[i].v3;

//two edges of the triangle
		double b_a_x = (double)b.x - a.x;
		double b_a_y = (double)b.y - a.y;
		double b_a_z = (double)b.z - a.z;

		double c_a_x = (double)c.x - a.x;
		double c_a_y = (double)c.y - a.y;
		double c_a_z = (double)c.z - a.z;

//normal vector 
		double cross_x = b_a_y * c_a_z - b_a_z * c_a_y;
		double cross_y = b_a_z * c_a_x - b_a_x * c_a_z;
		double cross_z = b_a_x * c_a_y - b_a_y * c_a_x;

		double len = sqrt(cross_x * cross_x + cross_y * cross_y + cross_z * cross_z);

//triangle with zero area, skip it (divide by 0)
		if (len == 0.0)
			continue;

//unit normal z component = cross_z / len
		if (cross_z / len < limit)
			overhang_area += len / 2.0;
	}
	return overhang_area;
}

//checking how many triangles share same corner
int shared_corner(triangle *tri1, triangle *tri2)
{
	vector *a = &tri1->v1;
	vector *b = &tri2->v1;
	int shared = 0;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (memcmp(&a[i], &b[j], sizeof(vector)) == 0)
				shared++;
		}
	}
	return shared;
}

//checking if every triangle shares corners with other triangles
bool is_watertight(STL_model *model)
{
	for (uint32_t i = 0; i < model->triangle_count; i++)
	{
		int neighbours = 0;
	
		for(uint32_t j = 0; j < model->triangle_count; j++)
		{
			if (shared_corner(&model->triangles[i], &model->triangles[j]) == 2)
				neighbours++;
		}
		if (neighbours != 3)
			return false;
	}
	return true;
}

//prints whole report
void print_report(FILE *out, STL_model *model, bool water, bool header)
{
	if(header)
		fprintf(out, "Header: %.80s\n\n", model->header);

	double volume = volume_calc(model);
	fprintf(out, "Volume is %f\n\n", volume);
	fprintf(out, "There are %u triangles\n\n", model->triangle_count);

	bounding_box box = bounding_box_calc(model);
	fprintf(out, "Lenght X: %f\nLenght Y: %f\nLenght Z: %f\n\n", (fabs)(box.max.x - box.min.x), (fabs)(box.max.y - box.min.y), (fabs)(box.max.z - box.min.z));

	double surface_area = surface_calc(model);
	fprintf(out, "Surface ares is %f\n", surface_area);

	double overhang = overhang_calc(model, 45.0);
	fprintf(out, "Overhang area (45 deg): %f (%.1f%% of surface)\n\n", overhang, 100.0 * overhang / surface_area);

	if (volume < 0)
		fprintf(out, "Negative value! Triangles are flipped inside out\n\n");

	double layer_height = 0.2;
	double height = box.max.z - box.min.z;
	fprintf(out, "Layers: %i (cut is %fmm)\n\n", (int)(height / layer_height), layer_height);

	fprintf(out, "Mesh density: %f triangles per unit*unit\n\n", (model->triangle_count / surface_area));

	double density = 7.85;
	double weight = (fabs(volume) / 1000.0) * density;
	fprintf(out, "Weight is %f g, Material: Steel\n\n", weight);

	if(water)
	{
		if (is_watertight(model))
			fprintf(out, "Model is watertight\n\n");
		else
			fprintf(out, "Model is not watertight, volume can be dishonest\n\n");
	}
}

//saves the full report to ~/BASE_DIR/output/output-modelName-date_time.txt
//AI did debug this part cuz I tried very hard and wanted to go get some food
int export_report(STL_model *model, char *model_path, bool water, bool header)
{
	char *home = getenv("HOME");
	if (home == NULL)
	{
		fprintf(stderr, "HOME is not set\n");
		return -1;
	}

//model name = file name without folders and without .stl
	char *name = strrchr(model_path, '/');
	if (name == NULL)
		name = model_path;
	else
		name++;

	char model_name[200];
	snprintf(model_name, sizeof(model_name), "%s", name);

	char *dot = strrchr(model_name, '.');
	if (dot != NULL)
		*dot = '\0';

//date and time, e.g. 2026-11-01_12-30-05
	time_t now = time(NULL);
	char stamp[32];
	strftime(stamp, sizeof(stamp), "%Y-%m-%d_%H-%M-%S", localtime(&now));

//create folders if they dont exist (error is ignored, fopen will tell if it failed)
	char folder[512];
	snprintf(folder, sizeof(folder), "%s/%s", home, BASE_DIR);
	mkdir(folder, 0755);
	snprintf(folder, sizeof(folder), "%s/%s/output", home, BASE_DIR);
	mkdir(folder, 0755);

//full path of the output file
	char path[1024];
	snprintf(path, sizeof(path), "%s/output-%s-%s.txt", folder, model_name, stamp);	
	snprintf(path, sizeof(path), "%s/output-%s-%s.txt", folder, model_name, stamp);

	FILE *out = fopen(path, "w");
	if (out == NULL)
	{
		fprintf(stderr, "Cannot create file %s\n", path);
		return -1;
	}

	print_report(out, model, water, header);
	fclose(out);

	printf("Report saved to %s\n", path);
	return 0;
}

void print_help(char *program_name)
{
	printf("STL Inspector - Fast CLI tool for binary STL file analysis\n\n");
	printf("Usage:\n");
	printf("  %s <file.stl> [options]\n\n", program_name);
	printf("Options:\n");
	printf("  -water       Check if the mesh is watertight (closed, no holes)\n");
	printf("               Note: slower on high-polygon models\n");
	printf("  -header      Display raw 80-byte STL header\n");
	printf("  -export      Save detailed report to ~/stl-inspector/output/\n");
	printf("  -h, --help   Display this help message\n\n");
	printf("Example:\n");
	printf("  %s model.stl -water -export\n", program_name);


}

bool file_validation(char *file_path)
{
	FILE *f = fopen(file_path, "rb");
//cannot be opened
	if(f == NULL)
	{
		fprintf(stderr, "Error: file %s cannot be opened\n");
		return false;
	}
	
	//we need atleast 84 bytes;
	fseek(f, 0, SEEK_END);
	long file_size = ftell(f);
	rewind(f);
	if (file_size < 84)
	{
		fprintf(stderr, "Error: file is too small\n");
		fclose(f);
		return false;
	}

	char header[81];
	uint32_t count = 0;
	
//if header is lower than 80char
	if (fread(header,1, 80, f) != 80)
	{
		fprintf(stderr, "Error while reading header\n");
		fclose(f);
		return false;
	}

//if there are less then two triangle
	if (fread(&count, sizeof(uint32_t), 1, f) != 1)
	{
		fprintf(stderr, "Error while reading triangles\n");
		return false;
	}

	fclose(f);

	if (count == 0)
	{
		fprintf(stderr, "Error: zero triangles");
		return false;
	}
	
	return true;
}

int main(int argc, char **argv)
{

	if (argc < 2)
	{
		print_help(argv[0]);
		return -1;
	}

	char *file_path = argv[1];
	bool water = false;
	bool header = false;
	bool export = false;
	bool help = false;



	for (int i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "-water") == 0)
			water = true;
		if (strcmp(argv[i], "-header") == 0)
			header = true;
		if (strcmp(argv[i], "-export") == 0)
			export = true;
		if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
			help = true;
	}

	if (help)
	{
		print_help(argv[0]);
		return 0;
	}

	if (!file_validation(file_path))
		return -1;


//load model + error if it doesnt load
	STL_model *model = load_binary_model(file_path);
	if (model == NULL)
		return -1;

	if (!export)
		print_report(stdout, model, water, header);


//export is always the full report
	if (export)
	{
		if (export_report(model, file_path, water, header) != 0)
		{
			free(model->triangles);
			free(model);
			return -1;
		}
	}


//we dont want memory leak
	free(model->triangles);
	free(model);
	return 0;
}




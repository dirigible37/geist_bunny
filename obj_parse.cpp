#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>

typedef struct vector3 {
	float x, y, z;
} vec3;

typedef struct vector3Int {
	int x, y, z;
} vec3Int;

typedef struct face {
	vec3Int vi, ni;
} face;

int main (int argc, char **argv) {
	std::vector<vec3> vertices;
	std::vector<vec3> normals;
	std::vector<face> faces;

	FILE * fp = fopen("bunny.obj", "r");
	if(fp == NULL) {
		printf("Error reading file\n");
		return 0;
	}
	
	char *dataType = (char *)malloc(sizeof(char) * 2);
	
	while(1) {
		int line = fscanf(fp, "%s", dataType);
		
		if(line == EOF) {
			printf("End of File!\n");
			break;
		}
		vec3 vertex;
		face tmpFace;
		if(strcmp("#", dataType) == 0) {
			char trash[128];
			fgets(trash, 128, fp);
			continue;
		}
		if(strcmp("v", dataType) == 0) {
			fscanf(fp, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			//printf("v %f %f %f\n", vertex.x, vertex.y, vertex.z);
			vertices.push_back(vertex);
		}
		else if(strcmp("vn", dataType) == 0) {
			fscanf(fp, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			//printf("vn %f %f %f\n", vertex.x, vertex.y, vertex.z);
			normals.push_back(vertex);
		}
		else if(strcmp("f", dataType) == 0) {
			vec3Int vi, ti, ni;
			fscanf(fp, "%d//%d %d//%d %d//%d\n", &vi.x, &ni.x, &vi.y, &ni.y, &vi.z, &ni.z);
			//printf("f %d//%d %d//%d %d//%d\n", vi.x, ni.x, vi.y, ni.y, vi.z, ni.z);
			tmpFace.vi = vi;
			tmpFace.ni = ni;
			faces.push_back(tmpFace);
		}
		else {
			printf("Unrecognized data type: %s\n", dataType);
			break;
		}
	}

	return 0;
}

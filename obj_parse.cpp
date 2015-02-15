/*#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
*/
typedef struct vector3 {
	GLfloat x, y, z;
} vec3;

typedef struct vector3Int {
	int x, y, z;
} vec3Int;

typedef struct face {
	vec3Int vi, ni;
} face;

std::vector<vec3> vertices;
std::vector<vec3> final_vertices;
std::vector<vec3> normals;
std::vector<vec3> final_normals;
std::vector<vec3> vbo;
std::vector<face> faces;

GLfloat * out_data;

int out_data_size = 0;

int read_obj () {

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
			vertices.push_back(vertex);
		}
		else if(strcmp("vn", dataType) == 0) {
			fscanf(fp, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			normals.push_back(vertex);
		}
		else if(strcmp("f", dataType) == 0) {
			vec3Int vi, ti, ni;
			fscanf(fp, "%d//%d %d//%d %d//%d\n", &vi.x, &ni.x, &vi.y, &ni.y, &vi.z, &ni.z);
			tmpFace.vi = vi;
			tmpFace.ni = ni;
			faces.push_back(tmpFace);
		}
		else {
			printf("Unrecognized data type: %s\n", dataType);
			break;
		}
	}
	int i, j;
	for(i = 0; i < faces.size(); i++) {
		final_vertices.push_back(vertices[faces[i].vi.x-1]);
		final_vertices.push_back(vertices[faces[i].vi.y-1]);
		final_vertices.push_back(vertices[faces[i].vi.z-1]);
		
		final_normals.push_back(normals[faces[i].ni.x-1]);
		final_normals.push_back(normals[faces[i].ni.y-1]);
		final_normals.push_back(normals[faces[i].ni.z-1]);
	}
	
	out_data_size = faces.size() * 2 * 3 * 3;

	out_data = (GLfloat *)malloc(sizeof(GLfloat) * out_data_size);
	j = 0;
	for(i = 0; i < faces.size()*3; i++) {
		out_data[j] = final_vertices[i].x;	
		j++;
		out_data[j] = final_vertices[i].y;	
		j++;
		out_data[j] = final_vertices[i].z;	
		j++;
	}
	for(i = 0; i < faces.size()*3; i++) {
		out_data[j] = final_normals[i].x;	
		j++;
		out_data[j] = final_normals[i].y;	
		j++;
		out_data[j] = final_normals[i].z;	
		j++;
	}
	
	return 0;
}

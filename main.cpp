#include <stdio.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include "obj_parse.cpp"

typedef struct point {
	float x, y, z;
} point;

void setup_viewvolume() {
	point eye, view, up;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1.0, 0.1, 20.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	eye.x = 2.0; 
	eye.y = 2.0;
	eye.z = 2.0;
	view.x = 0.0; 
	view.y = 0.0;
	view.z = 0.0;
	up.x = 0.0; 
	up.y = 1.0;
	up.z = 0.0;

	gluLookAt(eye.x, eye.y, eye.z, view.x, view.y, view.z, up.x, up.y, up.z);
}

void draw_stuff() {
	glClearColor(0.35, 0.35, 0.35, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int mybuf = 1;

void initOGL(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE);
	glutInitWindowSize(512,512);
	glutInitWindowPosition(100,50);
	glutCreateWindow("bunny!");
	setup_viewvolume();
	//do_lights();
	//do_material();
	glBindBuffer(GL_ARRAY_BUFFER, mybuf);
	glBufferData(GL_ARRAY_BUFFER, vertices.size(), vertices, GL_STATIC_DRAW);

	glVertexPointer(3, GL_FLOAT, 3*sizeof(GLfloat), NULL+0);
	glNormalPointer(GL_FLOAT, 3*sizeof(GLfloat), NU
}

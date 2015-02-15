#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <unistd.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include "obj_parse.cpp"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

struct point {
	float x;
	float y;
	float z;
};

void setup_the_viewvol()
{
	struct point eye;
	struct point view;
	struct point up;

	glEnable(GL_DEPTH_TEST);

	/* specify size and shape of view volume */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0,1.0,0.1,20.0);

	/* specify position for view volume */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	eye.x = -3.0; eye.y = 3.0; eye.z = 6.0;
	view.x = -0.5; view.y = 1.0; view.z = 0.0;
	up.x = 0.0; up.y = 1.0; up.z = 0.0;

	gluLookAt(eye.x,eye.y,eye.z,view.x,view.y,view.z,up.x,up.y,up.z);
}

void draw_stuff()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES,0,final_vertices.size());
	glutSwapBuffers();
}

void do_lights()
{
	/* white light */
	float light0_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
	float light0_diffuse[] = { 1.0, 1.0, 1.0, 0.0 };
	float light0_specular[] = { 1.0, 1.0, 1.0, 0.0 };
	float light0_position[] = { 1.5, 2.0, 2.0, 1.0 };
	float light0_direction[] = { -1.5, -2.0, -2.0, 1.0};

	/* turn off scene default ambient */
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,light0_ambient);

	/* make specular correct for spots */
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,1);

	glLightfv(GL_LIGHT0,GL_AMBIENT,light0_ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light0_diffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,light0_specular);
	glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,1.0);
	glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,180.0);
	glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION,0.5);
	glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION,0.1);
	glLightf(GL_LIGHT0,GL_QUADRATIC_ATTENUATION,0.01);
	glLightfv(GL_LIGHT0,GL_POSITION,light0_position);
	glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,light0_direction);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void do_material()
{
	float mat_ambient[] = {0.0,0.0,0.0,1.0};
	float mat_diffuse[] = {0.9,0.9,0.1,1.0};
	float mat_specular[] = {1.0,1.0,1.0,1.0};
	float mat_shininess[] = {2.0};

	glMaterialfv(GL_FRONT,GL_AMBIENT,mat_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);
}

const GLuint mybuf = 1;

void initOGL(int argc, char **argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE);
	glutInitWindowSize(512,512);
	glutInitWindowPosition(100,50);
	glutCreateWindow("my_cool_cube");
	glewInit();
	setup_the_viewvol();
	do_lights();
	do_material();
	glBindBuffer(GL_ARRAY_BUFFER,mybuf);
	glBufferData(GL_ARRAY_BUFFER, out_data_size * sizeof(GLfloat), out_data, GL_STATIC_DRAW);
	// When using VBOs, the final arg is a byte offset in buffer, not the address,
	// but gl<whatever>Pointer still expects an address type, hence the NULL.
	glVertexPointer(3,GL_FLOAT,3*sizeof(GLfloat),BUFFER_OFFSET(0));
	glNormalPointer(GL_FLOAT,3*sizeof(GLfloat),BUFFER_OFFSET(out_data_size/2 * sizeof(GLfloat)));
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	/* gray background */
	glClearColor(0.35,0.35,0.35,0.0);
}

void getout(unsigned char key, int x, int y)
{
	switch(key) {
		case 'q':               
			glDeleteBuffers(1,&mybuf);
			exit(1);
		default:
			break;
	}
}

int main(int argc, char **argv)
{
	read_obj();
	initOGL(argc,argv);
	glutDisplayFunc(draw_stuff);
	glutKeyboardFunc(getout);
	glutMainLoop();
	return 0;
}

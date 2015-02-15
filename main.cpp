#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include "obj_parse.cpp"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#define VPASSES 10
#define JITTER 0.01

double genrand()
{
	return(((double)(random()+1))/2147483649.);
}

struct point {
	float x;
	float y;
	float z;
};

void viewvolume_shape()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0,1.0,0.1,20.0);
}

void setup_the_viewvol()
{
	struct point eye;
	struct point view;
	struct point up;

	// specify position for view volume
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	eye.x = -3.0; eye.y = 3.0; eye.z = 6.0;
	view.x = -0.5; view.y = 1.0; view.z = 0.0;
	up.x = 0.0; up.y = 1.0; up.z = 0.0;

	gluLookAt(eye.x,eye.y,eye.z,view.x,view.y,view.z,up.x,up.y,up.z);
}

struct point cross(struct point u, struct point v)
{
	struct point w;
	w.x = u.y*v.z - u.z*v.y;
	w.y = -(u.x*v.z - u.z*v.x);
	w.z = u.x*v.y - u.y*v.x;
	return(w);
}

struct point unit_length(struct point u)
{
	double length;
	struct point v;
	length = sqrt(u.x*u.x+u.y*u.y+u.z*u.z);
	v.x = u.x/length;
	v.y = u.y/length;
	v.z = u.z/length;
	return(v);
}

void jitter_view()
{
	struct point eye, view, up, vdir, utemp, vtemp;

	eye.x = -3.0; eye.y = 3.0; eye.z = 6.0;
	view.x = JITTER*genrand() - 0.5; view.y = JITTER*genrand() + 1.0; view.z = JITTER*genrand();
	up.x = 0.0; up.y = 1.0; up.z = 0.0;
	vdir.x = view.x - eye.x; vdir.y = view.y - eye.y; vdir.z = view.z - eye.z;
	// Calculate correct up vector as orthogonal to vdir and in the plane of
	// vdir and (0,1,0).
	vtemp = cross(vdir,up);
	utemp = cross(vtemp,vdir);
	up = unit_length(utemp);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.x,eye.y,eye.z,view.x,view.y,view.z,up.x,up.y,up.z);
}

void draw_stuff()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES,0,final_vertices.size());
	//glutSwapBuffers();
	glFlush();
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
	glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_ACCUM);
	glutInitWindowSize(512,512);
	glutInitWindowPosition(100,50);
	glutCreateWindow("my_cool_cube");
	glewInit();
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
	glClearAccum(0.0,0.0,0.0,0.0);
}

void go()
{
	int view_pass;
	glClear(GL_ACCUM_BUFFER_BIT);
	for(view_pass=0;view_pass<VPASSES;view_pass++){
		jitter_view();
		draw_stuff();
		glFlush();
		glAccum(GL_ACCUM,1.0/(float)(VPASSES));
	}
	glAccum(GL_RETURN,1.0);
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
	glEnable(GL_DEPTH_TEST);
	viewvolume_shape();
	//setup_the_viewvol();
	jitter_view();
	do_lights();
	do_material();
	//glutDisplayFunc(draw_stuff);
	glutDisplayFunc(go);
	glutKeyboardFunc(getout);
	glutMainLoop();
	return 0;
}

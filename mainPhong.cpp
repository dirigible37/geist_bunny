#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glx.h>
#include <GL/glext.h>
#include "obj_parse.cpp"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#define VPASSES 50
#define JITTER 0.01
#define ADDEACH 0.02
#define FADE 0.98

float eye[] = {-3.0,3.0,6.0};
float viewpt[] = {-0.5,1.0,0.0};
float up[] = {0.0,1.0,0.0};

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
	glFlush();
}


void do_material()
{
	float mat_ambient[] = {0.0,0.0,0.0,1.0};
	float mat_diffuse[] = {0.29,0.21,0.11,1.0};
	float mat_specular[] = {0.3,0.3,0.3,1.0};
	float mat_shininess[] = {30.0};

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
	glutCreateWindow("my_cool_bunny");
	glewInit();
	glBindBuffer(GL_ARRAY_BUFFER,mybuf);
	glBufferData(GL_ARRAY_BUFFER, out_data_size * sizeof(GLfloat), out_data, GL_STATIC_DRAW);
	glVertexPointer(3,GL_FLOAT,3*sizeof(GLfloat),BUFFER_OFFSET(0));
	glNormalPointer(GL_FLOAT,3*sizeof(GLfloat),BUFFER_OFFSET(out_data_size/2 * sizeof(GLfloat)));
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glClearColor(0.54,0.90,1.0,0.0);
	glClearAccum(0.0,0.0,0.0,0.0);
}

void motion_blur()
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

	glTranslatef(0.0, 1.0, 0.0);
	draw_stuff();
	glFlush();
	glAccum(GL_ACCUM, ADDEACH);
	for(view_pass = 0; view_pass < VPASSES; view_pass++){
		glAccum(GL_MULT, FADE);
		glTranslatef(0.0, -1.0 / VPASSES, 0.0);
		draw_stuff();
		glFlush();
		glAccum(GL_ACCUM, ADDEACH);
	}
	glAccum(GL_RETURN, 1.0);
	glFlush();
}


void anti_alias()
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

char *read_shader_program(char *filename)
{
	FILE *fp;
	char *content = NULL;
	int fd, count;
	fd = open(filename,O_RDONLY);
	count = lseek(fd,0,SEEK_END);
	close(fd);
	content = (char *)calloc(1,(count+1));
	fp = fopen(filename,"r");
	count = fread(content,sizeof(char),count,fp);
	content[count] = '\0';
	fclose(fp);
	return content;
}

void set_light()
{
	float key_light_position[] = {-1.0,7.0,6.0,1.0};	
	float back_light_position[] = {0.5,5.0,-4.0,1.0};	
	float fill_light_position[] = {-5.0,-1.0,1.0,1.0}; 

	float default_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
	float default_diffuse[] = { 1.0, 1.0, 1.0, 0.0 };
	float default_specular[] = { 1.0, 1.0, 1.0, 0.0 };

	float dim_light_diffuse[] = { 0.3, 0.3, 0.3, 0.0 };

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,default_ambient);

	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,1);

	glLightfv(GL_LIGHT0,GL_AMBIENT,default_ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,default_diffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,default_specular);

	glLightfv(GL_LIGHT1,GL_AMBIENT,default_ambient);
	glLightfv(GL_LIGHT1,GL_DIFFUSE,dim_light_diffuse);
	glLightfv(GL_LIGHT1,GL_SPECULAR,default_specular);

	glLightfv(GL_LIGHT2,GL_AMBIENT,default_ambient);
	glLightfv(GL_LIGHT2,GL_DIFFUSE,dim_light_diffuse);
	glLightfv(GL_LIGHT2,GL_SPECULAR,default_specular);
	
	glLightfv(GL_LIGHT0,GL_POSITION,key_light_position);
	glLightfv(GL_LIGHT1,GL_POSITION,back_light_position);
	glLightfv(GL_LIGHT2,GL_POSITION,fill_light_position);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
}

void view_volume()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0,1.0,1.0,20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[0],eye[1],eye[2],viewpt[0],viewpt[1],viewpt[2],up[0],up[1],up[2]);
}

unsigned int set_shaders()
{
	GLint vertCompiled, fragCompiled;
	char *vs, *fs;
	GLuint v, f, p;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
	vs = read_shader_program((char*)"main.vert");
	fs = read_shader_program((char*)"main.frag");
	glShaderSource(v,1,(const char **)&vs,NULL);
	glShaderSource(f,1,(const char **)&fs,NULL);
	free(vs);
	free(fs);
	glCompileShader(v);
	glCompileShader(f);
	p = glCreateProgram();
	glAttachShader(p,f);
	glAttachShader(p,v);
	glLinkProgram(p);
	glUseProgram(p);
	return(p);
}

void handleKeys(unsigned char key, int x, int y)
{
	switch(key) {
		case 'q':               
			glDeleteBuffers(1,&mybuf);
			exit(1);
		case 'b':
			motion_blur();
			break;
		case 'a':
			anti_alias();
			break;
		default:
			break;
	}
}

int main(int argc, char **argv)
{
	read_obj();
	initOGL(argc,argv);
	glEnable(GL_DEPTH_TEST);
	view_volume();
	jitter_view();
	set_light();
	do_material();
	set_shaders();
	glutDisplayFunc(anti_alias);
	glutKeyboardFunc(handleKeys);
	glutMainLoop();
	return 0;
}

/************************************************
 *
 *             Example One
 *
 *  A very basic OpenGL program that draws a
 *  triangle on the screen.  This program illustrates
 *  the basic code required for an OpenGL program.
 *
 ************************************************/

#include <Windows.h>
#include <gl/glew.h>
#include <gl/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shaders.h"
#include <stdio.h>
#include "readply.h"
#include <iostream>	

GLuint program;			// shader programs
GLuint triangleVAO;		// the data to be displayed
float angle = 0.0;
int window;

glm::mat4 projection;	// projection matrix

float eyex = 0.2;
float eyey = 0.1;
float eyez = 0.6;
float theta = 1.5;
float phi = 1.5;
float r = 10.0;
int bad_variable;
int nv;
int nn;
unsigned int ni;

GLfloat* calculateNormal(GLfloat *coord1, GLfloat *coord2, GLfloat *coord3);
GLushort* calculateNormal(GLushort *coord1, GLushort *coord2, GLushort *coord3);

/*
 *  The init procedure creates the OpenGL data structures
 *  that contain the triangle geometry, compiles our
 *  shader program and links the shader programs to
 *  the data.
 */

void init() {
	GLuint vbuffer;
	GLuint ibuffer;
	GLint vPosition;
	GLint vNormal;
	int vs;
	int fs;
	char file[]{ "bunny.ply" };

	glGenVertexArrays(1, &triangleVAO);
	glBindVertexArray(triangleVAO);

	ply_model *model = readply(file);

	GLfloat *vertices = new GLfloat[3 * model->nvertex];
	GLfloat(*normal_table)[4] = new GLfloat[model->nvertex][4];
	GLfloat *polygons = new GLfloat[3 * model->nface];
	GLushort *indexes = new GLushort[3 * model->nface];


	for (int i = 0; i < model->nface; i++) {
		indexes[i * 3] = model->faces[i].vertices[0];
		indexes[i * 3 + 1] = model->faces[i].vertices[1];
		indexes[i * 3 + 2] = model->faces[i].vertices[2];
	}

	//for (int i = 0; i < model->nface; i++) {
	//	polygons[i * 4] = indexes[i * 3];
	//	polygons[i * 4 + 1] = indexes[i * 3 + 1];
	//	polygons[i * 4 + 2] = indexes[i * 3 + 2];
	//	polygons[i * 4 + 3] = model->faces[i].n;
	//}

	for (int i = 0; i < model->nvertex; i++) {
		polygons[i * 4] = polygons[i * 4] /model->nface;
		polygons[i * 4 + 1] = polygons[i * 4 + 1] / model->nface;
		polygons[i * 4 + 2] = polygons[i * 4 + 2] / model->nface;
	}

	for (int i = 0; i < model->nvertex; i++) {
		vertices[i * 3] = model->vertices[i].x;
		vertices[i * 3 + 1] = model->vertices[i].y;
		vertices[i * 3 + 2] = model->vertices[i].z;
		//GLushort *temp = calculateNormal(&indexes[i * 3], &indexes[i * 3 + 1], &indexes[i * 3 + 2]);
		GLfloat *temp = calculateNormal(&polygons[i * 3], &polygons[i * 3 + 1], &polygons[i * 3 + 2]);
		/*normals_table[i * 3] = temp[0];
		normals_table[i * 3 + 1] = temp[1];
		normals_table[i * 3 + 2] = temp[2];*/
	}

	nn = model->nvertex * 3;
	nv = model->nvertex * 3;
	ni = model->nface * 3;

	/*
	 *  load the vertex coordinate data
	 */
	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, (nv+nn)*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nv*sizeof(GLfloat), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, nv*sizeof(GLfloat), nn*sizeof(GLfloat), normal_table);

	/*
	 *  load the vertex indexes
	 */
	glGenBuffers(1, &ibuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ni*sizeof(GLushort), indexes, GL_STATIC_DRAW);

	/*
	 *  compile and build the shader program
	 */
	vs = buildShader(GL_VERTEX_SHADER, "example4.vs");
	fs = buildShader(GL_FRAGMENT_SHADER, "example4.fs");
	program = buildProgram(vs, fs, 0);

	/*
	 *  link the vertex coordinates to the vPosition
	 *  variable in the vertex program
	 */
	glUseProgram(program);
	vPosition = glGetAttribLocation(program, "vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);
	vNormal = glGetAttribLocation(program, "vNormal");
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vNormal);

}

/*
 *  finding the normals
 *
 *
 */

GLfloat* calculateNormal(GLfloat *coord1, GLfloat *coord2, GLfloat *coord3)
{
	/* calculate Vector1 and Vector2 */
	GLfloat va[3], vb[3], vr[3], val;
	va[0] = coord1[0] - coord2[0];
	va[1] = coord1[1] - coord2[1];
	va[2] = coord1[2] - coord2[2];

	vb[0] = coord1[0] - coord3[0];
	vb[1] = coord1[1] - coord3[1];
	vb[2] = coord1[2] - coord3[2];

	/* cross product */
	vr[0] = va[1] * vb[2] - vb[1] * va[2];
	vr[1] = vb[0] * va[2] - va[0] * vb[2];
	vr[2] = va[0] * vb[1] - vb[0] * va[1];

	val = sqrt(vr[0] * vr[0] + vr[1] * vr[1] + vr[2] * vr[2]);

	float norm[3];
	norm[0] = vr[0] / val;
	norm[1] = vr[1] / val;
	norm[2] = vr[2] / val;


	return norm;

}

GLushort* calculateNormal(GLushort *coord1, GLushort *coord2, GLushort *coord3)
{
	/* calculate Vector1 and Vector2 */
	GLushort va[3], vb[3], vr[3], val;
	va[0] = coord1[0] - coord2[0];
	va[1] = coord1[1] - coord2[1];
	va[2] = coord1[2] - coord2[2];

	vb[0] = coord1[0] - coord3[0];
	vb[1] = coord1[1] - coord3[1];
	vb[2] = coord1[2] - coord3[2];

	/* cross product */
	vr[0] = va[1] * vb[2] - vb[1] * va[2];
	vr[1] = vb[0] * va[2] - va[0] * vb[2];
	vr[2] = va[0] * vb[1] - vb[0] * va[1];

	return vr;
}


/*
 *  Executed each time the window is resized,
 *  usually once at the start of the program.
 */
void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).

	if (h == 0)
		h = 1;

	float ratio = 1.0 * w / h;

	glViewport(0, 0, w, h);

	projection = glm::perspective(45.0f, ratio, 0.5f, 1000.0f);

}

/*
 *  This procedure is called each time the screen needs
 *  to be redisplayed
 */
void displayFunc() {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 modelViewPerspective;
	int modelLoc;
	int normalLoc;

	model = glm::rotate(glm::mat4(1.0), angle, glm::vec3(0.0, 1.0, 0.0));

	view = glm::lookAt(glm::vec3(eyex, eyey, eyez),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat3 normal = glm::transpose(glm::inverse(glm::mat3(view*model)));

	modelViewPerspective = projection * view * model;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);
	modelLoc = glGetUniformLocation(program, "model");
	glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(modelViewPerspective));
	normalLoc = glGetUniformLocation(program, "normalMat");
	glUniformMatrix3fv(normalLoc, 1, 0, glm::value_ptr(normal));

	glBindVertexArray(triangleVAO);
	glDrawElements(GL_TRIANGLES, ni, GL_UNSIGNED_SHORT, NULL);

	glutSwapBuffers();

}


/*
 *  Update the value of angle on each update
 */
void idleFunc() {

	glutSetWindow(window);
	angle = angle + 1.0;
	glutPostRedisplay();

}

/*
 *  Called each time a key is pressed on
 *  the keyboard.
 */
void keyboardFunc(unsigned char key, int phi, int theta) {
	switch (key) {
	case 'a':
		phi -= 0.001;
		break;
	case 'd':
		phi += 0.001;
		break;
	case 'w':
		theta += 0.001;
		break;
	case 's':
		theta -= 0.001;
		break;
	}
	eyex = r*sin(theta)*cos(phi);
	eyey = r*sin(theta)*sin(phi);
	eyez = r*cos(theta);
	glutPostRedisplay();
}

int main(int argc, char **argv) {

	/*
	 *  initialize glut, set some parameters for
	 *  the application and create the window
	 */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(320, 320);
	window = glutCreateWindow("Assignment One");

	/*
	 *  initialize glew
	 */
	GLenum error = glewInit();
	if (error != GLEW_OK) {
		printf("Error starting GLEW: %s\n", glewGetErrorString(error));
		exit(0);
	}

	glutDisplayFunc(displayFunc);
	glutIdleFunc(idleFunc);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keyboardFunc);



	init();
	glEnable(GL_DEPTH_TEST);

	glClearColor(.7, .7, .7, 1.0);

	glutMainLoop();

}
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

GLuint program;			// shader programs
GLuint triangleVAO;		// the data to be displayed
float angle = 0.0;
int window;

glm::mat4 projection;	// projection matrix

float eyex = 0.0;
float eyey = 0.0;
float eyez = 100.0;
float theta = 1.5;
float phi = 1.5;
float r = 10.0;


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

	glGenVertexArrays(1, &triangleVAO);
	glBindVertexArray(triangleVAO);

	GLfloat vertices[8][4] = {
		{ -1.0, -1.0, -1.0, 1.0 }, //0
		{ -1.0, -1.0, 1.0, 1.0 }, //1
		{ -1.0, 1.0, -1.0, 1.0 }, //2
		{ -1.0, 1.0, 1.0, 1.0 }, //3
		{ 1.0, -1.0, -1.0, 1.0 }, //4
		{ 1.0, -1.0, 1.0, 1.0 }, //5
		{ 1.0, 1.0, -1.0, 1.0 }, //6
		{ 1.0, 1.0, 1.0, 1.0 } //7
	};
	GLfloat normals[8][3] = {
		{ -1.0, -1.0, -1.0 }, //0
		{ -1.0, -1.0, 1.0 }, //1
		{ -1.0, 1.0, -1.0 }, //2
		{ -1.0, 1.0, 1.0 }, //3
		{ 1.0, -1.0, -1.0 }, //4
		{ 1.0, -1.0, 1.0 }, //5
		{ 1.0, 1.0, -1.0 }, //6
		{ 1.0, 1.0, 1.0 } //7


	};
	GLushort indexes[36] = { 0, 1, 3, 0, 2, 3,
		0, 4, 5, 0, 1, 5,
		2, 6, 7, 2, 3, 7,
		0, 4, 6, 0, 2, 6,
		1, 5, 7, 1, 3, 7,
		4, 5, 7, 4, 6, 7 };


	/*
	 *  load the vertex coordinate data
	 */
	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)+sizeof(normals), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(normals), normals);

	/*
	 *  load the vertex indexes
	 */
	glGenBuffers(1, &ibuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

	/*
	 *  compile and build the shader program
	 */
	vs = buildShader(GL_VERTEX_SHADER, "Lab_4.vs");
	fs = buildShader(GL_FRAGMENT_SHADER, "Lab_4.fs");
	program = buildProgram(vs, fs, 0);

	/*
	 *  link the vertex coordinates to the vPosition
	 *  variable in the vertex program
	 */
	glUseProgram(program);
	vPosition = glGetAttribLocation(program, "vPosition");
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);
	vNormal = glGetAttribLocation(program, "vNormal");
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*) sizeof(vertices));
	glEnableVertexAttribArray(vNormal);

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

	projection = glm::perspective(45.0f, ratio, 1.0f, 100.0f);

}

/*
 *  This procedure is called each time the screen needs
 *  to be redisplayed
 */
void displayFunc() {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 viewPerspective;
	int modelLoc;
	int normalLoc;
	int viewLoc;
	int colourLoc;
	model = glm::mat4(1.0);
	view = glm::lookAt(glm::vec3(eyex, eyey, eyez),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat3 normal = glm::transpose(glm::inverse(glm::mat3(view*model)));
	viewPerspective = projection * view;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);
	modelLoc = glGetUniformLocation(program, "model");
	viewLoc = glGetUniformLocation(program, "viewPerspective");
	glUniformMatrix4fv(viewLoc, 1, 0, glm::value_ptr(viewPerspective));
	normalLoc = glGetUniformLocation(program, "normalMat");
	glUniformMatrix3fv(normalLoc, 1, 0, glm::value_ptr(normal));

	colourLoc = glGetUniformLocation(program, "colour");
	glBindVertexArray(triangleVAO);
	glUniform4f(colourLoc, 1.0, 0.0, 0.0, 1.0);
	glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));
	glUniformMatrix3fv(normalLoc, 1, 0, glm::value_ptr(normal));
	glDrawElements(GL_LINES, 36, GL_UNSIGNED_SHORT, NULL);

	model = glm::translate(model, glm::vec3(2.0, 2.0, 0.0));
	normal = glm::transpose(glm::inverse(glm::mat3(view*model)));
	glUniform4f(colourLoc, 0.0, 1.0, 0.0, 1.0);
	glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));
	glUniformMatrix3fv(normalLoc, 1, 0, glm::value_ptr(normal));
	glDrawElements(GL_LINES, 36, GL_UNSIGNED_SHORT, NULL);
	
	model = glm::translate(model, glm::vec3(-4.0, 2.0, 0.0));
	normal = glm::transpose(glm::inverse(glm::mat3(view*model)));
	glUniform4f(colourLoc, 0.0, 0.0, 1.0, 1.0);
	glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(model));
	glUniformMatrix3fv(normalLoc, 1, 0, glm::value_ptr(normal));
	glDrawElements(GL_LINES, 36, GL_UNSIGNED_SHORT, NULL);
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
	float x = 45.0;
	switch (key) {
	case 'z':
		//x = x + 1.0f;
		projection = glm::perspective(-45.0f, 1.0f, 1.0f, 100.0f);
		break;
	case 'x':
		//x = x - 1.0f;
		projection = glm::perspective(-50.0f, 1.0f, 1.0f, 100.0f);
		break;
	case 'p':
		projection = glm::perspective(45.0f, 1.0f, 1.0f, 100.0f);
		break;
	case 'o':
		projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 1.0f, 100.0f);
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
	window = glutCreateWindow("Lab 4");

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

	eyex = 0.0;
	eyey = 15.0;
	eyez = 0.0;

	init();
	glEnable(GL_DEPTH_TEST);

	glClearColor(1.0, 1.0, 1.0, 1.0);

	glutMainLoop();

}
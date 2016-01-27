/************************************************
*
*             Laboratory 2

*	An example showing how tiny obj loader can
*	be used to load an obj file.
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
#include "tiny_obj_loader.h"
#include <iostream>
#include <algorithm>    

GLuint program;			// shader programs
//GLuint triangleVAO;
GLuint objVAO; // the data to be displayed
float angle = 0.0;
int triangles; // number of triangles
int window;

double theta = 1.5;
double phi = 1.5;
double r = 500.0;
float eyex = 0.0;
float eyey = 500.0;
float eyez = 0.0;

glm::mat4 projection;	// projection matrix

float cx;
float cy;
float cz;
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
	GLfloat (*vertices)[3]; // vertex coordinate data read and stored here
	GLfloat (*normals)[3]; // vertex normals data read and stored here
	GLuint *indices; // vertex indices data read and stored here
	std::vector<tinyobj::shape_t> shapes; // shapes data read in from the obj file
	std::vector<tinyobj::material_t> materials; // materials data read in from the obj file
	int nv; // total number of vertices
	int nn; // total number of normals
	int ni; // total number of indices

	glGenVertexArrays(1, &objVAO);
	glBindVertexArray(objVAO);

	/*  Load the obj file */

	tinyobj::LoadObj(shapes, materials, "Vase.obj", NULL);

	/*  Retrieve the vertex coordinate data */


	nv = shapes.at(0).mesh.positions.size()/3;
	vertices = new GLfloat[nv][3];

	int count = 0, xyz = 0, npositions = nv*3;
	float xh = -999999.0f, yh = -999999.0f, zh = -999999.0f, xl = 9999999.0f, yl = 9999999.0f, zl = 9999999.0f;
	while (count < nv) {
		//*vertices = shapes.at(0).mesh.positions.at(count);
		vertices[count][0] = shapes.at(0).mesh.positions.at(count * 3);
		vertices[count][1] = shapes.at(0).mesh.positions.at(count * 3 + 1);
		vertices[count][2] = shapes.at(0).mesh.positions.at(count * 3 + 2);
		//vertices[count][3] = 1.0f;
		//		std::cout << *vertices << "\n";

		if (xyz == 0) {
			xyz++;
			xh = std::max(vertices[count][0], xh);
			xl = std::min(vertices[count][0], xl);
		}
		else if (xyz == 1) {
			xyz++;
			yh = std::max(vertices[count][1], yh);
			yl = std::min(vertices[count][1], yl);
		}
		else {
			xyz = 0;
			zh = std::max(vertices[count][2], zh);
			zl = std::min(vertices[count][2], zl);
		}
		count++;
	}

	std::cout << "x max: " << xh << " x min: " << xl << "\n";
	std::cout << "y max: " << yh << " y min: " << yl << "\n";
	std::cout << "z max: " << zh << " z min: " << zl << "\n";

	/* Compute center of the object */



	cx = (xh + xl) / 2;
	cy = (yh + yl) / 2;
	cz = (zh + zl) / 2;

	printf("center: %f %f %f\n", cx, cy, cz); // cx, cy and cz are the coordinates of the centre

	/*  Retrieve the vertex normals */
	nn = shapes.at(0).mesh.positions.size()/3;
	normals = new GLfloat[nn][3];
	count = 0;
	while (count < nn) {
		//*normals = shapes.at(0).mesh.normals.at(count);
		normals[count][0] = shapes.at(0).mesh.normals.at(count * 3);
		normals[count][1] = shapes.at(0).mesh.normals.at(count * 3 + 1);
		normals[count][2] = shapes.at(0).mesh.normals.at(count * 3 + 2);
		count++;
	}

	/*  Retrieve the triangle indices */

	ni = shapes.at(0).mesh.indices.size();
	indices = new GLuint[ni];

	count = 0;
	while (count < ni) {
		indices[count] = shapes.at(0).mesh.indices.at(count);
		count++;
	}

	std::cout << "ni " << ni << "\n";
	std::cout << "nn " << nn << "\n";
	std::cout << "nv " << nv << "\n";
	triangles = ni/3;


	/*
	*  load the vertex coordinate data
	*/
	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, nv*sizeof(GLfloat[3]) + nn*sizeof(GLfloat[3]), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nv*sizeof(GLfloat[3]), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, nv*sizeof(GLfloat[3]), nn*sizeof(GLfloat[3]), normals);

	/*
	*  load the vertex indexes
	*/
	glGenBuffers(1, &ibuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ni*sizeof(GLuint), indices, GL_STATIC_DRAW);

	/*
	*  compile and build the shader program
	*/
	vs = buildShader(GL_VERTEX_SHADER, "lab_2.vs");
	fs = buildShader(GL_FRAGMENT_SHADER, "lab_2.fs");
	program = buildProgram(vs, fs, 0);

	/*
	*  link the vertex coordinates to the vPosition
	*  variable in the vertex program.  Do the same
	*  for the normal vectors.
	*/
	glUseProgram(program);
	vPosition = glGetAttribLocation(program, "vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);
	vNormal = glGetAttribLocation(program, "vNormal");
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*) (nv*sizeof(GLfloat[3])));
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

	projection = glm::perspective(45.0f, ratio, 1.0f, 800.0f);
}
/*
*  This procedure is called each time the screen needs
*  to be redisplayed
*/
void displayFunc() {
	glm::mat4 view;
	glm::mat4 modelViewPerspective;
	int modelLoc;
	int normalLoc;
	view = glm::lookAt(glm::vec3(eyex, eyey, eyez),
		glm::vec3(cx, cy, cz),
		glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat3 normal = glm::transpose(glm::inverse(glm::mat3(view)));
	modelViewPerspective = projection * view;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);
	modelLoc = glGetUniformLocation(program, "model");
	glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(modelViewPerspective));
	normalLoc = glGetUniformLocation(program, "normalMat");
	glUniformMatrix3fv(normalLoc, 1, 0, glm::value_ptr(normal));
	glBindVertexArray(objVAO);
	glDrawElements(GL_TRIANGLES, 3 * triangles, GL_UNSIGNED_INT, NULL);
	glutSwapBuffers();
}

/*
*  Update the value of angle on each update
*/
void idleFunc() {

	glutSetWindow(window);
	angle = angle + 0.001;
	glutPostRedisplay();

}

/*
*  Called each time a key is pressed on
*  the keyboard.
*/
void keyboardFunc(unsigned char key, int x, int y) {

	switch (key) {
	case 'a':
		phi -= 0.1;
		break;
	case 'd':
		phi += 0.1;
		break;
	case 'w':
		theta += 0.1;
		break;
	case 's':
		theta -= 0.1;
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
	window = glutCreateWindow("Lab Two");

	/*
	*  initialize glew
	*/
	GLenum error = glewInit();
	if (error != GLEW_OK) {
		printf("Error starting GLEW: %s\n", glewGetErrorString(error));
		exit(0);
	}
	eyex = 0.0;
	eyey = 500.0;
	eyez = 0.0;

	theta = 1.5;
	phi = 1.5;
	r = 500.0;
	glutDisplayFunc(displayFunc);
	glutIdleFunc(idleFunc);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keyboardFunc);

	init();

	glEnable(GL_DEPTH_TEST);
	glClearColor(.5, .5, .5, 1);

	glutMainLoop();

}
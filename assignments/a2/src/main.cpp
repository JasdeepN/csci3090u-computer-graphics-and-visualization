/************************************************
 *
 *             Example Six
 *
 *  This example illustrates how different
 *  lighting and shading models can be
 *  implemented in shader programs
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
#include <list>

std::list<GLuint> program;// shader programs 
GLuint currProgram;
GLuint objVAO;			// the data to be displayed
int triangles;			// number of triangles
int window;
int runNumber;
int testNumber = 0;
float sum = 0;

int vs;
int fs;
char vname[256];
char fname[256];


double getSeconds();
void buildShaders();

char *vertexName;
char *fragmentName;

double theta, phi;
double r;
double startTime = 0, endTime = 0;
double runTime = 0;

double totalRunTimeStart = 0;
double totalRunTimeEnd = 0;


float cx, cy, cz;

glm::mat4 projection;	// projection matrix
float eyex, eyey, eyez;	// eye position

/*
 *  The init procedure creates the OpenGL data structures
 *  that contain the triangle geometry, compiles our
 *  shader program and links the shader programs to
 *  the data.
 */

void init() {
	totalRunTimeStart = getSeconds();
	GLuint vbuffer;
	GLuint ibuffer;
	GLint vPosition;
	GLint vNormal;

	GLfloat *vertices;
	GLfloat *normals;
	GLuint *indices;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	int nv;
	int nn;
	int ni;
	int i;
	float xmin, ymin, zmin;
	float xmax, ymax, zmax;
	glGenVertexArrays(1, &objVAO);
	glBindVertexArray(objVAO);

	/*  Load the obj file */

	//std::string err = tinyobj::LoadObj(shapes, materials, "buddha.obj", 0);

	//std::string err = tinyobj::LoadObj(shapes, materials, "dragon.obj", 0);
	std::string err = tinyobj::LoadObj(shapes, materials, "vase.obj", 0);


	if (!err.empty()) {
		std::cerr << err << std::endl;
		return;
	}

	/*  Retrieve the vertex coordinate data */

	nv = shapes[0].mesh.positions.size();
	vertices = new GLfloat[nv];
	for (i = 0; i < nv; i++) {
		vertices[i] = shapes[0].mesh.positions[i];
	}

	/*
	 *  Find the range of the x, y and z
	 *  coordinates.
	 */
	xmin = ymin = zmin = 1000000.0;
	xmax = ymax = zmax = -1000000.0;
	for (i = 0; i < nv / 3; i++) {
		if (vertices[3 * i] < xmin)
			xmin = vertices[3 * i];
		if (vertices[3 * i] > xmax)
			xmax = vertices[3 * i];
		if (vertices[3 * i + 1] < ymin)
			ymin = vertices[3 * i + 1];
		if (vertices[3 * i + 1] > ymax)
			ymax = vertices[3 * i + 1];
		if (vertices[3 * i + 2] < zmin)
			zmin = vertices[3 * i + 2];
		if (vertices[3 * i + 2] > zmax)
			zmax = vertices[3 * i + 2];
	}
	/* compute center and print range */
	cx = (xmin + xmax) / 2.0f;
	cy = (ymin + ymax) / 2.0f;
	cz = (zmin + zmax) / 2.0f;
	printf("X range: %f %f\n", xmin, xmax);
	printf("Y range: %f %f\n", ymin, ymax);
	printf("Z range: %f %f\n", zmin, zmax);
	printf("center: %f %f %f\n", cx, cy, cz);

	/*  Retrieve the vertex normals */

	nn = shapes[0].mesh.normals.size();
	normals = new GLfloat[nn];
	for (i = 0; i < nn; i++) {
		normals[i] = shapes[0].mesh.normals[i];
	}

	/*  Retrieve the triangle indices */

	ni = shapes[0].mesh.indices.size();
	triangles = ni / 3;
	indices = new GLuint[ni];
	for (i = 0; i < ni; i++) {
		indices[i] = shapes[0].mesh.indices[i];
	}

	/*
	 *  load the vertex coordinate data
	 */
	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, (nv + nn)*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nv*sizeof(GLfloat), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, nv*sizeof(GLfloat), nn*sizeof(GLfloat), normals);

	/*
	 *  load the vertex indexes
	 */
	glGenBuffers(1, &ibuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ni*sizeof(GLuint), indices, GL_STATIC_DRAW);

	/*
	 *  build shaders and set the first set
	 */


	buildShaders();

	/*
	 *  link the vertex coordinates to the vPosition
	 *  variable in the vertex program.  Do the same
	 *  for the normal vectors.
	 */
	glUseProgram(currProgram);
	vPosition = glGetAttribLocation(currProgram, "vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);
	vNormal = glGetAttribLocation(currProgram, "vNormal");
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(nv*sizeof(vertices)));
	glEnableVertexAttribArray(vNormal);

}

void buildShaders(){
	/*
	*  compile and build the shader program
	*/
	sprintf(vname, "example6a.vs", vertexName);
	sprintf(fname, "example6a.frag", fragmentName);
	vs = buildShader(GL_VERTEX_SHADER, vname);
	fs = buildShader(GL_FRAGMENT_SHADER, fname);
	program.push_back(buildProgram(vs, fs, 0));
	printf("shader 1 compiled\n");


	sprintf(vname, "example6d.vs", vertexName);
	sprintf(fname, "example6d.frag", fragmentName);
	vs = buildShader(GL_VERTEX_SHADER, vname);
	fs = buildShader(GL_FRAGMENT_SHADER, fname);
	program.push_back(buildProgram(vs, fs, 0));
	printf("shader 2 compiled\n");

	sprintf(vname, "example6b.vs", vertexName);
	sprintf(fname, "example6b.frag", fragmentName);
	vs = buildShader(GL_VERTEX_SHADER, vname);
	fs = buildShader(GL_FRAGMENT_SHADER, fname);
	program.push_back(buildProgram(vs, fs, 0));
	printf("shader 3 compiled\n");

	sprintf(vname, "example6e.vs", vertexName);
	sprintf(fname, "example6e.frag", fragmentName);
	vs = buildShader(GL_VERTEX_SHADER, vname);
	fs = buildShader(GL_FRAGMENT_SHADER, fname);
	program.push_back(buildProgram(vs, fs, 0));
	printf("shader 4 compiled\n");

	sprintf(vname, "example6c.vs", vertexName);
	sprintf(fname, "example6c.frag", fragmentName);
	vs = buildShader(GL_VERTEX_SHADER, vname);
	fs = buildShader(GL_FRAGMENT_SHADER, fname);
	program.push_back(buildProgram(vs, fs, 0));
	printf("shader 5 compiled\n");

	sprintf(vname, "example6f.vs", vertexName);
	sprintf(fname, "example6f.frag", fragmentName);
	vs = buildShader(GL_VERTEX_SHADER, vname);
	fs = buildShader(GL_FRAGMENT_SHADER, fname);
	program.push_back(buildProgram(vs, fs, 0));
	printf("shader 6 compiled\n");


	currProgram = program.front();
	glUseProgram(currProgram);
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

	float ratio = 1.0f * w / h;

	glViewport(0, 0, w, h);

	projection = glm::perspective(45.0f, ratio, 1.0f, 800.0f);

}

/*
 *  This procedure is called each time the screen needs
 *  to be redisplayed
 */
void displayFunc() {
	glm::mat4 view;
	int modelViewLoc;
	int projectionLoc;
	int normalLoc;
	int eyeLoc;

	view = glm::lookAt(glm::vec3(eyex, eyey, eyez),
		glm::vec3(cx, cy, cz),
		glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat3 normal = glm::transpose(glm::inverse(glm::mat3(view)));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(currProgram);
	modelViewLoc = glGetUniformLocation(currProgram, "modelView");
	glUniformMatrix4fv(modelViewLoc, 1, 0, glm::value_ptr(view));
	projectionLoc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, 0, glm::value_ptr(projection));
	normalLoc = glGetUniformLocation(currProgram, "normalMat");
	glUniformMatrix3fv(normalLoc, 1, 0, glm::value_ptr(normal));
	eyeLoc = glGetUniformLocation(currProgram, "eye");

	glUniform3f(eyeLoc, eyex, eyey, eyez);

	glBindVertexArray(objVAO);

	startTime = getSeconds();
	//printf("start time %f\n", startTime);
	glDrawElements(GL_TRIANGLES, 3 * triangles, GL_UNSIGNED_INT, NULL);
	glFinish();
	endTime = getSeconds();
	//printf("end time %f\n", endTime);


	glutSwapBuffers();

}

/*
*	returns time in seconds
*/
double getSeconds() {
	LARGE_INTEGER freq, val;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&val);
	return (double)val.QuadPart / (double)freq.QuadPart;
}

/*
*  Update the value of angle on each update
*/
void idleFunc() {

	//glutSetWindow(window);
	//printf("in idle\n");
	runTime = endTime - startTime;
	if (!program.empty())
	{
		if (runNumber < 10)
		{
			//printf("runtime %f end time %f start time %f\n", runTime, startTime, endTime);
			currProgram = program.front();
			glUseProgram(currProgram);
			printf("shader ver %i runtime %f\n", currProgram / 3, runTime);
			sum = sum + runTime;
			//startTime = 0;
			//endTime = 0;
			glutPostRedisplay();
			runNumber++;
		}
		else if (runNumber == 10){
			testNumber++;
			printf("test number %i complete avg runtime %f\n", testNumber, sum/runNumber);
			system("pause");
			//fprintf() DO THIS SHIT
			program.pop_front();
			runNumber = 0;
			sum = 0;
		}
	}
	else if (program.empty())
	{
		//printf("finshed\n");
		//system("pause");
		printf("total runtime: %f", (totalRunTimeEnd - totalRunTimeStart));
		exit(0);
	}
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

	if (argc > 1) {
		vertexName = argv[1];
	}
	else {
		vertexName = "a";
	}
	if (argc > 2) {
		fragmentName = argv[2];
	}
	else {
		fragmentName = "a";
	}

	/*
	 *  initialize glut, set some parameters for
	 *  the application and create the window
	 */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(500, 500);
	window = glutCreateWindow("Assignment 2");

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
	eyey = 0.0;
	//eyez = -2.0 ; //buddha and drag
	eyez = 500.0; //vase

	theta = 0.5;
	phi = 1.5;
	r = 500.0;

	init();

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	glutMainLoop();

}
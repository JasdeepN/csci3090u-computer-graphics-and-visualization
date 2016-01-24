/********************************************
 *
 *             Example9
 *
 *  This example shows the use of multiple
 *  textures, both read from files.  The objects
 *  used in example 7 are used in this example.
 *
 ******************************************/
#include <Windows.h>
#include <gl/glew.h>
#include <gl/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shaders.h"
#include <stdio.h>
#include <FreeImage.h>

GLuint program;			// shader programs
GLuint objVAO;			// the data to be displayed
int triangles;			// number of triangles
int window;
GLuint texName1;
GLuint texName2;
glm::mat4 projection;	// projection matrix

float time = 0.0;

struct textureStruct {
	int height;
	int width;
	int bytes;
	unsigned char *data;
};

textureStruct* loadImage(char *filename) {
	int i,j;
	FIBITMAP *bitmap;
	BYTE *bits;
	int width;
	int height;
	int bytes;
	unsigned char *data;
	textureStruct *result;
	int k;

	result = new textureStruct();

	bitmap = FreeImage_Load(FIF_PNG, filename, PNG_DEFAULT);
	height = FreeImage_GetHeight(bitmap);
	width = FreeImage_GetWidth(bitmap);
	bytes = FreeImage_GetBPP(bitmap)/8;
	printf("image size: %d %d %d\n",width, height, bytes);
	data = new unsigned char[width*height*bytes];
	result->height = height;
	result->width = width;
	result->bytes = bytes;
	result->data = data;

	k = 0;
	for(j=0; j<height; j++) {
		bits = FreeImage_GetScanLine(bitmap,j);
			for(i=0; i<width; i++) {
				data[k++] = bits[FI_RGBA_RED];
				data[k++] = bits[FI_RGBA_GREEN];
				data[k++] = bits[FI_RGBA_BLUE];
				if(bytes > 3) {
					data[k++] = bits[FI_RGBA_ALPHA];
				}
				bits += bytes;
			}
	}
	FreeImage_Unload(bitmap);
	return(result);
}

/*
 *  Initialize the vertex buffer object and
 *  the texture.
 */
void init(void) { 
	GLuint vbuffer;
	GLuint ibuffer;
	GLuint vs;
	GLuint fs;
	GLuint vPosition;
	GLuint vTexture;
	textureStruct *texture1;
	textureStruct *texture2;
	GLuint tex1;
	GLuint tex2;

	glGenVertexArrays(1, &objVAO);
	glBindVertexArray(objVAO);

	/*
	 *  Two squares, one straight on, the other
	 *  slanted into the screen.
	 */
	GLfloat vertices[] = {
		 -2.0, -1.0, 0.0,
		 -2.0, 1.0, 0.0,
		  0.0, 1.0, 0.0,
		  0.0, -1.0, 0.0,
		  1.0, -1.0, 0.0,
		  1.0, 1.0, 0.0,
		  2.41421, 1.0, -1.41421,
		  2.41421, -1.0, -1.41421 };

	GLfloat texCoords[] = {
		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0,
		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0};

	GLuint indices[] = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4 };

	glGenBuffers(1, &vbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)+sizeof(texCoords), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(texCoords), texCoords);

	glGenBuffers(1, &ibuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	vs = buildShader(GL_VERTEX_SHADER, "example9.vs");
	fs = buildShader(GL_FRAGMENT_SHADER, "example9.fs");
	program = buildProgram(vs,fs,0);

	glUseProgram(program);
	vPosition = glGetAttribLocation(program,"vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);
	vTexture = glGetAttribLocation(program, "vTexture");
	glVertexAttribPointer(vTexture, 2, GL_FLOAT, GL_FALSE, 0, (void*) sizeof(vertices));
	glEnableVertexAttribArray(vTexture);
	triangles = 4;

    glClearColor (0.3, 0.3, 0.3, 1.0);
    glEnable(GL_DEPTH_TEST);

	/*
	 *  Create the texture.
	 */
    
    glGenTextures(1, &texName1);
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texName1);

	texture1 = loadImage("test.png");
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture1->width, 
	texture1->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
	texture1->data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenTextures(1, &texName2);
	glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texName2);

	texture2 = loadImage("test3.png");
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture2->width, 
	texture2->height, 0, GL_RGB, GL_UNSIGNED_BYTE, 
	texture2->data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	tex1 = glGetUniformLocation(program, "tex1");
	tex2 = glGetUniformLocation(program, "tex2");
	glUniform1i(tex1, 0);
	glUniform1i(tex2, 1);

}

void reshapeFunc(int w, int h) {

    glViewport(0, 0, w, h);
    float ratio = 1.0f * w / h;
    
	projection = glm::perspective(60.0f, ratio, 1.0f,30.0f);
    
    glTranslatef(0.0, 0.0, -3.6);
}


void displayFunc(void) {
	glm::mat4 view;
	int modelViewLoc;
	GLuint timeLoc;

	view = projection * glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, -3.6));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);
	modelViewLoc = glGetUniformLocation(program,"modelView");
	glUniformMatrix4fv(modelViewLoc, 1, 0, glm::value_ptr(view));

	timeLoc = glGetUniformLocation(program, "time");
	glUniform1f(timeLoc, time);

	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texName1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texName2);
	glBindVertexArray(objVAO);
	glDrawElements(GL_TRIANGLES, 3*triangles, GL_UNSIGNED_INT, NULL);
    
    glutSwapBuffers();
}

void idleFunc() {

	glutSetWindow(window);
	time = time + 0.001;
	glutPostRedisplay();

}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(500,500);
	window = glutCreateWindow("Example 9 - image textures");

	/*
	 *  initialize glew
	 */
	GLenum error = glewInit();
	if(error != GLEW_OK) {
		printf("Error starting GLEW: %s\n",glewGetErrorString(error));
		exit(0);
	}

    glutReshapeFunc (reshapeFunc);
    glutDisplayFunc(displayFunc);
	glutIdleFunc(idleFunc);

	init();
	glClearColor(0.3,0.3,0.3,1.0);

    glutMainLoop();
    return 0;
}


/*
 *  Simple vertex shader for the first
 *  texture example.
 */

#version 330 core

in vec4 vPosition;
in vec2 vTexture;



uniform mat4 modelView;

out vec2 texCoord;

void main(void) {

	mat2 rotation_matrix;
	gl_Position = modelView * vPosition;
	rotation_matrix[0]= vec2(cos(5), sin(5));
	rotation_matrix[1]= vec2(-sin(5), cos(5));
	texCoord = rotation_matrix * vTexture;

}
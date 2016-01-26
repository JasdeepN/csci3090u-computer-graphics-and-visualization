/*
 *  Simple vertex shader for example four
 */

in vec4 vPosition;
in vec3 vNormal;
uniform mat4 model;
uniform mat3 normalMat;
out vec3 normal;
out vec3 fragmentColor;
layout(location = 1) in vec3 vertexColor;

void main() {

	gl_Position = model * vPosition;
	normal = normalMat* vNormal;
	fragmentColor = vertexColor;
}
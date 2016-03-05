/*
 *  Simple vertex shader for example six
 */

in vec4 vPosition;
in vec3 vNormal;
in vec3 eye;
uniform mat4 modelView;
uniform mat4 projection;
uniform mat3 normalMat;
out vec3 normal;
out vec4 position;
out vec4 Fcolour;

void main() {


	gl_Position = projection * modelView * vPosition;
	position = vPosition;
	normal = vNormal;


	vec3 N;
	vec3 L = vec3(1.0, 1.0, 0.0);
	vec4 colour = vec4(1.0, 0.0, 0.0, 1.0);
	vec4 Lcolour = vec4(1.0, 1.0, 1.0, 1.0);
	vec3 H = normalize(L + vec3(0.0, 0.0, 1.0));
    L = normalize(L);
	float diffuse;
	float specular;
	float n = 100.0;
    float ambient = 0.3;

	N = normalize(normal);
	diffuse = dot(N,L);
	if(diffuse < 0.0) {
		diffuse = 0.0;
		specular = 0.0;
	} else {
		specular = pow(max(0.0, dot(N,H)),n);
	}

	Fcolour = min(ambient*colour + diffuse*colour*Lcolour + Lcolour*specular, vec4(1.0));
} 
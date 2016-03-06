/*
 *  Simple vertex shader for example six
 */

in vec4 vPosition;
in vec3 vNormal;
uniform mat4 modelView;
uniform mat4 projection;
uniform mat3 normalMat;
out vec3 normal;
out vec4 position;
uniform vec3 eye;
out vec4 Fcolour;

void main() {

	gl_Position = projection * modelView * vPosition;
	position = vPosition;
	normal = vNormal;

	vec3 N;
	vec3 Lposition = vec3(500.0, 500.0, 800.0);
	vec4 colour = vec4(1.0, 0.0, 0.0, 1.0);
	vec4 Lcolour = vec4(1.0, 1.0, 1.0, 1.0);
	vec3 spotDirection = vec3(500.0, 500.0, 750.0);
	float spotCutoff = 0.85;
	float spotExp = 200.0;
	float spotCos;
	float atten;
	vec3 H;
	float diffuse;
	float specular;
	float n = 100.0;
	vec3 L;

	N = normalize(normal);
	L = Lposition - position.xyz;
	L = normalize(L);
	H = normalize(L + eye);
	spotCos = dot(L, normalize(spotDirection));
	if(spotCos < spotCutoff) {
		atten = 0;
	} else {
		atten = pow(spotCos,spotExp);
	}
	diffuse = dot(N,L) * atten;
	if(diffuse < 0.0) {
		diffuse = 0.0;
		specular = 0.0;
	} else {
		specular = pow(max(0.0, dot(N,H)),n) * atten;
	}

	Fcolour = min(0.3*colour + diffuse*colour*Lcolour + Lcolour*specular, vec4(1.0));
} 
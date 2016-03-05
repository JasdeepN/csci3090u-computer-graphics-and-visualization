/*
 *  Simple fragment sharder for laboratory two
 */

in vec3 normal;

void main() {
	vec3 N;
	vec3 L = vec3(1.0, 1.0, 0.0); //direction vector for light
	vec4 colour = vec4(1.0, 0.0, 0.0, 1.0);
	vec4 Lcolour = vec4(1.0, 1.0, 1.0, 1.0);
	vec3 H = normalize(L + vec3(0.0, 0.0, 1.0)); //L + viewing direction
	float diffuse;
	float specular;
	float n = 100.0; //shininess (brightness for specular
    float ambient = 0.3;


	N = normalize(normal);
	L = normalize(L);
	diffuse = dot(N,L);
	if(diffuse < 0.0) {
		diffuse = 0.0;
		specular = 0.0;
	} else {
		specular = pow(max(0.0, dot(N,H)),n);
	}

	gl_FragColor = min(ambient*colour + diffuse*colour*Lcolour + Lcolour*specular, vec4(1.0));
	gl_FragColor.a = colour.a;
}
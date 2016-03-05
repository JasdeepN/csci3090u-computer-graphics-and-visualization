/*
 *  Simple fragment sharder for laboratory two
 */

in vec3 normal;
in vec4 position;
in vec4 Fcolour;

void main() {
	vec4 colour = vec4(1.0, 0.0, 0.0, 1.0);
	gl_FragColor = Fcolour;
	gl_FragColor.a = colour.a;
}
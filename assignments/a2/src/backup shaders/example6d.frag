
//  Simple fragment sharder for laboratory two
 

uniform vec3 eye;
in vec4 Fcolour;

void main() {
	vec4 colour = vec4(1.0, 0.0, 0.0, 1.0);

	gl_FragColor = Fcolour;
	gl_FragColor.a = colour.a;
}
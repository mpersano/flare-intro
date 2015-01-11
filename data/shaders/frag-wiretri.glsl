varying float edge_dist;
varying float world_z;

uniform vec3 color;
uniform float thick;

void main(void)
{
	float s = 1. - smoothstep(thick, thick + 1., edge_dist/world_z);
	gl_FragColor = vec4((.1 + .9*s)*color, 1);
}

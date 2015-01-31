varying float edge_dist;
varying float world_z;

uniform vec3 color;

void main(void)
{
	float s = 1. - smoothstep(1., .9, edge_dist/world_z);
	gl_FragColor = vec4((.2 + .8*s)*color, 1);
}

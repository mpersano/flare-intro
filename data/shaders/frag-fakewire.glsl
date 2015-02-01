varying float edge_dist;
varying float world_z;

uniform vec3 color;

void main(void)
{
	float e = edge_dist;
	float s = 1. - smoothstep(.85, .8, edge_dist);
	float q = smoothstep(1., .95, edge_dist);

	gl_FragColor = vec4((.2 + .8*s)*color*q*smoothstep(200, 100, world_z), 1);
}

varying float edge_dist;
varying float world_z;

void main(void)
{
	vec3 inner_color = vec3(.125, .125, .125);
	vec3 border_color = vec3(.5, .5, .5);

	float q = smoothstep(.7, .6, edge_dist);
	float r = smoothstep(1., .9, edge_dist);

	float fog = smoothstep(700., 500., world_z);

	vec3 color = (q*inner_color + (1. - q)*border_color)*fog*r;

	gl_FragColor = vec4(color, 1);
}

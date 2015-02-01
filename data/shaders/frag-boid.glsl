varying float edge_dist;

void main(void)
{
	vec3 inner_color = vec3(0., 0., .05);
	vec3 border_color = vec3(.5, .5, .5);

	float q = smoothstep(.9, .85, edge_dist);

	vec3 color = q*inner_color + (1. - q)*border_color;

	gl_FragColor = vec4(color, 1);
}

varying float edge_dist;

void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
	edge_dist = gl_MultiTexCoord0.x;
}

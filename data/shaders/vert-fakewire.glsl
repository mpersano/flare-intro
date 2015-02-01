varying float edge_dist;
varying float world_z;

void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
	world_z = gl_Position.z;
	edge_dist = gl_MultiTexCoord0.x;
}

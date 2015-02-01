varying vec3 color;

void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
	color = gl_MultiTexCoord0.xyz;
}

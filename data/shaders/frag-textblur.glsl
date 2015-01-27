uniform sampler2D texture;

uniform float dist;
uniform float phase;
uniform float vscale;
uniform float fade;

void main(void)
{
	vec2 dir = vec2(dist*.75*(2. + sin(30.*vscale*gl_TexCoord[0].t + phase)), 0);

	gl_FragColor = 
		fade*
		(.05*texture2D(texture, gl_TexCoord[0].st - 4.*dir) +
		 .09*texture2D(texture, gl_TexCoord[0].st - 3.*dir) +
		 .12*texture2D(texture, gl_TexCoord[0].st - 2.*dir) +
		 .15*texture2D(texture, gl_TexCoord[0].st - dir) +
		 .16*texture2D(texture, gl_TexCoord[0].st) +
		 .15*texture2D(texture, gl_TexCoord[0].st + dir) +
		 .12*texture2D(texture, gl_TexCoord[0].st + 2.*dir) +
		 .09*texture2D(texture, gl_TexCoord[0].st + 3.*dir) +
		 .05*texture2D(texture, gl_TexCoord[0].st + 4.*dir));
}


#version 430 core

layout	(binding=0) uniform sampler2D fbo;

out vec4 fColor;

in vData
{
	vec4 color;
	vec2 st;
} vIn;


void main ()
{
	fColor = texture2D (fbo, vIn.st) ;
	//fColor = 	vec4 (gl_FragCoord.x/1900, gl_FragCoord.y/1000.0, 0.0, 1.0);
	//fColor = vec4(1.0,0.0,0.0,1.0);
}

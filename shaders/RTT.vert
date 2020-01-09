

#version 430 core

layout 	( location = 0 ) in vec4 position;
layout 	( location = 1 ) in vec4 colors;
layout 	( location = 2 ) in vec2 st;

layout ( binding = 0  ) uniform transform
{
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;
} Transform;

out vData
{
	vec4 color;
	vec2 st;
} vOut;

void main ()
{
	vec4 vert = position;
	vert.z = -0.1;
	mat4 MVP 	= Transform.projection * Transform.view * Transform.model;
	vOut.color 	= colors;
	vOut.st	= st;
	gl_Position = MVP * vert;
}



#version 440 core

layout ( location = 0 ) in vec4 position;
layout ( location = 1 ) in vec3 color;

layout ( binding = 0  ) uniform tranform
{
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;
} Transform;

out vData
{
	vec3 colorPos;
	vec3 trueColor;
} vOut;

void main ()
{
	mat4 MVP = Transform.projection * Transform.view * Transform.model;
	vOut.trueColor 	= color;
	vOut.colorPos	= position.xyz;
	gl_Position = MVP * position;
}

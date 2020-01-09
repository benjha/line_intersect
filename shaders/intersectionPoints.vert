

#version 430 core

#define BINDING_IMAGE_HEAD_INDEX 	1
#define BINDING_BUFFER_LINKED_LIST 	1

uniform int WINDOW_WIDTH;
uniform int WINDOW_HEIGHT;


layout	( binding = 0  	) uniform transform
{
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;
} Transform;


struct NodeType
{
	vec4 color;
	float depth;
	uint nextIndex;
};

layout(binding = BINDING_IMAGE_HEAD_INDEX,		r32ui ) uniform uimage2D u_headIndex;
layout(binding = BINDING_BUFFER_LINKED_LIST, 	std430) buffer LinkedList
{
	NodeType nodes[];
	// Padding[]
};

void main ()
{
	//uint fragmentIndices[MAX_FRAGMENTS];

	int x = gl_VertexID % int(WINDOW_WIDTH);    // % is the "modulo operator", the remainder of i / width;
	int y = gl_VertexID / int(WINDOW_HEIGHT);

	uint walkerIndex = imageLoad(u_headIndex, ivec2(x,y)).r;

	vec4 vert = vec4(0.0);
	// Check, if a fragment was written.
	if (walkerIndex != 0xffffffff)
		if (nodes[walkerIndex].nextIndex != 0xffffffff)
	{
		vert.xyz = nodes[walkerIndex].color.xyz;
	}
	vert.z = 0.0;
	vert.w = 1.0;

	mat4 MVP 	= Transform.projection * Transform.view * Transform.model;
	gl_Position = MVP * vert;
}

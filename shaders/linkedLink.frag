
#version 440 core

#define BINDING_ATOMIC_FREE_INDEX 	1
#define BINDING_IMAGE_HEAD_INDEX 	1
#define BINDING_BUFFER_LINKED_LIST 	1

struct NodeType
{
	vec4 color;
	float depth;
	uint nextIndex;
};

layout(binding = BINDING_ATOMIC_FREE_INDEX			) uniform 	atomic_uint		u_freeNodeIndex;
layout(binding = BINDING_IMAGE_HEAD_INDEX, r32ui	) uniform 	uimage2D 		u_headIndex;
layout(binding = BINDING_BUFFER_LINKED_LIST, std430	) buffer	LinkedList
{
	NodeType nodes[];
	// Padding[]
};

uniform uint u_maxNodes;

out vec4 fColor;

in vData
{
	vec3 colorPos;
	vec3 trueColor;
} vIn;



void main ()
{


// Get the index of the next free node in the buffer.
	uint freeNodeIndex = atomicCounterIncrement(u_freeNodeIndex);


// Check, if still space in the buffer.
	if(freeNodeIndex < u_maxNodes)
	{
		// Replace new index as the new head and gather the previous head, which will be the next index of this entry.
		uint nextIndex = imageAtomicExchange(u_headIndex, ivec2(gl_FragCoord.xy), freeNodeIndex);
		
						
		// Store the color, depth and the next index for later resolving.
		//TODO: check the effect of alpha component
		nodes[freeNodeIndex].color = vec4(vIn.colorPos, 1.0);
		nodes[freeNodeIndex].depth = gl_FragCoord.z;
		nodes[freeNodeIndex].nextIndex = nextIndex;
  	} 

	//fColor = vec4 (vIn.trueColor, 1.0);

	//fColor = vIn.color;
	//fColor = vec4 (normalColor, 1.0);
	//fColor = vec4 (1.0, 1.0, 0.0, 1.0);
}

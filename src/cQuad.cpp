
#include <vector>
#include <iostream>

#include "cQuad.h"
#include "cMacros.h"
#include "Vertex.h"


cQuad::cQuad ()
{
	vbo = 0;
}

//=======================================================================================

cQuad::~cQuad()
{
	FREE_GLBO	( vbo );
}

//=======================================================================================

void cQuad::init (unsigned int x0, unsigned int y0,unsigned int x1, unsigned int y1, unsigned int tw, unsigned int th)
{
	std::vector<Vertex>	vertices;
	Vertex vertex;

// V1
	INITVERTEX (vertex);
	vertex.location[0] = x0 ;
	vertex.location[1] = y0 ;
	vertex.location[2] = 0.0f;
	vertex.location[3] = 1.0f;

	vertex.texture[0] = 0.0f; //* tw;
	vertex.texture[1] = 0.0f; //* th;

	vertex.color[0] = 1.0f;
	vertex.color[1] = 0.0f;
	vertex.color[2] = 0.0f;
	vertex.color[3] = 0.0f;

	vertices.push_back(vertex);

//V2
	vertex.location[0] = x1 ;
	vertex.location[1] = y0 ;
	vertex.location[2] = 0.0f;
	vertex.location[3] = 1.0f;

	vertex.texture[0] = 1.0f * tw;
	vertex.texture[1] = 0.0f * th;

	vertex.color[0] = 1.0f;
	vertex.color[1] = 1.0f;
	vertex.color[2] = 0.0f;
	vertex.color[3] = 0.0f;

	vertices.push_back(vertex);

//V3
	vertex.location[0] = x1;
	vertex.location[1] = y1;
	vertex.location[2] = 0.0f;
	vertex.location[3] = 1.0f;

	vertex.texture[0] = 1.0f * tw;
	vertex.texture[1] = 1.0f * th;

	vertex.color[0] = 1.0f;
	vertex.color[1] = 1.0f;
	vertex.color[2] = 0.0f;
	vertex.color[3] = 0.0f;

	vertices.push_back(vertex);

//V4
	vertex.location[0] = x0;
	vertex.location[1] = y1;
	vertex.location[2] = 0.0f;
	vertex.location[3] = 1.0f;

	vertex.texture[0] = 0.0f * tw;
	vertex.texture[1] = 1.0f * th;

	vertex.color[0] = 1.0f;
	vertex.color[1] = 1.0f;
	vertex.color[2] = 0.0f;
	vertex.color[3] = 0.0f;

	vertices.push_back(vertex);

	size = vertices.size ();

	//std::cout << size << std::endl;

	glGenBuffers						( 1, &vbo );
	glNamedBufferDataEXT				( vbo, sizeof ( Vertex ) * size, &vertices[0], GL_STATIC_DRAW );
	glGetNamedBufferParameterui64vNV 	( vbo, GL_BUFFER_GPU_ADDRESS_NV, &vboAddres );
	glMakeNamedBufferResidentNV			( vbo, GL_READ_ONLY );
	glEnableClientState					( GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV ); // Can this line affect other ways of displaying vertices ?

	vertices.erase (vertices.begin(), vertices.end());
}

//=======================================================================================

void cQuad::draw()
{
	glEnableVertexAttribArray	( 0 ); // from the shader: layout ( location = 0 ) position
	glEnableVertexAttribArray	( 1 ); // from the shader: layout ( location = 1 ) colors
	glEnableVertexAttribArray	( 2 ); // from the shader: layout ( location = 2 ) textures

	glVertexAttribFormatNV		( 0, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ) ); // specifying vertex positions format
	glVertexAttribFormatNV		( 1, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ) ); // specifying vertex colors format
	glVertexAttribFormatNV		( 2, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ) ); // specifying tex's format

	glBufferAddressRangeNV		( GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV, 0, vboAddres, sizeof ( Vertex ) * size );
	glBufferAddressRangeNV		( GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV, 1, vboAddres + (GLsizei)COLOR_OFFSET, sizeof ( Vertex ) * size - (GLsizei)COLOR_OFFSET   );
	glBufferAddressRangeNV		( GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV, 2, vboAddres + (GLsizei)TEXTURE_OFFSET, sizeof ( Vertex ) * size - (GLsizei)TEXTURE_OFFSET  );

	glDrawArrays				( GL_QUADS, 0, size);

	glDisableVertexAttribArray	( 0 );
	glDisableVertexAttribArray	( 1 );
	glDisableVertexAttribArray	( 2 );


}

//=======================================================================================

void cQuad::drawInstanced	( unsigned int numInstances, unsigned int target, unsigned int posID)
{
	glEnableVertexAttribArray	( 0 ); // from the shader: layout ( location = 0 ) position
	glEnableVertexAttribArray	( 1 ); // from the shader: layout ( location = 1 ) colors
	glEnableVertexAttribArray	( 2 ); // from the shader: layout ( location = 2 ) textures

	glVertexAttribFormatNV		( 0, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ) ); // specifying vertex positions format
	glVertexAttribFormatNV		( 1, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ) ); // specifying vertex colors format
	glVertexAttribFormatNV		( 2, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ) ); // specifying tex's format

	glBindMultiTextureEXT		(  GL_TEXTURE0 + 1, target, posID);

	glBufferAddressRangeNV		( GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV, 0, vboAddres, sizeof ( Vertex ) * size );
	glBufferAddressRangeNV		( GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV, 1, vboAddres + (GLsizei)COLOR_OFFSET, sizeof ( Vertex ) * size - (GLsizei)COLOR_OFFSET   );
	glBufferAddressRangeNV		( GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV, 2, vboAddres + (GLsizei)TEXTURE_OFFSET, sizeof ( Vertex ) * size - (GLsizei)TEXTURE_OFFSET  );

	glDrawArraysInstanced		( GL_QUADS, 0, size, numInstances );

	glBindMultiTextureEXT		(  GL_TEXTURE0 + 1, target, 0);

	glDisableVertexAttribArray	( 0 );
	glDisableVertexAttribArray	( 1 );
	glDisableVertexAttribArray	( 2 );


}


//
//										Benjamin Hernandez, PhD. benjamin.hernandez@bsc.es
//																	   All Rights Reserved
// 														   Barcelona Supercomputing Center
//
//															   See ReadMe.txt for details
//
//=======================================================================================

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <GL/glew.h>


#include "cMacros.h"
#include "cLines.h"
#include "Vertex.h"


cLines::cLines ( std::string &filename, float scale )
{
	cLines::vbo				= 0;
	cLines::indexVbo		= 0;
	cLines::vboAddress		= 0;
	cLines::indexVboAddress	= 0;
	cLines::size			= 0;

	//genPoints ( 1024, 70 );
	parse (filename);

	fillVBO	();

	vertices.erase (vertices.begin(), vertices.end());

}
//
//=======================================================================================
//
cLines::~cLines	( void )
{
	FREE_GLBO ( vbo );
	FREE_GLBO ( indexVbo );
}
//
//=======================================================================================
//
void cLines::parse (string &filename)
{
	char 	comma;
	float	lon, lat;
	float 	color[4];
	unsigned int numlines = 80000, nlines = 0, nvert;
	unsigned int i;
	unsigned int idx = 0;
	vector<float> pos;
	string line;
	string::size_type sz;
	ifstream file;

	Vertex vertex;

	file.open (filename.data(), ios::in);
	if (!file.is_open())
	{
		std::cout << "File " << filename << " not found! \n";
		exit (1);
	}
	int n;
	std::string str;
	while (!file.eof())
	{
		file >> line;
		//std::cout<< line <<std::endl;
		n = line.find(",");
		str = line.substr (n+1);
		n = str.find(",");
		nvert = atoi (str.substr(n+1).data());

		color[0] = rand () % 255 / 255.0f;
		color[1] = rand () % 128 / 255.0f;;
		color[2] = rand () % 255 / 255.0f;;

		for (i=0;i<nvert && !file.eof();i++)
		{
			INITVERTEX( vertex );

			file >> lon >> comma >> lat;
			pos.push_back(lon);
			pos.push_back(lat);
			vertex.location[0] = lon * 10;
			vertex.location[1] = lat * 10;
			vertex.location[3] = 1.0f;

			vertex.color[0] = color[0];
			vertex.color[1] = color[1];
			vertex.color[2] = color[2];
			vertex.color[3] = 1.0f;

			vertices.push_back	( vertex	);
/*
			if (i%2==1 && i+1 < nvert)
			{
				vertices.push_back	( vertex	);
			}
	*/		indices.push_back	( idx++ 		);

			//pos.push_back(lon2x_m(lon));
			//pos.push_back(lat2y_m(lat));
			nlines++;
		}
		//idx = 0;
		indices.push_back (64*1024);

	}

	file.close ();
}
//
//=======================================================================================
//
void cLines::genPoints ( unsigned int numLines, unsigned int numNodes )
{
	float color[4];
	Vertex vertex;
	cLines::numLines = numLines;
	cLines::numNodes = numNodes;
	float z = 10.0f;
	unsigned int idx = 0.0;

	for( unsigned int n = 0; n < numLines; ++n )
	{
		color[0] = rand () % 255 / 255.0f;
		color[1] = rand () % 128 / 255.0f;;
		color[2] = rand () % 255 / 255.0f;;

		for( unsigned int f = 0; f < numNodes; ++f )
		{

			INITVERTEX( vertex );

			vertex.color[0] = color[0];
			vertex.color[1] = color[1];
			vertex.color[2] = color[2];
			vertex.color[3] = 1.0f;

			vertex.location[0] = f*4.0 - numNodes*2.0;
			vertex.location[1] = ((rand() % (100)) - 5) ;
			vertex.location[2] = 0.0f;
			vertex.location[3] = 1.0f;

			z-= 0.000001;

			vertices.push_back	( vertex	);
			indices.push_back	( idx++ 		);
		}
		indices.push_back (64*1024);
	}

}
//
//=======================================================================================
//
void cLines::fillVBO( )
{
	int indexVboSize;
	size = vertices.size ();

	cout << size << endl;
	cout << "Indices size: " <<  indices.size() << endl;
	if (!vbo)
	{
		glGenBuffers						( 1, &vbo );
		glNamedBufferDataEXT				( vbo, sizeof ( Vertex ) * size, &vertices[0], GL_STATIC_DRAW );
		glGetNamedBufferParameterui64vNV 	( vbo, GL_BUFFER_GPU_ADDRESS_NV, &vboAddress );
		glMakeNamedBufferResidentNV			( vbo, GL_READ_ONLY );

		glGenBuffers						( 1, &indexVbo);
		glNamedBufferDataEXT				( indexVbo, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
		glGetNamedBufferParameterui64vNV	( indexVbo, GL_BUFFER_GPU_ADDRESS_NV, &indexVboAddress);
		//glGetNamedBufferParameteriv			( indexVbo, GL_BUFFER_SIZE, &indexVboSize);
		glMakeNamedBufferResidentNV			( indexVbo, GL_READ_ONLY );


		glEnableClientState					( GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV );  // Enable Vertex Buffer Unified Memory (VBUM) for the vertex attributes


		glEnable(GL_PRIMITIVE_RESTART);
		glPrimitiveRestartIndex (64*1024);
	}
	//cout << "indexVboSize: " << indexVboSize << endl;
}
//
//=======================================================================================
//
void cLines::draw ()
{
	glEnableClientState					( GL_ELEMENT_ARRAY_UNIFIED_NV ); // Enable Vertex Buffer Unified Memory (VBUM) for the indices

	glEnableVertexAttribArray	( 0 ); // from the shader: layout ( location = 0 ) position
	glEnableVertexAttribArray	( 1 ); // from the shader: layout ( location = 1 ) colors
	//glEnableVertexAttribArray	( 2 ); // from the shader: layout ( location = 2 ) uvs

	glVertexAttribFormatNV		( 0, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ) ); // specifying vertex positions format
	glVertexAttribFormatNV		( 1, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ) ); // specifying vertex colors format
	//glVertexAttribFormatNV		( 2, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ) ); // specifying uv's format


	glBufferAddressRangeNV		( GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV, 0, vboAddress, sizeof ( Vertex ) * size );
	glBufferAddressRangeNV		( GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV, 1, vboAddress + (GLsizei)COLOR_OFFSET, sizeof ( Vertex ) * size - (GLsizei)COLOR_OFFSET   );
	//glBufferAddressRangeNV		( GL_ELEMENT_ARRAY_ADDRESS_NV, 0, addr, length);
	//glBufferAddressRangeNV		( GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV, 2, vboAddres + (GLsizei)TEXTURE_OFFSET, sizeof ( Vertex ) * size - (GLsizei)TEXTURE_OFFSET  );

	// Set up the pointer in GPU memory to the index buffer
	glBufferAddressRangeNV		( GL_ELEMENT_ARRAY_ADDRESS_NV, 0, indexVboAddress, sizeof ( unsigned int ) * indices.size());

	//for (unsigned int i=0; i<numLines; i++)
	{
		//glDrawArrays				( GL_LINE_STRIP, i*numNodes, numNodes - 1 );
		//glDrawArrays				( GL_LINES, 0, size );

		glDrawElements				( GL_LINE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);
		//glDrawElementsBaseVertex  ( GL_LINE_STRIP, 77, GL_UNSIGNED_INT, &indices[0], 0);
		//glDrawElementsBaseVertex  ( GL_LINE_STRIP, 77, GL_UNSIGNED_INT, &indices[0], numNodes);
		//cout << numLines << endl;
	}

	glDisableVertexAttribArray	( 0 );
	glDisableVertexAttribArray	( 1 );
	//glDisableVertexAttribArray	( 2 );


    glDisableClientState(GL_ELEMENT_ARRAY_UNIFIED_NV);

}


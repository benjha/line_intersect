//
//										Benjamin Hernandez, PhD. benjamin.hernandez@bsc.es
//																	   All Rights Reserved
// 														   Barcelona Supercomputing Center
//
//															   See ReadMe.txt for details
//
//=======================================================================================
#ifndef __LINES_H__
#define __LINES_H__

#include <string>
#include <vector>

#include "Vertex.h"

using namespace std;

class cLines
{
public:
							cLines					( string &Filename, float  	scale );
							~cLines					( void );
	void					fillVBO					(	);
	void					draw					( 	);


	unsigned int			getVBO					( 	) { return vbo; }
	unsigned long long		getVBOAddres			( 	) { return vboAddress; }


private:

	void					genPoints				( unsigned int numLines, unsigned int numNodes );
	void					parse 					(string &filename);

	unsigned int			vbo;
	unsigned int 			indexVbo;
	unsigned int			size;

	unsigned int			numLines;
	unsigned int			numNodes;

	GLuint64EXT				vboAddress;
	GLuint64EXT				indexVboAddress;

	vector<Vertex>			vertices;
	vector<unsigned int>	indices;



};


#endif

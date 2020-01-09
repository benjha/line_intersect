
#ifndef __QUAD_H__
#define __QUAD_H__

#include <GL/glew.h>


/*
 * cQuad. A class to render a quad of size w, h with
 * texture coordinates tw, th it is used for render to
 * texture display contents.
 *
 * It is used to render vehicles as textured point sprites.
 *
 * It uses bindless buffer rendering
 *
 */

class cQuad
{
public:
						cQuad					( 	);
						~cQuad					(	);
		void			init					( unsigned int x0, unsigned int y0,unsigned int x1, unsigned int y1, unsigned int tw, unsigned int th );
		void 			draw					( 	);
		void			drawInstanced			( unsigned int numInstances, unsigned int target, unsigned int posID );
private:
		unsigned int			vbo;
		unsigned int			size;

		GLuint64EXT				vboAddres;

};

#endif

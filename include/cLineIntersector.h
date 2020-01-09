/*
 * cLineIntersector.h
 *
 *  Created on: Nov 13, 2015
 *      Author: benjha
 */

#ifndef __LINEINTERSECTOR_H__
#define __LINEINTERSECTOR_H__

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "cOpenGL_BaseApp.h"
#include "cCamera.h"

// Data structure to download the linked list to RAM
struct sNodeType
{
	float 			color[4];
	float 			depth;
	unsigned int 	nextIndex;
	float padding1;
	float padding2;
};

class FramebufferObject;
class cQuad;
class cLines;

class cLineIntersector : public cOpenGL_GLUT_BaseApp
{
public:
	//constructor
						cLineIntersector 				( 	unsigned int width, unsigned int height,
															std::string filename	);
	// Destructor
						~cLineIntersector 				(	);
	int					start		 					( 	int argc, char** argv,
															unsigned int mayorGLversion, unsigned int minorGLversion,
															bool stereo = false		);
	void				stop							(	);
	void				display 						(	);
	void 				idle							(	);
	void 				keys 							( unsigned char key, int x, int y );

private:
	//Projection types enum:
	enum { ORTHO_2D, ORTHO_3D, PERSPECTIVE };

	// Parses road link data
	void 				parse							(string filename);
	// Initializes OpenGL state, shaders, etc.
	void				init							(	);
	// Initializes member variables.
	void				initMembers						(	);
	// draws a Quad
	void				drawQuad						(	);
	// Sets the camera projection
	void				setProjection					( unsigned int projType, float left, float right, float bottom, float top, float nearPlane, float farPlane	);
	// Prints Linked list contents
	void 				printLinkedList					(	);
	// Renders intersection points
	void 				displayIntersections			(	);

	Camera				*camera;

	// Stores and renders Road link data
	cLines				*roads;
	string				roadsFilename;

	// Frame buffer object used for map rendering
	FramebufferObject 	*fbo;
	// FBO color and depth
	unsigned int 		fboColorTexId, fboDepthTexId;
	// Draws a screen size quad to display the fbo contents
	cQuad				*quad;
	// tranformation matrices: [0] - Model; [1] - View; [2] - Projection
	glm::mat4			transformMat[3];
	// Uniform Buffer Object ID that stores the Modelview Pojection Matrix
	unsigned int		MVPmatUBO;
	// Render to texture shader
	unsigned int		rttShader;

	// Linked list shader
	unsigned int		llShader;
	// atomic counter to gather a free node slot concurrently.
	unsigned int		freeNodeIndex;
	// Head index texture/image, which contains the list
	unsigned int		headIndexTexture;
	// Buffer to clear/reset the head pointers.
	unsigned int		clearBuffer;
	// Buffer for the linked list.
	unsigned int		linkedListBuffer;
	// Buffer for intersection points
	unsigned int		intersectionsVBO;
	// Shader to draw Intersection points
	unsigned int		interShader;

	// Variables to download linked list to RAM
	sNodeType			*linkedList;
	unsigned int 		*headIndexes;
};


#endif /* CLINEINTERSECTOR_H_ */

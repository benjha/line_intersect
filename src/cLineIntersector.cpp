/*
 * cLineIntersector.cpp
 *
 *  Created on: Nov 13, 2015
 *      Author: benjha
 */

#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "cLineIntersector.h"
#include "cLines.h"
#include "cMacros.h"
#include "cQuad.h"
#include "framebufferObject.h"
#include "LoadShaders.h"
#include "cModel.h"


#define MAX_NODES 					4*WINDOW_WIDTH*WINDOW_HEIGHT // Maximum number of nodes in the linked list. Shared for all fragments.
#define BINDING_ATOMIC_FREE_INDEX 	1 // the number comes from the shader
#define BINDING_IMAGE_HEAD_INDEX 	1
#define BINDING_BUFFER_LINKED_LIST 	1

// Variables to download linked list to RAM
//sNodeType 		nodes[WINDOW_WIDTH*WINDOW_HEIGHT];
//unsigned int 	headIndexes[WINDOW_WIDTH*WINDOW_HEIGHT];

extern cLineIntersector lineIntersector;

// Static GLUT's call-backs

void staticDraw ()
{
	lineIntersector.display ();
}

void staticIdle ()
{
	lineIntersector.idle ();
}

void staticKeys (unsigned char key, int x, int y)
{
	lineIntersector.keys (key, x, y);
}

//=======================================================================================


cLineIntersector::cLineIntersector (unsigned int winWidth, unsigned int winHeight, std::string roadsFilename )
						: cOpenGL_GLUT_BaseApp ( winWidth, winHeight )
{
	this->roadsFilename = roadsFilename;
	initMembers ();
}

//=======================================================================================



//=======================================================================================

cLineIntersector::~cLineIntersector ( )
{
	FREE_MEMORY		( fbo	);
	FREE_MEMORY		( model	);
	FREE_MEMORY		( quad	);
	FREE_MEMORY		( roads );
	FREE_ARRAY		( linkedList	);
	FREE_ARRAY		( headIndexes 	);

	FREE_TEXTURE	( headIndexTexture 	);
	FREE_GLBO		( freeNodeIndex 	);
	FREE_GLBO		( clearBuffer		);
	FREE_GLBO		( linkedListBuffer	);
	FREE_GLBO		( intersectionsVBO	);
}

//=======================================================================================

int cLineIntersector::start	( 	int argc, char** argv,
								unsigned int mayorGLversion, unsigned int minorGLversion, bool stereo )
{
	initGLUTAndExtensions( argc, argv, mayorGLversion, minorGLversion );

	registerGLUTCallbacks( staticDraw, staticIdle, staticKeys, 0, 0, 0);

	init ();

	glutMainLoop();

	return STARTED;
}

//=======================================================================================

void cLineIntersector::stop	(	)
{
// Something needs to happen here. Free variables ?
}

//=======================================================================================

void cLineIntersector::display	(	)
{
	unsigned int zero = 0;

	glClear			( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glClearColor	(0.95, 0.95, 0.95, 1.0);
	//glClearColor (150/255.0, 75/255.0, 0.0, 1.0);

	//setProjection ( ORTHO_3D, -50, 50, 10, 40, 1, 100);
	camera->setView ( MVPmatUBO,  transformMat, width, height);


	fbo->Bind();
		glDrawBuffer (GL_COLOR_ATTACHMENT0_EXT);
		glClear			( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glUseProgram (llShader);
			glBindBufferBase	( GL_UNIFORM_BUFFER, 0, MVPmatUBO 	); // passing Modelview Projection matrix
			glBindBufferBase	( GL_SHADER_STORAGE_BUFFER, BINDING_BUFFER_LINKED_LIST, linkedListBuffer	);
			glBindImageTexture	( BINDING_IMAGE_HEAD_INDEX, headIndexTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI	);

			glBindBufferBase	( GL_ATOMIC_COUNTER_BUFFER, BINDING_ATOMIC_FREE_INDEX, freeNodeIndex	);
			// Reset the atomic counter.
			glBufferSubData		( GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(unsigned int), &zero	);

			// Reset the head pointers by copying the clear buffer into the texture.
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clearBuffer);
	    		glBindTexture(GL_TEXTURE_2D, headIndexTexture);
	    			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED_INTEGER, GL_UNSIGNED_INT, 0);
	    		glBindTexture(GL_TEXTURE_2D, 0);
	    	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	    	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
				//model->draw();
	    		roads->draw();
		    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

		   glClear			(  GL_DEPTH_BUFFER_BIT );
		   //displayIntersections ();
	fbo->Disable();



	setProjection ( ORTHO_3D, 0, width*1.0f, 0.0, height*1.0f, -1.0, 200.0);
	glUseProgram	( rttShader);
		glBindBufferBase		( GL_UNIFORM_BUFFER, 0, MVPmatUBO 	); // passing Modelview Projection matrix
		//glBindImageTexture	( BINDING_IMAGE_HEAD_INDEX, headIndexTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI	);
		glBindTexture( GL_TEXTURE_2D, fboColorTexId);
			quad->draw ();
		glBindTexture( GL_TEXTURE_2D, 0);

	glUseProgram	(0);

//	camera->setView ( MVPmatUBO,  transformMat, width, height);
	//displayIntersections ();


	//printLinkedList ();


	glutSwapBuffers ();
}

//=======================================================================================

void cLineIntersector::displayIntersections (	)
{
	//setProjection ( ORTHO_3D, -8, 8, 64, 128, -50, 50);

	glUseProgram (interShader);
		glBindBufferBase	( GL_UNIFORM_BUFFER, 0, MVPmatUBO 	); // passing Modelview Projection matrix
		glBindBufferBase	( GL_SHADER_STORAGE_BUFFER, BINDING_BUFFER_LINKED_LIST, linkedListBuffer	);
		glBindImageTexture	( BINDING_IMAGE_HEAD_INDEX, headIndexTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI	);

			glDrawArrays (GL_POINTS, 0, width * height);

		glDisableVertexAttribArray (0);

	glUseProgram (0);

}

//=======================================================================================

void cLineIntersector::printLinkedList (	)
{
	unsigned int i, j;
	unsigned int index;
	//head index is the index of the last item in the list
	glBindTexture(GL_TEXTURE_2D, headIndexTexture);
		glGetTexImage (GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, headIndexes);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindBuffer	( GL_SHADER_STORAGE_BUFFER, linkedListBuffer);
	void *p = glMapBuffer( GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY );
	memcpy(linkedList, p, sizeof(sNodeType)*width*height);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	std::cout << std::setprecision(12);

	for (i=0;i<width*height/5;i++)
	{
		index= headIndexes[i];
		//std::cout << "Index: " << index << std::endl;

		while (index != 0xffffffff && j<8 )
		{
			//if (nodes[index].nextIndex != 0xffffffff )
				std::cout << " x: " << linkedList[index].color[0] <<
							 " y: " << linkedList[index].color[1] <<
							 " z: " << linkedList[index].color[2] <<
							 " next index: " << linkedList[index].nextIndex <<
						std::endl;
			j++;
			index = linkedList[index].nextIndex;
		}

		j = 0;
	}
	std::cout << std::endl << std::endl;
}

//=======================================================================================

void cLineIntersector::idle	(	)
{
	getFPS();
/*
	for (i = 0; i<NUM_TILES;i++)
	{
		if (mapRndrThrd[i].ready)
		{
			glTextureImage2DEXT 	(fboColorTexId[i], GL_TEXTURE_2D, 0, GL_RGBA, tileW, tileH, 0, GL_RGBA, GL_UNSIGNED_BYTE, map[i]->getPixels());
			mapRndrThrd[i].ready = false;
		}
	}
*/
	glutPostRedisplay();
}

//=======================================================================================

void cLineIntersector::initMembers ()
{
	fbo 				= 0;
	quad 				= 0;
	MVPmatUBO 			= 0;
	llShader			= 0;
	rttShader			= 0;
	fboColorTexId		= 0;
	fboDepthTexId		= 0;
	model				= 0;
	freeNodeIndex		= 0;
	headIndexTexture	= 0;
	clearBuffer			= 0;
	linkedListBuffer	= 0;
	linkedList			= 0;
	headIndexes			= 0;
	intersectionsVBO	= 0;
	interShader			= 0;
	roads				= 0;

}

//=======================================================================================

void cLineIntersector::init  ()
{
	unsigned int i;
	glClearColor	(1.0,1.0,1.0,1.0);
	glEnable		( GL_CULL_FACE 			);
	glEnable		( GL_DEPTH_TEST 		);
	glViewport		( 0, 0, width , height 	);

	glEnable 	(GL_LINE_SMOOTH);
	glLineWidth (2.0);

	glEnable (GL_POINT_SMOOTH);
	glPointSize (4.);

	// Creating the Uniform Buffer Object for Transformation Matrices
	glGenBuffers				( 1, &MVPmatUBO );
	glNamedBufferDataEXT		( MVPmatUBO, sizeof(glm::mat4) * 3, 0, GL_DYNAMIC_DRAW );

	camera = new Camera (0, Camera::FREE, Frustum::RADAR );

	vec3 pos (-84*10,36*10,0.2);
	//vec3 pos (0,36,30);
	vec3 dir (0.0, 0.0, -1.0 );
	vec3 up	 ( 0.0f, 1.0f, 0.0f );
	vec3 piv = pos + dir;

	camera->setPosition( pos );
	camera->setDirection( dir );
	camera->setUpVec ( up );
	camera->setPivot(piv);
	camera->getFrustum()->setFovY(45);
	camera->getFrustum()->setNearD(0.0001);
	camera->getFrustum()->setFarD(10);

	quad = new cQuad ();
	quad->init(0,0, width,height,1,1);

	// TODO: load linked list shaders
	ShaderInfo		rttCode[] = {
			{ GL_VERTEX_SHADER,		"shaders/RTT.vert" },
			{ GL_FRAGMENT_SHADER,	"shaders/RTT.frag" },
			{ GL_NONE,				NULL }
		};
	rttShader	= LoadShaders( rttCode );

	ShaderInfo		llCode[] = {
				{ GL_VERTEX_SHADER,		"shaders/linkedLink.vert" },
				{ GL_FRAGMENT_SHADER,	"shaders/linkedLink.frag" },
				{ GL_NONE,				NULL }
			};
	llShader = LoadShaders( llCode );

	ShaderInfo 		interCode[] = {
				{ GL_VERTEX_SHADER,		"shaders/intersectionPoints.vert"},
				{ GL_FRAGMENT_SHADER,	"shaders/intersectionPoints.frag"},
				{ GL_NONE,				NULL }
			};
	interShader = LoadShaders ( interCode );

	glUseProgram ( llShader );
		unsigned int maxNodesLocation = glGetUniformLocation(llShader, "u_maxNodes");
	    glUniform1ui ( maxNodesLocation, MAX_NODES );
	glUseProgram ( 0 );

	glUseProgram (interShader);
		unsigned int win_width			= glGetUniformLocation (interShader, "WINDOW_WIDTH");
		unsigned int win_height			= glGetUniformLocation (interShader, "WINDOW_HEIGHT");
		glUniform1i ( win_width,	WINDOW_WIDTH);
		glUniform1i ( win_height, 	WINDOW_HEIGHT);
	glUseProgram (0);

	// Atomic counter to gather a free node slot concurrently.
	glGenBuffers		( 1, &freeNodeIndex	);
	glNamedBufferDataEXT( freeNodeIndex, sizeof(unsigned int), 0, GL_DYNAMIC_DRAW			);

	// Head index texture/image, which contains the list
	glGenTextures		( 1, &headIndexTexture			);
	glBindTexture		( GL_TEXTURE_2D, headIndexTexture	);
		glTexImage2D		( GL_TEXTURE_2D, 0, GL_R32UI, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, 0);
		glTexParameteri		( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri		( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf		( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf		( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture		( GL_TEXTURE_2D, 0);

	//glBindImageTexture(BINDING_IMAGE_HEAD_INDEX, headIndexTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

	 // Buffer for cleaning the head index texture.
	unsigned int clearData[width * height];
	for (i = 0; i < width * height; i++)
	{
		// 0xffffffff means end of list, so for the start there is no entry.
	    clearData[i] = 0xffffffff;
	}

	// Buffer to clear/reset the head pointers.
	glGenBuffers	( 1, &clearBuffer	);
	glBindBuffer	( GL_PIXEL_UNPACK_BUFFER, clearBuffer	);
		glBufferData	( GL_PIXEL_UNPACK_BUFFER, sizeof(unsigned int) * width * height, clearData, GL_STATIC_COPY);
	glBindBuffer	( GL_PIXEL_UNPACK_BUFFER, 0	);

	// Buffer for the linked list.
	glGenBuffers		( 1, &linkedListBuffer);
	// Size is RGBA, depth (5 * GLfloat), next pointer (1 * GLuint) and 2 paddings (2 * GLfloat).
	glNamedBufferDataEXT( linkedListBuffer, MAX_NODES * (sizeof(float) * 5 + sizeof(unsigned int) * 1) + sizeof(float) * 2, 0, GL_DYNAMIC_DRAW			);

	//glBufferData		( GL_SHADER_STORAGE_BUFFER, MAX_NODES * (sizeof(GLfloat) * 5 + sizeof(GLuint) * 1) + sizeof(GLfloat) * 2, 0, GL_DYNAMIC_DRAW);
	//glBindBufferBase	( GL_SHADER_STORAGE_BUFFER, BINDING_BUFFER_LINKED_LIST, 0);
	headIndexes = new unsigned int[width*height];
	linkedList 	= new sNodeType[width*height];

	float *points = new float[4*width*height];
	for (i=0;i<width*height*4;i+=4)
	{
		points[i  ]	= i*1.0f;
		points[i+1] = i*1.0f;
		points[i+2] = 0.0f;
		points[i+4] = 0.0f;
	}
	glGenBuffers 	(1, &intersectionsVBO);
	glNamedBufferDataEXT (intersectionsVBO, 4*width*height*sizeof(float), points, GL_STATIC_DRAW );

	FREE_ARRAY (points);

	fbo = new FramebufferObject ();
	glGenTextures (1, &fboColorTexId);

	glTextureParameteriEXT	( fboColorTexId, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR 				);
	glTextureParameteriEXT	( fboColorTexId, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR 				);
	glTextureParameteriEXT	( fboColorTexId, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT   	);
	glTextureParameteriEXT	( fboColorTexId, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT		);
	glTextureImage2DEXT		( fboColorTexId, GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0 );


	glGenTextures (1, &fboDepthTexId);
//		glTexParameteri	( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE          );
	glTextureParameteriEXT	( fboDepthTexId, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR 	);
	glTextureParameteriEXT	( fboDepthTexId, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR	);
	glTextureParameteriEXT	( fboDepthTexId, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT	);
	glTextureParameteriEXT	( fboDepthTexId, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT	);
	glTextureImage2DEXT		( fboDepthTexId, GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F_NV, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL	);

	fbo->Bind();
			fbo->AttachTexture(GL_TEXTURE_2D, fboColorTexId, GL_COLOR_ATTACHMENT0, 0, 0);
			fbo->AttachTexture(GL_TEXTURE_2D, fboDepthTexId, GL_DEPTH_ATTACHMENT, 0, 0);
			if (fbo->IsValid())
			{
				cout << "FBO is complete.\n";
			}
			else
			{
				cout << "Check FBO configuration, rendering might not work. \n";
			}
	fbo->Disable();
	std::string filename = "data/woman.obj";
	model = new cModel (filename, 1.0f);

	roads = new cLines (roadsFilename, 1.0f);

}

//=======================================================================================

void cLineIntersector::setProjection	( unsigned int projType, float left, float right, float bottom, float top, float nearPlane, float farPlane	)
{
	// Setting model matrix to identity matrix
	transformMat[0]	= glm::mat4 ( 1.0f );
	// Setting viewing matrix to identity matrix.
	// This will change when adding keyboard / mouse interaction to move the camera
	transformMat[1] = glm::mat4 ( 1.0f );

	if ( projType == ORTHO_2D)
	{
#ifdef	OMPI_MPI_H
		transformMat[2] = glm::ortho ( 0.0f, width*(mpiComm.getNP()-1)*1.0f, 0.0f, height*1.0f, -1.0f, 1.0f );
#else
		transformMat[2] = glm::ortho ( left, right, bottom, top, -1.0f, 1.0f );
#endif
	}
	else if ( projType == ORTHO_3D )
	{
		transformMat[2] = glm::ortho ( left, right, bottom, top,
											nearPlane, farPlane );
	}
	else if ( projType == PERSPECTIVE )
	{
		// fov set to 45 degrees
		transformMat[2] = glm::perspective 	( (float)M_PI*0.25f, width*1.0f / height*1.0f,
												nearPlane, farPlane );
	}

	// loading modelview projection matrix to the UBO
	glNamedBufferDataEXT	( MVPmatUBO,  sizeof(glm::mat4)*3, &transformMat[0], GL_DYNAMIC_DRAW);
}

//=======================================================================================


void cLineIntersector::keys (unsigned char key, int x, int y)
{
	float dist = 0.1;
	switch (key)
	{
		case 27: //ESC
			glutLeaveMainLoop();
			break;
		case '+':
			camera->moveForward(dist);
			break;
		case '-':
			camera->moveBackward(dist);
			break;
		case 'a': // pan left
			camera->moveLeft(dist);
			break;
		case 'd': // pan right
			camera->moveRight(dist);
			break;
		case 'w': // pan up
			camera->moveUp(dist);
			break;
		case 's': // pan down
			camera->moveDown(dist);
			break;

		default:
			break;
	}
	glutPostRedisplay();

}

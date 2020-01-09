/*
 * cSlaveOpenGL_GLX_BaseApp.cpp
 *
 *  Created on: Jun 4, 2015
 *      Author: benjha
 */

#include	<iostream>
#include 	<sstream>
#include 	<fstream>
#include	<GL/glew.h>
#include	<GL/freeglut.h>
#include 	<sys/time.h>
#include	"cMacros.h"
#include	"cOpenGL_BaseApp.h"

#ifdef OMPI_MPI_H

//=======================================================================================
/*
 *
 *  Class cMPIComm
 *
 */
//=======================================================================================

cMPIComm::cMPIComm()
{
	pid 		= -1;
	np			= -1;
	masterPID 	= -1;
	requests	= 0;
}

//=======================================================================================

cMPIComm::~cMPIComm ()
{

}

//=======================================================================================

void cMPIComm::setProccessInfo ( int pid, int np, int masterId, MPI_Request *requests )
{
	cMPIComm::pid 		= pid;
	cMPIComm::np		= np;
	cMPIComm::masterPID	= masterId;
	cMPIComm::requests  = requests;

}

//=======================================================================================

int cMPIComm::isend ( frameBufferType *buffer, unsigned int frameBufferSizeMsg, unsigned int to_nodeID, MPI_Comm comm)
{
	MPI_Isend ((frameBufferType*)buffer, frameBufferSizeMsg, MPI_BYTE, to_nodeID, 0, comm,  &requests[masterPID]); // master pID = 0

	return 1;
}

//=======================================================================================

int cMPIComm::send	( unsigned int to_nodeID, MPI_Comm comm  	)
{
	return 1;
}

//=======================================================================================

int cMPIComm::irecv ( frameBufferType *buffer, unsigned int frameBufferSizeMsg, unsigned int from_nodeID, MPI_Comm comm )
{
	MPI_Irecv (buffer, frameBufferSizeMsg, MPI_BYTE, from_nodeID, 0, comm,  &requests[from_nodeID]);
	return 1;
}

//=======================================================================================

int	cMPIComm::recv ( unsigned int from_nodeID, MPI_Comm comm  	)
{
	return 1;
}

#endif

//=======================================================================================
/*
 *
 *
 *  Class cOpenGL_GLX_BaseApp
 *
 *
 */
//=======================================================================================
cOpenGL_GLX_BaseApp::cOpenGL_GLX_BaseApp ( )
											: width (1280), height (720), windowless (false)
{
	displayName = std::string ("0.0");
	initMembers ();
}

//=======================================================================================

cOpenGL_GLX_BaseApp::cOpenGL_GLX_BaseApp ( unsigned int width, unsigned int height )
											: windowless (false)
{
	cOpenGL_GLX_BaseApp::width			= width;
	cOpenGL_GLX_BaseApp::height 		= height;
	cOpenGL_GLX_BaseApp::displayName 	= std::string ("0.0");

	initMembers ();
}

//=======================================================================================

cOpenGL_GLX_BaseApp::cOpenGL_GLX_BaseApp ( unsigned int width, unsigned int height, std::string display, bool windowless )
{
	cOpenGL_GLX_BaseApp::width			= width;
	cOpenGL_GLX_BaseApp::height 		= height;
	cOpenGL_GLX_BaseApp::windowless 	= windowless;
	cOpenGL_GLX_BaseApp::displayName	= display;

	initMembers ();
}

//=======================================================================================

cOpenGL_GLX_BaseApp::~cOpenGL_GLX_BaseApp ()
{
	FREE_ARRAY ( depth );
	FREE_ARRAY ( color );
}

//=======================================================================================

void cOpenGL_GLX_BaseApp::initMembers ()
{
	color 		= 0;
	depth 		= 0;
	frames		= 0;
	timebase	= 0;
	fps			= 0.0;
	timeSet		= false;
	time_ 		= elapsedTime ();

	int wh 				= width * height;
	color		 		= new frameBufferType [ wh * 4 ];

	if (color == 0 )
		std::cout << "Error: Not enough memory. Color buffer not allocated. \n.";

	depth				= new frameBufferType [ wh ];
	if ( depth == 0 )
		std::cout << "Error: Not enough memory. Depth buffer not allocated. \n.";

	frameBufferSizeMsg 	= wh * 4 * sizeof (frameBufferType );


}

//=======================================================================================

int cOpenGL_GLX_BaseApp::initGLContexAndExtensions			( unsigned int mayorGLversion, unsigned int minorGLversion, bool stereo )
{
	rcx = new RenderContext;
	rcx->winWidth 	= width;
	rcx->winHeight	= height;
#ifndef OMPI_MPI_H
	rcx->winX		= 0;
	rcx->winY		= 0;
#else
	rcx->winX		= (mpiComm.getPID()%6)*(width+5);
	rcx->winY		= (mpiComm.getPID()/6)*(height+50);
#endif
	initEarlyGLXfnPointers();

	// TODO: What happens if numGPUs > 1
	if (windowless)
	{
		CreateWindowless	( rcx, displayName.data(), mayorGLversion, minorGLversion);
	}
	else
	{
		CreateWindow 		( rcx, displayName.data(), mayorGLversion, minorGLversion, stereo);
		setX11Events		( rcx );
	}

	// Loading OpenGL extensions

	int glew_status = glewInit();
	if( glew_status != GLEW_OK )
	{
		return GLEW_INIT_FAILED;
	}
	std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << endl;

	return STARTED;

}

//=======================================================================================
#ifdef OMPI_MPI_H
void cOpenGL_GLX_BaseApp::setMPInfo ( int pid, int np, int masterId, MPI_Request *requests )
{
	mpiComm.setProccessInfo(pid, np,masterId, requests );
}
#endif
//=======================================================================================

int cOpenGL_GLX_BaseApp::start (int argc, char** argv, unsigned int mayorGLversion, unsigned int minorGLversion)
{
#ifdef OMPI_MPI_H
	//TODO: check MPI info has been set
#endif

	initGLContexAndExtensions ( mayorGLversion, minorGLversion);

	init ();

	glLoop  ();


	return STARTED;
}

//=======================================================================================

void cOpenGL_GLX_BaseApp::stop ( )
{
#ifdef OMPI_MPI_H
	std::cout << "Shutting down node " << mpiComm.getPID() << std::endl;
#endif
	Cleanup (rcx);
}

//=======================================================================================

void cOpenGL_GLX_BaseApp::init ()
{
	glClearColor  		( 1.0,1.0,0.0,1.0		);
}

//=======================================================================================

void cOpenGL_GLX_BaseApp::glLoop ()
{
	while (1)
	{
		if (!windowless)
		{
			//bool bWinMapped = false;
			XEvent event;
			XNextEvent(rcx->dpy, &event);


			if (event.type == ClientMessage)
				break;
		}
		display ();
		idle	();
	}
}

//=======================================================================================

void cOpenGL_GLX_BaseApp::display ()
{
	glClear			( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	if (windowless)
		glXSwapBuffers(rcx->dpy, rcx->pbuffer);
	else
		glXSwapBuffers(rcx->dpy, rcx->win);

}

//=======================================================================================

void cOpenGL_GLX_BaseApp::idle								(	)
{

}

//=======================================================================================

long cOpenGL_GLX_BaseApp::elapsedTime(	)
{
    if ( timeSet )
    {
        struct timeval now;
        long elapsed;

        gettimeofday( &now, 0 );

        elapsed = (now.tv_usec - time_0.tv_usec) / 1000;
        elapsed += (now.tv_sec - time_0.tv_sec) * 1000;

        return elapsed;
    }
    else
    {
        gettimeofday( &time_0, 0 );
        timeSet = true ;
        return 0 ;
    }
}

//=======================================================================================

bool cOpenGL_GLX_BaseApp::events ()
{
	XEvent event;
	while(XPending(rcx->dpy))
	{
		XNextEvent(rcx->dpy,&event);

		if (event.type == ClientMessage)
			return true;
	}

	return false;
}

//=======================================================================================

double cOpenGL_GLX_BaseApp::getFPS								(	)
{
	frames++;
	time_ = elapsedTime ();

	if (time_ - timebase > 1000)
	{
		fps	= frames*1000.0f/(time_-timebase);

#ifdef OMPI_MPI_H
		std::cout << "Process Id: " << mpiComm.getPID() << std::endl;
#endif
		std::cout << "fps: " << fps << " ms: " << 1.0 / fps * 1000.0 << " frames: " << frames << std::endl;



		timebase	= time_;
		frames		= 0;
	}

	return fps;
}

//=======================================================================================

void cOpenGL_GLX_BaseApp::writeFrameBuffer(int pid)
{
	unsigned int i;
	std::stringstream filename;
	filename << "Image_" << pid << ".ppm";

	FILE *fp = fopen(filename.str().data(), "wb");
	fprintf(fp, "P6\n%d %d\n255\n", width, height);

	for (i=0;i<width*height*4;i+=4)
	{
		static unsigned char pixel[3];
		pixel[0] = color[i];
		pixel[1] = color[i+1];  /* green */
		pixel[2] = color[i+2];  /* blue */
		fwrite(pixel, 1, 3, fp);
	}

	fclose(fp);
}
//=======================================================================================
/*
 *
 *
 *  Class cOpenGL_GLUT_BaseApp
 *
 *
 */
//=======================================================================================

cOpenGL_GLUT_BaseApp::cOpenGL_GLUT_BaseApp ( ) : width (1280), height (720)
{
	initMembers ();
}

//=======================================================================================

cOpenGL_GLUT_BaseApp::cOpenGL_GLUT_BaseApp (unsigned int width, unsigned int height )
{
	cOpenGL_GLUT_BaseApp::width			= width;
	cOpenGL_GLUT_BaseApp::height 		= height;

	initMembers ();
}

//=======================================================================================

cOpenGL_GLUT_BaseApp::~cOpenGL_GLUT_BaseApp	(	)
{
#ifdef OMPI_MPI_H
	FREE_ARRAY ( depth );
	FREE_ARRAY ( color );
#endif
}

//=======================================================================================

void cOpenGL_GLUT_BaseApp::initMembers	( 	)
{
	frames		= 0;
	timebase	= 0;
	fps			= 0.0;
	timeSet		= false;
	time_ 		= elapsedTime ();


#ifdef OMPI_MPI_H
	color 		= 0;
	depth 		= 0;
	int wh		= width * height;
	color		= new frameBufferType [ wh * 4 ];

	if (color == 0 )
		std::cout << "Error: Not enough memory. Color buffer not allocated. \n.";

	depth				= new frameBufferType [ wh ];
	if ( depth == 0 )
		std::cout << "Error: Not enough memory. Depth buffer not allocated. \n.";

	frameBufferSizeMsg 	= wh * 4 * sizeof (frameBufferType );
#endif
}

//=======================================================================================

int cOpenGL_GLUT_BaseApp::start ( int argc, char** argv, unsigned int mayorGLversion, unsigned int minorGLversion, bool stereo )
{
	initGLUTAndExtensions( argc, argv, mayorGLversion, minorGLversion );

	init ();

	glutMainLoop();

	return STARTED;
}

//=======================================================================================

void cOpenGL_GLUT_BaseApp::stop ( )
{

}

//=======================================================================================

int cOpenGL_GLUT_BaseApp::initGLUTAndExtensions ( int argc, char** argv, unsigned int mayorGLversion, unsigned int minorGLversion	)
{
	glutInit				( &argc, argv);
	glutInitDisplayMode		( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitContextVersion	( mayorGLversion, minorGLversion);
	glutInitContextFlags	( GLUT_FORWARD_COMPATIBLE );

	glutInitContextProfile	( GLUT_COMPATIBILITY_PROFILE );
	glutInitWindowSize		( width, height);
	glutInitWindowPosition	( 0, 0);
	glutCreateWindow		( "GLUT" );
	glutSetOption 			( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS );

	// Loading OpenGL extensions
	int glew_status = glewInit();
	if( glew_status != GLEW_OK )
	{
		return GLEW_INIT_FAILED;
	}
	std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << endl;

	return STARTED;
}

//=======================================================================================
void cOpenGL_GLUT_BaseApp::registerGLUTCallbacks ( 	void (* display)	( void ),
													void (* idle)		( void ),
													void (* keyboard)	( unsigned char, int, int ),
													void (* mouse)		( int, int, int, int ),
													void (* motion)		( int, int, int, int ),
													void (* freeMe)		( void )
												 )
{
	glutKeyboardFunc		( keyboard			);
	glutDisplayFunc			( display			);
	glutIdleFunc 			( idle				);
	glutCloseFunc			( freeMe			);

}

//=======================================================================================

long cOpenGL_GLUT_BaseApp::elapsedTime	(  	)
{
	 if ( timeSet )
	{
		struct timeval now;
		long elapsed;

		gettimeofday( &now, 0 );

		elapsed = (now.tv_usec - time_0.tv_usec) / 1000;
		elapsed += (now.tv_sec - time_0.tv_sec) * 1000;

		return elapsed;
	}
	else
	{
		gettimeofday( &time_0, 0 );
		timeSet = true ;
		return 0 ;
	}
}

//=======================================================================================

void cOpenGL_GLUT_BaseApp::init		(	)
{
	glClearColor  (1.0,0.0,0.0,1.0);
}

//=======================================================================================



//=======================================================================================

void cOpenGL_GLUT_BaseApp::display (	)
{
	glClear			( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glutSwapBuffers ();
}

//=======================================================================================

void cOpenGL_GLUT_BaseApp::keyboard ( unsigned char key, int x, int y )
{
	switch (key)
	{
		case 27:
			glutLeaveMainLoop();
			break;
		default:
			break;
	}
}

//=======================================================================================

void cOpenGL_GLUT_BaseApp::idle (	)
{

}

//=======================================================================================

bool cOpenGL_GLUT_BaseApp::events (	)
{
	return true;
}

//=======================================================================================

double cOpenGL_GLUT_BaseApp::getFPS	(	)
{
	frames++;
	time_ = elapsedTime ();

	if (time_ - timebase > 1000)
	{
		fps	= frames*1000.0f/(time_-timebase);

#ifdef OMPI_MPI_H
		std::cout << "Process Id: " << mpiComm.getPID() << std::endl;
#endif
		std::cout << "fps: " << fps << " ms: " << 1.0 / fps * 1000.0 <<  " frames: " << frames << std::endl;



		timebase	= time_;
		frames		= 0;
		time_ = 0;
	}

	return fps;
}

//=======================================================================================
#ifdef OMPI_MPI_H
void cOpenGL_GLUT_BaseApp::setMPInfo ( int pid, int np, int masterId, MPI_Request *requests )
{
	mpiComm.setProccessInfo(pid, np, masterId, requests);
}

//=======================================================================================

void cOpenGL_GLUT_BaseApp::writeFrameBuffer(int pid)
{
	unsigned int i;
	std::stringstream filename;
	filename << "Image_" << pid << ".ppm";

	FILE *fp = fopen(filename.str().data(), "wb");
	fprintf(fp, "P6\n%d %d\n255\n", width, height);

	for (i=0;i<width*height*4;i+=4)
	{
		static unsigned char pixel[3];
		pixel[0] = color[i];
		pixel[1] = color[i+1];  /* green */
		pixel[2] = color[i+2];  /* blue */
		fwrite(pixel, 1, 3, fp);
	}

	fclose(fp);
}

#endif

//=======================================================================================

void cOpenGL_GLUT_BaseApp::writeFrameBuffer(int pid, unsigned char *buffer)
{
	unsigned int i;
	std::stringstream filename;
	filename << "Image_" << pid << ".ppm";

	FILE *fp = fopen(filename.str().data(), "wb");
	fprintf(fp, "P6\n%d %d\n255\n", width, height);

	for (i=0;i<width*height*4;i+=4)
	{
		static unsigned char pixel[3];
		pixel[0] = buffer[i];
		pixel[1] = buffer[i+1];  /* green */
		pixel[2] = buffer[i+2];  /* blue */
		fwrite(pixel, 1, 3, fp);
	}

	fclose(fp);
}

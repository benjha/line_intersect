/*
 * cSlave.h
 *
 *  Created on: Jun 4, 2015
 *      Author: benjha
 */

#ifndef CSLAVEOPENGLGLXBASEAPP_H_
#define CSLAVEOPENGLGLXBASEAPP_H_

#include <string>
//#include <mpi.h>
#include "glx_x11_events.h"

typedef unsigned char	frameBufferType;

/*
 * cMPIComm. This class is used to send/recv framebuffers from different nodes
 *
 */

#ifdef OMPI_MPI_H

class cMPIComm
{
public:
			cMPIComm										(	);
			~cMPIComm										(	);
			// Sets process ID, number of processes and master process id
			void		setProccessInfo						( int pid, int np, int masterId, MPI_Request *requests			);
			// Gets process ID
			int			getPID								(	) { return pid; 		}
			// Gets master ID
			int			getMasterPID						(	) {	return masterPID;	}
			// Gets number of processes
			int			getNP								(	) { return np;			}
			// Gets i-request
			MPI_Request getRequest							(unsigned int i ) { return requests[i]; }
			// Sends asynchronously the frame buffer context to nodeID
			// must be used with asynchronous receive
			int			isend								( frameBufferType *buffer, unsigned int frameBufferSizeMsg, unsigned int to_nodeID, MPI_Comm comm );
			// Sends synchronously the frame buffer context to nodeID
			// must be used with synchronous receive
			int			send								( unsigned int to_nodeID, MPI_Comm comm  	);
			// Receives asynchronously the frame buffer context from nodeID
			// must be used with asynchronous receive
			int			irecv								( frameBufferType *buffer, unsigned int frameBufferSizeMsg, unsigned int from_nodeID, MPI_Comm comm );
			// Sends synchronously the frame buffer context to nodeID
			// must be used with synchronous receive
			int			recv								( unsigned int from_nodeID, MPI_Comm comm  	);
private:

			int 		pid;					// MPI process ID.
			int			np;						// Number of MPI Processes
			int			masterPID;				// Master process ID.
			MPI_Request *requests;				// requests for async. communications
};

#endif


/*
 * cOpenGL_GLX_BaseApp. This class encapsulate the basic functionality to create and
 * display an OpenGL context using GLX.
 *
 */
class cOpenGL_GLX_BaseApp
{
public:
		enum {MASTER_PID_UNDEFINED, GLEW_INIT_FAILED, STARTED};
		// Constructor
							cOpenGL_GLX_BaseApp 				(  );
							cOpenGL_GLX_BaseApp 				( unsigned int width, unsigned int height );
							cOpenGL_GLX_BaseApp 				( unsigned int width, unsigned int height, std::string display, bool windowless );
		// Destructor
		virtual				~cOpenGL_GLX_BaseApp 			(	);
		// Entry point function to launch the OpenGL renderer
		// calls functions to initialize GLEW, context and OpenGL initial state
		virtual int 		start		 						( 	int argc, char** argv,
																	unsigned int mayorGLversion, unsigned int minorGLversion				);
		// Stops the OpenGL renderer and context.
		virtual void		stop								( 	);


		// Creates and Initializes OpenGL context and loads OpenGL extensions
		int					initGLContexAndExtensions			( unsigned int mayorGLversion, unsigned int minorGLversion, bool stereo = false	);
		// Returns the elapsed time. It is used to measure
		// fps. This method is based on glut's get time
		long				elapsedTime							(  	);
		// OpenGL init function.
		virtual void		init								(	);
		// OpenGL main loop.
		virtual void		glLoop								(   );
		// OpenGL Display function.
		virtual void		display								(	);
		// OpenGL Idle function.
		virtual void		idle								(	);
		// Process GUI events
		bool 				events 								(	);
		// Returns and print fps and time per frame
		double				getFPS								(	);
#ifdef OMPI_MPI_H
		// Sets process ID, number of processes and master process id
		void				setMPInfo							( int pid, int np, int masterId, MPI_Request *requests	);
#endif
		//writes FrameBuffer contents to disk in PPM format
		void				writeFrameBuffer					( int pid	);

protected:
		// This is use to measure framerate.
		struct timeval		time_0;
		// Controls when time_0 has been set for the first time.
		bool				timeSet;
		// stores frame per second values
		double				fps;
		// timing variables
		unsigned long		frames, time_,timebase;
		// timing variables
		//unsigned long		frameCount;

		unsigned int		width;					// framebuffer width.
		unsigned int		height;					// framebuffer height.
		bool				windowless;				// defines if the application is windowless (glx) or windowed (glut).
		unsigned int		frameBufferSizeMsg;		// the size of the color and depth arrays to be  used in MPI send message

		std::string			displayName;				// Display name

		frameBufferType		*color;					// stores frame buffer color values.
		frameBufferType		*depth;					// stores frame buffer depth values.

		RenderContext 		*rcx;					// OpenGL Render Context Structure.
#ifdef OMPI_MPI_H
		cMPIComm			mpiComm;				// class that deals with Frambuffer Communications
#endif

private:
		// Initialize class member variables to default values.
		void				initMembers							( 	);



};


/*
 * cOpenGL_GLUT_BaseApp. This class encapsulate the basic functionality to create and
 * display an OpenGL context using GLUT. It also communicates the frame buffer information to its
 * master.
 */

class cOpenGL_GLUT_BaseApp
{
public:
		enum {MASTER_PID_UNDEFINED, GLEW_INIT_FAILED, STARTED};
		// Constructor
							cOpenGL_GLUT_BaseApp 				(  );
							cOpenGL_GLUT_BaseApp 				( unsigned int width, unsigned int height );

		// Destructor
		virtual				~cOpenGL_GLUT_BaseApp 				(	);
		// Entry point function to launch the OpenGL renderer
		// calls functions to initialize GLEW, context and OpenGL initial state
		virtual int 		start		 						( 	int argc, char** argv,
																	unsigned int mayorGLversion, unsigned int minorGLversion, bool stereo = false	);
		// Stops the OpenGL renderer and context.
		virtual void		stop								( 	);

		// Returns the elapsed time. It is used to measure
		// fps. This method is based on glut's get time
		long				elapsedTime							(  	);

		// Creates and Initializes OpenGL context and loads OpenGL extensions
		int					initGLUTAndExtensions				( 	int argc, char** argv,
																	unsigned int mayorGLversion, unsigned int minorGLversion	);

		void				registerGLUTCallbacks				( 	void (* display)	( void ),
																	void (* idle)		( void ),
																	void (* keyboard)	( unsigned char, int, int ),
																	void (* mouse)		( int, int, int, int ),
																	void (* motion)		( int, int, int, int ),
																	void (* freeMe)		( void )
																																);

		// OpenGL init function.
		virtual void		init								(	);
		// OpenGL Display function.
		static	void		display								(	);
		// OpenGL Idle function.
		static 	void		idle								(	);
		// Detects keyboard events
		static	void		keyboard							( unsigned char key, int x, int y );
		// Process GUI events
		bool 				events 								(	);
		// Returns and print fps and time per frame
		double				getFPS								(	);
#ifdef OMPI_MPI_H
		// Sets process ID, number of processes and master process id
		void				setMPInfo							( int pid, int np, int masterId, MPI_Request *requests );
#endif
		//writes FrameBuffer contents to disk in PPM format
		void				writeFrameBuffer					( int pid	);
		void 				writeFrameBuffer					(int pid, unsigned char *buffer);


protected:
		// This is use to measure framerate.
		struct timeval		time_0;
		// Controls when time_0 has been set for the first time.
		bool				timeSet;
		// stores frame per second values
		double				fps;
		// timing variables
		unsigned long		frames, time_,timebase;
		// timing variables
		//unsigned long		frameCount;


		unsigned int		width;					// framebuffer width.
		unsigned int		height;					// framebuffer height.
#ifdef OMPI_MPI_H
		unsigned int		frameBufferSizeMsg;		// the size of the color and depth arrays to be  used in MPI send message

		frameBufferType		*color;					// stores frame buffer color values.
		frameBufferType		*depth;					// stores frame buffer depth values.


		cMPIComm			mpiComm;				// class that deals with Frambuffer Communications
#endif



private:
		// Initialize class member variables to default values.
		void				initMembers							( 	);



};



#endif /* CSLAVE_H_ */

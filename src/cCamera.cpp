//
//										Benjamin Hernandez, PhD. benjamin.hernandez@bsc.es
//																	   All Rights Reserved
// 														   Barcelona Supercomputing Center
//
//															   See ReadMe.txt for details
//
//=======================================================================================

#include <GL/glew.h>
#include "cCamera.h"

//=======================================================================================
//
Camera::Camera( unsigned int id, int type, int frustum_type )
{
	Camera::id		= id;
	Camera::type	= type;
	hasModel		= false;
	hasAudio		= false;
	active			= false;
	Camera::type	= type;
	fR				= (float)(rand() % 256) / 255.0f;
	fG				= (float)(rand() % 256) / 255.0f;
	fB				= (float)(rand() % 256) / 255.0f;
	frustum			= new Frustum( frustum_type, 45.0f, 0.0001f, 10000.0f );
	eye_separation	= 5.0f;
	focallength		= 1.0f;

	position        = vec3( 0.0f, 0.0f, 0.0f );
	direction       = vec3( 0.0f, 0.0f, 1.0f );
	up              = vec3( 0.0f, 1.0f, 0.0f );
	pivot           = vec3( 0.0f, 0.0f, 0.0f );

	fLeft			= 0.0f;
	fRight			= 0.0f;
	bottom			= 0.0f;
	top				= 0.0f;
	wd2				= 0.0f;
	ndfl			= 0.0f;
}
//
//=======================================================================================
//
Camera::~Camera( void )
{
	FREE_MEMORY( frustum );
}
//
//=======================================================================================
//
void Camera::setPosition( vec3& pos )
{
	position = pos;
}
//
//=======================================================================================
//
void Camera::setDirection( vec3& dir )
{
	direction 	= normalize( dir );
	right    	= cross( direction, up );
}
//
//=======================================================================================
//
vec3& Camera::getPosition( void )
{
	return position;
}
//
//=======================================================================================
//
vec3& Camera::getUp( void )
{
	return up;
}
//
//=======================================================================================
//
vec3& Camera::getDirection( void )
{
	return direction;
}
//
//=======================================================================================
//
void Camera::setUpVec( vec3& up )
{
	Camera::up 	= normalize( up );
	right    	= cross( direction, Camera::up );
}
//
//=======================================================================================
//
void Camera::setPivot( vec3& pivot )
{
	Camera::pivot 	= pivot;
	vec3 dir = pivot - position;
	if( length( dir ) > 0 )
	direction		= normalize( pivot - position );
	right    		= cross( direction, up );
}
//
//=======================================================================================
//
void Camera::moveForward( float dist )
{
	position = position + (dist * direction);
}
//
//=======================================================================================
//
void Camera::moveBackward( float dist )
{
	position = position - (dist * direction);
}
//
//=======================================================================================
//
void Camera::moveRight( float dist )
{
	position = position + (dist * right);
}
//
//=======================================================================================
//
void Camera::moveLeft( float dist )
{
	position = position - ( dist * right );
}
//
//=======================================================================================
//
void Camera::moveUp( float dist )
{
	position = position + ( dist * up );
}
//
//=======================================================================================
//
void Camera::moveDown( float dist )
{
	position = position - ( dist * up );
}
//
//=======================================================================================
//
void Camera::move( float dist, float deg_angle )
{
	vec3 right, moveDir;
	float cosAng, sinAng, radAng;

	radAng   = (float)(deg_angle * DEG2RAD);
	cosAng   = cos( radAng );
	sinAng   = sin( radAng );
	right    = cross( direction, up );
	moveDir  = (cosAng * direction) - (sinAng * right);
	position = position + (dist * moveDir);
}
//
//=======================================================================================
//
void Camera::rotateAngle( float deg_angle, vec3& axis )
{
    quat q;
	vec3 newAxis;
#ifdef CAMERA_ROTATE_AXIS
	axis = normalize( axis );
	newAxis.x = axis.x * right.x + axis.y * up.x + axis.z * direction.x;
	newAxis.y = axis.x * right.y + axis.y * up.y + axis.z * direction.y;
	newAxis.z = axis.x * right.z + axis.y * up.z + axis.z * direction.z;
#else
	newAxis = normalize( axis );
#endif
    q = glm::angleAxis( deg_angle, newAxis );
    direction = glm::rotate( q, direction );
    up = glm::rotate( q, up );
}
//
//=======================================================================================
//
void Camera::moveAround( float deg_angle, vec3& axis )
{
	quat q;
	vec3 negPos;
	vec3 newAxis;
#ifdef CAMERA_ROTATE_AXIS
	normalize( axis );
	newAxis.x = axis.x * right.x + axis.y * up.x + axis.z * direction.x;
	newAxis.y = axis.x * right.y + axis.y * up.y + axis.z * direction.y;
	newAxis.z = axis.x * right.z + axis.y * up.z + axis.z * direction.z;
#else
	newAxis = normalize( axis );
#endif
	negPos		= position - pivot;
	negPos      = normalize( negPos );

    q           = glm::angleAxis( deg_angle, newAxis );
    position    = glm::rotate( q, negPos );
    direction   = glm::rotate( q, direction );
    up          = glm::rotate( q, up );

}
//
//=======================================================================================
//
void Camera::getWorldFrustumCoords( float* coords )
{
	float	 deltaNear[3][3];
	float	 deltaFar[3][3];
	float	 dx;
	float	 dy;
	vec3 vecX		= frustum->getX();
	dx				= frustum->getNearW();
	dy				= frustum->getNearH();
	deltaNear[0][0] = vecX.x		* dx;
	deltaNear[0][1] = vecX.y		* dx;
	deltaNear[0][2] = vecX.z		* dx;
	deltaNear[1][0] = up.x			* dy;
	deltaNear[1][1] = up.y			* dy;
	deltaNear[1][2] = up.z			* dy;
	deltaNear[2][0] = direction.x	* frustum->getNearD();
	deltaNear[2][1] = direction.y	* frustum->getNearD();
	deltaNear[2][2] = direction.z	* frustum->getNearD();

	dx				= frustum->getFarW();
	dy				= frustum->getFarH();
	deltaFar[0][0]	= vecX.x		* dx;
	deltaFar[0][1]	= vecX.y		* dx;
	deltaFar[0][2]	= vecX.z		* dx;
	deltaFar[1][0]	= up.x			* dy;
	deltaFar[1][1]	= up.y			* dy;
	deltaFar[1][2]	= up.z			* dy;
	deltaFar[2][0]	= direction.x	* frustum->getFarD();
	deltaFar[2][1]	= direction.y	* frustum->getFarD();
	deltaFar[2][2]	= direction.z	* frustum->getFarD();

	coords[0]		= position.x - deltaNear[0][0] - deltaNear[1][0] + deltaNear[2][0];
	coords[1]		= position.y - deltaNear[0][1] - deltaNear[1][1] + deltaNear[2][1];
	coords[2]		= position.z - deltaNear[0][2] - deltaNear[1][2] + deltaNear[2][2];
	coords[3]		= position.x + deltaNear[0][0] - deltaNear[1][0] + deltaNear[2][0];
	coords[4]		= position.y + deltaNear[0][1] - deltaNear[1][1] + deltaNear[2][1];
	coords[5]		= position.z + deltaNear[0][2] - deltaNear[1][2] + deltaNear[2][2];
	coords[6]		= position.x - deltaNear[0][0] + deltaNear[1][0] + deltaNear[2][0];
	coords[7]		= position.y - deltaNear[0][1] + deltaNear[1][1] + deltaNear[2][1];
	coords[8]		= position.z - deltaNear[0][2] + deltaNear[1][2] + deltaNear[2][2];
	coords[9]		= position.x + deltaNear[0][0] + deltaNear[1][0] + deltaNear[2][0];
	coords[10]		= position.y + deltaNear[0][1] + deltaNear[1][1] + deltaNear[2][1];
	coords[11]		= position.z + deltaNear[0][2] + deltaNear[1][2] + deltaNear[2][2];

	coords[12]		= position.x - deltaFar[0][0]  - deltaFar[1][0]  + deltaFar[2][0];
	coords[13]		= position.y - deltaFar[0][1]  - deltaFar[1][1]  + deltaFar[2][1];
	coords[14]		= position.z - deltaFar[0][2]  - deltaFar[1][2]  + deltaFar[2][2];
	coords[15]		= position.x + deltaFar[0][0]  - deltaFar[1][0]  + deltaFar[2][0];
	coords[16]		= position.y + deltaFar[0][1]  - deltaFar[1][1]  + deltaFar[2][1];
	coords[17]		= position.z + deltaFar[0][2]  - deltaFar[1][2]  + deltaFar[2][2];
	coords[18]		= position.x - deltaFar[0][0]  + deltaFar[1][0]  + deltaFar[2][0];
	coords[19]		= position.y - deltaFar[0][1]  + deltaFar[1][1]  + deltaFar[2][1];
	coords[20]		= position.z - deltaFar[0][2]  + deltaFar[1][2]  + deltaFar[2][2];
	coords[21]		= position.x + deltaFar[0][0]  + deltaFar[1][0]  + deltaFar[2][0];
	coords[22]		= position.y + deltaFar[0][1]  + deltaFar[1][1]  + deltaFar[2][1];
	coords[23]		= position.z + deltaFar[0][2]  + deltaFar[1][2]  + deltaFar[2][2];
}
//
//=======================================================================================
//
void Camera::draw( void )
{
	frustum->drawLines();
	frustum->drawNormals();
}
//
//=======================================================================================
//
void Camera::setView( void )
{
	active = true;
	lookat = position + direction;
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	frustum->updateRatio();
	gluPerspective( frustum->getFovY(),
					frustum->getRatio(),
					frustum->getNearD(),
					frustum->getFarD()	);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt(
		position.x, position.y, position.z,
		lookat.x,   lookat.y,   lookat.z,
		up.x,       up.y,       up.z
	);
	frustum->setPlanes( position, lookat, up );
}
//
//=======================================================================================
//
void Camera::setView (unsigned int MVPBufferId, glm::mat4* transformMat, unsigned int viewPortWidth, unsigned int viewPortHeight )
{
	transformMat[0]	= glm::mat4			( 1.0f );
	transformMat[1]	= glm::mat4			( 1.0f );
	transformMat[2]	= glm::mat4			( 1.0f );


	transformMat[2] = glm::perspective 	( getFrustum()->getFovY(), viewPortWidth*1.0f / viewPortHeight*1.0f,
									  getFrustum()->getNearD(), getFrustum()->getFarD() );

	//transformMat[2] = glm::ortho ( -0.0125f, 0.0125f, -0.0125f, 0.0125f,  getFrustum()->getNearD(), getFrustum()->getFarD()  );

	transformMat[1] = glm::lookAt		( position, position+direction, up );
	transformMat[0]	= glm::mat4			( 1.0f );

	glNamedBufferDataEXT	( MVPBufferId,  sizeof(glm::mat4)*3, &transformMat[0], GL_DYNAMIC_DRAW);

}
//
//=======================================================================================
//
void Camera::readMatrices( void )
{
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	{
		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();
		{
			setView();
			glGetDoublev( GL_PROJECTION_MATRIX, projectionMatrix );
			glGetDoublev( GL_MODELVIEW_MATRIX,  modelviewMatrix  );
			glMatrixMode( GL_MODELVIEW );
		}
		glPopMatrix();
		glMatrixMode( GL_PROJECTION );
	}
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
}
//
//=======================================================================================
//
unsigned int Camera::getId( void )
{
	return id;
}

//=======================================================================================

void Camera::setInactive( void )
{
	active = false;
}
//
//=======================================================================================
//
int Camera::getType( void )
{
	return type;
}
//
//=======================================================================================
//
Frustum* Camera::getFrustum( void )
{
	return frustum;
}
//
//=======================================================================================
//
void Camera::setEyeSeparation( float value )
{
	eye_separation = value;
	if( eye_separation < 0.0f )
	{
		eye_separation = 0.0;
	}
}
//
//=======================================================================================
//
void Camera::calcStereoInternals( int stereoMode )
{
	int viewportDims[4];
	glGetIntegerv( GL_VIEWPORT, viewportDims );
	frustum->updateRatio();

	if( stereoMode == ACTIVE_STEREO )
	{
		wd2		= frustum->getNearD() * frustum->TANG;
		ndfl	= frustum->getNearD() / (frustum->getFarD() + 100.0f);

		R		= normalize( direction * up );

		R.x    *= eye_separation * 0.5f;
		R.y    *= eye_separation * 0.5f;
		R.z    *= eye_separation * 0.5f;

		top		=  wd2;
		bottom	= -wd2;
	}
	else if( stereoMode == ANAGLYPH_STEREO )
	{
		float length, radians;
		// Clip to avoid extreme stereo
		frustum->setNearD( focallength / 5.0f );
		// Derive the the "right" vector
		CROSSPROD( direction, up, R );
		NORMALIZE( R, length );
		R.x *= eye_separation / 2.0f;
		R.y *= eye_separation / 2.0f;
		R.z *= eye_separation / 2.0f;
		// Misc stuff
		radians = DEG2RAD * frustum->getFovY() / 2.0f;
		wd2     = frustum->getNearD() * tan( radians );
		ndfl    = frustum->getNearD() / focallength;
		// Set the buffer for writing and reading
		glDrawBuffer( GL_BACK );
		glReadBuffer( GL_BACK );
		// Clear things
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glClear( GL_ACCUM_BUFFER_BIT ); // Not strictly necessary
		glViewport( 0, 0, (GLsizei)viewportDims[2], (GLsizei)viewportDims[3] );
	}
}
//
//=======================================================================================
//
void Camera::setLeftView( int stereoMode )
{
	fLeft  = -frustum->getRatio() * wd2 + 0.5f * eye_separation * ndfl;
	fRight =  frustum->getRatio() * wd2 + 0.5f * eye_separation * ndfl;

	if( stereoMode == ACTIVE_STEREO )
	{
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		glFrustum(	fLeft,
					fRight,
					bottom,
					top,
					frustum->getNearD(),
					frustum->getFarD()	);
		glMatrixMode( GL_MODELVIEW );
		glDrawBuffer( GL_BACK_LEFT );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glLoadIdentity();
		gluLookAt(
					position.x - R.x,
					position.y - R.y,
					position.z - R.z,
					position.x + direction.x - R.x,
					position.y + direction.y - R.y,
					position.z + direction.z - R.z,
					up.x,
					up.y,
					up.z							);
	}
	else if( stereoMode == ANAGLYPH_STEREO )
	{
		glColorMask( GL_TRUE, GL_TRUE,  GL_TRUE,  GL_TRUE );
		glColorMask( GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE );
		// Create the projection
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		fLeft   = -frustum->getRatio() * wd2 + eye_separation * ndfl;
		fRight  =  frustum->getRatio() * wd2 + eye_separation * ndfl;
		top    =  wd2;
		bottom = -wd2;
		glFrustum(	fLeft,
					fRight,
					bottom,
					top,
					frustum->getNearD(),
					frustum->getFarD()	);
		// Create the model
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		gluLookAt(	position.x - R.x,
					position.y - R.y,
					position.z - R.z,
					position.x - R.x + direction.x,
					position.y - R.y + direction.y,
					position.z - R.z + direction.z,
					up.x,
					up.y,
					up.z							);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}
}
//
//=======================================================================================
//
void Camera::setRightView( int stereoMode )
{
	fLeft  = -frustum->getRatio() * wd2 - 0.5f * eye_separation * ndfl;
    fRight =  frustum->getRatio() * wd2 - 0.5f * eye_separation * ndfl;

	if ( stereoMode == ACTIVE_STEREO )
	{
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		glFrustum(	fLeft,
					fRight,
					bottom,
					top,
					frustum->getNearD(),
					frustum->getFarD()	);
		glMatrixMode( GL_MODELVIEW );
		glDrawBuffer( GL_BACK_RIGHT );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glLoadIdentity();
		gluLookAt(
					position.x + R.x,
					position.y + R.y,
					position.z + R.z,
					position.x + direction.x + R.x,
					position.y + direction.y + R.y,
					position.z + direction.z + R.z,
					up.x,
					up.y,
					up.z							);
	}
	else if( stereoMode == ANAGLYPH_STEREO )
	{
		glFlush();
		glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
		// Write over the accumulation buffer
		glAccum( GL_LOAD, 1.0 ); // Could also use glAccum(GL_ACCUM,1.0);
		glDrawBuffer( GL_BACK );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		// The projection
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		fLeft   = -frustum->getRatio() * wd2 - eye_separation * ndfl;
		fRight  =  frustum->getRatio() * wd2 - eye_separation * ndfl;
		top    =  wd2;
		bottom = -wd2;
		glFrustum(	fLeft,
					fRight,
					bottom,
					top,
					frustum->getNearD(),
					frustum->getFarD()	);
		// Right eye filter
		glColorMask( GL_TRUE,  GL_TRUE, GL_TRUE, GL_TRUE );
		glColorMask( GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE );
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		gluLookAt(	position.x + R.x,
					position.y + R.y,
					position.z + R.z,
					position.x + R.x + direction.x,
					position.y + R.y + direction.y,
					position.z + R.z + direction.z,
					up.x,
					up.y,
					up.z							);
	}
}
//
//=======================================================================================

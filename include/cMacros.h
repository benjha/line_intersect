//
//										Benjamin Hernandez, PhD. benjamin.hernandez@bsc.es
//																	   All Rights Reserved
// 														   Barcelona Supercomputing Center
//
//															   See ReadMe.txt for details
//
//=======================================================================================
#pragma once

#if !defined(__gl_h_) || !defined(__GL_H__) || !defined(_GL_H) || !defined(__X_GL_H)
#warning macros.h included before gl.h
#endif

#ifndef FREE_TEXTURE
	#define FREE_TEXTURE( ptr )				\
	if( ptr )								\
	{										\
		glDeleteTextures( 1, &ptr );		\
		ptr = 0;							\
	}
#endif


//=======================================================================================

#ifndef FREE_MEMORY
#define FREE_MEMORY(ptr)	\
    if (ptr) {				\
		delete ptr;			\
        ptr=0;			\
    }
#endif

#ifndef FREE_ARRAY
#define FREE_ARRAY(ptr)	\
    if (ptr) {				\
		delete [] ptr;			\
        ptr=0;			\
    }
#endif

#ifndef FREE_GLBO
#define FREE_GLBO(ptr) \
	if (ptr) { \
		glDeleteBuffers(1,&ptr); \
		ptr = 0; \
	}
#endif

#ifndef FREE_TEXTURE
#define FREE_TEXTURE(ptr) \
	if (ptr) { \
		glDeleteTextures(1,&ptr); \
		ptr = 0; \
	}
#endif

#ifndef FREE_OGL_LIST
#define FREE_OGL_LIST(ptr) \
	if (ptr) { \
		glDeleteLists(ptr,1); \
		ptr = 0; \
	}
#endif

#define WINDOW_WIDTH 		1920
#define WINDOW_HEIGHT 		1080

#define MAP_TILE_WIDTH		512
#define MAP_TILE_HEIGHT		512

#define deg2rad(d) (((d)*M_PI)/180)
#define rad2deg(d) (((d)*180)/M_PI)
#define earth_radius 6378137

#ifndef DEG2RAD
	#define DEG2RAD	0.01745329251994329576f
#endif

#ifndef RAD2DEG
	#define RAD2DEG	57.29577951308232087679f
#endif

#define CROSSPROD( p1, p2, p3 ) \
   p3.x = p1.y*p2.z - p1.z*p2.y; \
   p3.y = p1.z*p2.x - p1.x*p2.z; \
   p3.z = p1.x*p2.y - p1.y*p2.x

#ifndef __NORMALIZE
#define __NORMALIZE 1
#define NORMALIZE(p,length) \
   length = sqrt(p.x * p.x + p.y * p.y + p.z * p.z); \
   if( length != 0 ) { \
      p.x /= length; \
      p.y /= length; \
      p.z /= length; \
   } else { \
      p.x = 0; \
      p.y = 0; \
      p.z = 0; \
   }
#endif

//#define y2lat_d(y) { rad2deg(2 * atan(exp(  deg2rad(y) ) ) - M_PI/2); }

// The following functions take or return there results in degrees
//double y2lat_d(double y) { return rad2deg(2 * atan(exp(  deg2rad(y) ) ) - M_PI/2); }
//double x2lon_d(double x) { return x; }
//double lat2y_d(double lat) { return rad2deg(log(tan(M_PI/4+ deg2rad(lat)/2))); }
//double lon2x_d(double lon) { return lon; }

// The following functions take or return there results in something close to meters, along the equator
//double y2lat_m(double y) { return rad2deg(2 * atan(exp( (y / earth_radius ) )) - M_PI/2); }
//double x2lon_m(double x) { return rad2deg(x / earth_radius); }
#define y2lat_m(y)		( rad2deg( 2 * atan( exp( (y / earth_radius)))- M_PI/2) );
#define x2lon_m(x)		( rad2deg ( x / earth_radius) );
#define lat2y_m(lat)	( earth_radius * log(tan(M_PI/4+ deg2rad(lat)/2)) )
#define lon2x_m(lon) 	( deg2rad(lon) * earth_radius )


//Resolution (meters/pixel) for given zoom level (measured at Equator)"

#define resolution(zoom) ( (2 * M_PI * 6378137) / ( MAP_TILE_WIDTH  * pow(2,zoom)) );


//=======================================================================================

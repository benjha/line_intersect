//============================================================================
// Name        : LineIntersection.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include <iostream>
#include <time.h>
#include <stdlib.h>
#include "cLineIntersector.h"
#include "cMacros.h"

using namespace std;

cLineIntersector lineIntersector (WINDOW_WIDTH, WINDOW_HEIGHT,"data/knox_link.dat" );


int main ( int argc, char** argv )
{
	srand (time(NULL));
	lineIntersector.start ( argc, argv, 4, 3, false );
	return 0;
}

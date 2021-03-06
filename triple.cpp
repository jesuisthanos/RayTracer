//
//  Framework for a raytracer
//  File: triple.cpp
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Author: Maarten Everts
//    Stephane Gosset
//    Sy-Thanh Ho
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#include "triple.h"

ostream& operator<<(ostream& s, const Triple& v) 
{
    return s << '[' << v.x << ',' << v.y << ',' << v.z << ']';
}

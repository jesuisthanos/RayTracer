//
//  Framework for a raytracer
//  File: sphere.cpp
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Authors:
//    Maarten Everts
//    Jasper van de Gronde
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#include "sphere.h"
#include <iostream>
#include <math.h>

/************************** Sphere **********************************/

Hit Sphere::intersect(const Ray &ray)
{
    /****************************************************
    * RT1.1: INTERSECTION CALCULATION
    *
    * Given: ray, position, r
    * Sought: intersects? if true: *t
    * 
    * Insert calculation of ray/sphere intersection here. 
    *
    * You have the sphere's center (C) and radius (r) as well as
    * the ray's origin (ray.O) and direction (ray.D).
    *
    * If the ray does not intersect the sphere, return Hit::NO_HIT().
    * Otherwise, return an instance of Hit() with the distance of the
    * intersection point from the ray origin as t and the normal ad N (see example).
    ****************************************************/

    // place holder for actual intersection calculation

    Vector OC = position - ray.O;
	Vector oc = OC.normalized();  // normalized OC
	/* sine of the angle between the perpendicular vector from point of
		view to the sphere, and OC
	*/
	double sin = r / OC.length();
	double cos = sqrt(1 - sin * sin); // and it's cosine
    if (oc.dot(ray.D) < cos) {  // the definition of dot function is ||a||.||b||.cos, so ...
        return Hit::NO_HIT();
    }
    double t = 1000;
	/** Now we consider the angle between OC and D is theta */
	double cos1 = oc.dot(ray.D);
	double sin1 = sqrt(1 - cos1 * cos1);
	double x = sqrt(r * r - OC.length() * OC.length() * sin1 * sin1);
	t = OC.length() * cos1 - x;

    /****************************************************
    * RT1.2: NORMAL CALCULATION
    *
    * Given: t, C, r
    * Sought: N
    * 
    * Insert calculation of the sphere's normal at the intersection point.
    ****************************************************/
	/* If we call the intersection point is P then CP = OP - OC
	At the same time OP = ray.D * t;
	*/
    Vector N = ray.D * t - OC;

    return Hit(t,N);
}

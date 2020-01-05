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
//    Stephane Gosset
//    Sy-Thanh Ho
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

    Vector OC = position - ray.O;
    // normalized OC
	Vector oc = OC.normalized();
    
	// sine of the angle between the perpendicular vector from point of view to the sphere, and OC
	double sin = r / OC.length();
    // and it's cosine
	double cos = sqrt(1 - sin * sin);

    // the definition of dot function is ||a||.||b||.cos, so ...
    if (oc.dot(ray.D) < cos) {
        return Hit::NO_HIT();
    }
    double t = 1000;

	// Now we consider the angle between OC and D is theta
	double cos1 = oc.dot(ray.D);
	// double sin1 = sqrt(1 - cos1 * cos1);
	double x = sqrt(r * r - OC.length() * OC.length() * (1 - cos1 * cos1));
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
	N = N / N.length();

    return Hit(t,N);
}

double Sphere::getZPos(){
    return position.z;
}

Triple Sphere::mapTexture(const Ray &ray, const Hit &hit, const Point &point){
    Vector longitude = arctic.cross(hit.N).cross(arctic).normalized();
    //Vector latitude = arctic.cross(hit.normalize();
    Vector origin = arctic.cross(greenwich).cross(arctic).normalized();

    double dx(0.0);
    double dy(0.0);

    bool left = longitude.cross(origin).dot(arctic) < 0;
    double dummyLong = -longitude.dot(origin) / 4 + 0.25;

    if (left) {
        dx = -dummyLong;
    }
    else {
        dx = dummyLong;
    }

    bool up = arctic.dot(hit.N) > 0;

    double dummyLat = (1 - hit.N.dot(longitude)) / 2;

    if (up) {
        dy = -dummyLat;
    }
    else {
        dy = dummyLat;
    }

    return Triple(dx + 0.5, 0, dy + 0.5);
}

void Sphere::rotateSystem() {
    rotationVect.normalize();
    double factor1 = -sin(rotationAngle * MATHPI / 180);
    double factor2 = cos(rotationAngle * MATHPI / 180);
    Vector a = arctic.dot(rotationVect) * rotationVect;
    Vector a1 = arctic - a;
    Vector a2 = a1.cross(rotationVect);
    cout << a2 << endl;
    if ((a2.x == 0.0) && (a2.y == 0.0) && (a2.z == 0.0)) {}
    else {
        a2.normalize();
        Vector a3 = a1.normalized();
        Vector a4 = factor1 * a2 + factor2 * a3;
        Vector a5 = a4 * a1.length() + a;
        arctic = a5;
    }
    

    Vector b = greenwich.dot(rotationVect) * rotationVect;
    Vector b1 = greenwich - b;
    Vector b2 = b1.cross(rotationVect);
    if ((b2.x == 0.0) && (b2.y == 0.0) && (b2.z == 0.0)) {}
    else {
        b2.normalize();
        Vector b3 = b1.normalized();
        Vector b4 = factor1 * b2 + factor2 * b3;
        Vector b5 = b4 * b1.length() + b;
        greenwich = b5;
    }
}

string Sphere::objType() {
    return "sphere";
}

void Sphere::setArctic(Vector newArc){
    arctic = newArc;
}
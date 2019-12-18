//
//  Framework for a raytracer
//  File: sphere.h
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

#ifndef SPHERE_H_115209AE
#define SPHERE_H_115209AE

#include "object.h"

class Sphere : public Object
{
public:
    Sphere(Point position,double r,Vector arctic,Vector greenwich) : position(position), r(r), arctic(arctic), greenwich(greenwich), isRotated(false),
    rotationAngle(0.0), rotationVect(Vector(0, 1, 0)){ }
    Sphere(Point position, double r, Vector arctic, Vector greenwich, Vector rotationVect, double rotationAngle) :
        position(position), r(r), arctic(arctic), greenwich(greenwich), rotationVect(rotationVect), rotationAngle(rotationAngle), isRotated(true) {
        rotationVect.normalize();
        Vector a = arctic.dot(rotationVect) * rotationVect;
        Vector a1 = arctic - a;
        Vector b = greenwich.dot(rotationVect) * rotationVect;
        Vector b1 = greenwich - b;
    }

    virtual Hit intersect(const Ray &ray);
    virtual Triple mapTexture(const Ray &ray, const Hit &hit, const Point &point);

    const Point position;
    const double r;
    Vector arctic;
    Vector greenwich;
    const bool isRotated;
    Vector rotationVect;
    const double rotationAngle;
};

#endif /* end of include guard: SPHERE_H_115209AE */

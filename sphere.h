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
#define MATHPI 3.1415927

#include "object.h"

class Sphere : public Object
{
public:
    Sphere(Point position,double r,Vector arctic,Vector greenwich) : position(position), r(r), arctic(arctic), greenwich(greenwich), rotationVect(Vector(0, 1, 0)), rotationAngle(0.0), isRotated(false){}
    Sphere(Point position, double r, Vector arctic, Vector greenwich, Vector rotationVect, double rotationAngle) :
        position(position), r(r), arctic(arctic), greenwich(greenwich), rotationVect(rotationVect), rotationAngle(rotationAngle), isRotated(true) {
        // rotationVect.normalize();
        // Vector a = arctic.dot(rotationVect) * rotationVect;
        // Vector a1 = arctic - a;
        // Vector a2 = a1.cross(rotationVect).normalized();
        // Vector a3 = a1.normalized();
        // Vector b = greenwich.dot(rotationVect) * rotationVect;
        // Vector b1 = greenwich - b;
        // Vector b2 = b1.cross(rotationVect).normalized();
        // Vector b3 = b1.normalized();

        // double factor1 = -sin(rotationAngle * MATHPI / 180);
        // double factor2 = cos(rotationAngle * MATHPI / 180);
        // Vector a4 = factor1 * a2 + factor2 * a3;
        // Vector a5 = a4 * a1.length() + a;
        // Vector b4 = factor1 * b2 + factor2 * b3;
        // Vector b5 = b4 * b1.length() + b;

        // arctic = a5;
        // greenwich = b5;
    }

    virtual Hit intersect(const Ray &ray);
    virtual double getZPos();
    virtual Triple mapTexture(const Ray &ray, const Hit &hit, const Point &point);
    virtual string objType();

    void rotateSystem();
    void setArctic(Vector newArc);

    const Point position;
    const double r;
    Vector arctic;
    Vector greenwich;
    Vector rotationVect;
    const double rotationAngle;
    const bool isRotated;
};

#endif /* end of include guard: SPHERE_H_115209AE */

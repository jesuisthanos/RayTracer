//
//  Framework for a raytracer
//  File: object.h
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

#ifndef OBJECT_H_AXKLE0OF
#define OBJECT_H_AXKLE0OF

#include "triple.h"
#include "light.h"

class Material;

class Object {
public:
    Material *material;

    virtual ~Object() { }

    virtual Hit intersect(const Ray &ray) = 0;

    virtual double getZPos() = 0;
    
    virtual Triple mapTexture(const Ray &ray, const Hit &hit, const Point &point) = 0;

    virtual string objType() = 0;
};

#endif /* end of include guard: OBJECT_H_AXKLE0OF */

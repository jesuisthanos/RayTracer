//
//  Framework for a raytracer
//  File: cone.cpp
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//
//    Stephane Gosset
//    Sy-Thanh Ho

#include "cone.h"
#include <iostream>
#include <math.h>

#define N 2

/************************** Cone **********************************/

Hit Cone::intersect(const Ray& ray)
{
	return Hit::NO_HIT();


}

double Cone::getZPos(){
	return (base.z + top.z)/2;
}

Triple Cone::mapTexture(const Ray &ray, const Hit &hit, const Point &point){

	return Triple(0, 0, 0);
}

string Cone::objType() {
	return "Cone";
}
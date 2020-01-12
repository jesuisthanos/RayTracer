//
//  Framework for a raytracer
//  File: triple.cpp
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//
//    Stephane Gosset
//    Sy-Thanh Ho

#include "circle.h"
#include <iostream>
#include <math.h>

#define N 2

/************************** Triangle **********************************/

Hit Circle::intersect(const Ray& ray)
{
	double t = norm.dot(center - ray.O) / norm.dot(ray.D);
	if (t > 0) {
		if (ray.D.dot(norm) < 0) return Hit(t, norm);
		else return Hit(t, -norm);
	}
	else {
		return Hit::NO_HIT();
	}
	return Hit::NO_HIT();
}

double Circle::getZPos(){
	return center.z;
}

Triple Circle::mapTexture(const Ray &ray, const Hit &hit, const Point &point){
	return Triple(0, 0, 0);
}

string Circle::objType() {
	return "circle";
}
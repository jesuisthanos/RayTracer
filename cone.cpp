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
	//return Hit::NO_HIT();

	Vector BA = ray.O - top;
	Vector BC = base - top;
	Vector bc = BC.normalized();
	double _BD = BA.dot(bc);
	
	Point D = top + _BD * bc;

	Vector AD = D - ray.O;
	double _AF = AD.dot(ray.D.normalized());

	Point F = ray.O + ray.D.normalized() * _AF;

	double _DF = (F - D).length();

	double _BC = BC.length();
	double _BD = (top - D).length();

	double xp = radius * _BD / _BC;
	if (_DF <= xp) {
		double t = (F - ray.O).length() - sqrt(radius * radius - _DF * _DF);
		Point Fp = ray.O + t * ray.D.normalized();
		Vector BFp = Fp - top;
		Vector Norm = BFp.cross(BFp.cross(BC)).normalized();
		return Hit(t, Norm);
	}
	else {
		return Hit::NO_HIT();
	}
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
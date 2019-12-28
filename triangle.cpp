//
//  Framework for a raytracer
//  File: triple.cpp
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//
//    Stephane Gosset
//    Sy-Thanh Ho

#include "triangle.h"
#include <iostream>
#include <math.h>

#define N 2

/************************** Triangle **********************************/

Vector Triangle::getNormal() {
	return normal;
}

void Triangle::setNormal(Vector newNormal) {
	normal = newNormal;
}

Hit Triangle::intersect(const Ray& ray)
{
	// return Hit::NO_HIT();
	// if (ray.D.dot(getNormal()) < 0.0) {
	// 	cout << getNormal();
	// 	setNormal(getNormal() * -1);
	// }

	// cout << getNormal();

	Vector OA = vertex1 - ray.O;
	if (OA.cross(ray.D).length() < 0.0001) {
		return Hit(OA.length(), getNormal());
	} else {  // it doesn't hit at A, we call it E
		double k = OA.dot(getNormal()) / (ray.D.dot(getNormal()));
		Triple E = ray.O + k * (Triple)ray.D;

		// We check on which side of each edge E is
		//AB
		Vector AC = vertex3 - vertex1;
		Vector BC = vertex3 - vertex2;
		Vector AE = E - vertex1;
		Vector BE = E - vertex2;
		if(AC.cross(BC).dot(AE.cross(BE)) < 0.0){
			return Hit::NO_HIT();
		}

		//AC
		Vector AB = vertex2 - vertex1;
		Vector CB = vertex2 - vertex3;
		Vector CE = E - vertex3;
		if(AB.cross(CB).dot(AE.cross(CE)) < 0.0){
			return Hit::NO_HIT();
		}
		
		//BC
		Vector BA = vertex1 - vertex2;
		Vector CA = vertex1 - vertex3;
		if(BA.cross(CA).dot(BE.cross(CE)) < 0.0){
			return Hit::NO_HIT();
		}
		if (ray.D.dot(getNormal()) < 0.0) {
			return Hit(k, getNormal());
		} else {
			return Hit(k, getNormal() * (-1));
		}	
	}
}

Triple Triangle::mapTexture(const Ray &ray, const Hit &hit, const Point &point){
	Vector AB = vertex2 - vertex1;
	Vector AC = vertex3 - vertex1;
	Vector horizontal = AB - (AB.dot(AC.normalized()))*(AC.normalized());
	Vector vertical = AC - (AC.dot(AB.normalized()))*(AB.normalized());
	Vector horizontalUnit = horizontal.normalized();
	Vector verticalUnit = vertical.normalized();
	Vector pointVector = point - vertex1;
	double horizontalOffset = pointVector.dot(horizontalUnit) / horizontal.length();
	double verticalOffset = pointVector.dot(verticalUnit) / vertical.length();

	return rightSide ? Triple(1.0 - horizontalOffset, 0, 1.0 - verticalOffset) : Triple(horizontalOffset, 0, verticalOffset);
}
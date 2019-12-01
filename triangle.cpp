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
		// we use barycentric coordinate system and a set of 3 equations
		// so we could easily find this out, watch
		/*
		Vector AB = vertex2 - vertex1;
		Vector AC = vertex3 - vertex1;
		Vector ab = AB / AB.length();
		Vector ac = AC / AC.length();
		Vector AE = E - vertex1;
		// we have to solve a, b that satisfy:
		// a*ab + b*ac == AE
		// a * ab.x + b * ac.x = AE.x
		// a * ab.y + b * ac.y = AE.y
		// a * ab.z + b * ac.z = AE.z
		// because E is already on this plane, we just need 2 out of these
		// 3 functions
		double A[2][2] = {
			{ab.x, ac.x},
			{ab.y, ac.y}
		};
		double A_inv[2][2];
		double factor = 1.0 / (A[0][0] * A[1][1] - A[0][1] * A[1][0]);
		A_inv[0][0] = factor * A[1][1];
		A_inv[0][1] = factor * A[0][1] * -1;
		A_inv[1][0] = factor * A[1][0] * -1;
		A_inv[1][1] = factor * A[0][0];
		double a = A_inv[0][0] * AE.x + A_inv[0][1] * AE.y;
		double b = A_inv[1][0] * AE.x + A_inv[1][1] * AE.y;
		if ((a < 0.0) || (b < 0.0) || (a + b > 1)) {
			return Hit::NO_HIT();
		} else {
			return Hit(k, getNormal());
		}
		*/

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
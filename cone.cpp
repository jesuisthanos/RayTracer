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

	Vector D = ray.D;
	Vector O = ray.O;
	Vector CB = base - top;
	Vector CO = O - top;
	Vector V = CB.normalized();
	double dv_ = D.dot(V);
	double max_len = sqrt(radius * radius + CB.dot(CB));

	double cos_t = CB.length() / max_len;
	//cout << cos_t << endl;
	double cos2_t = cos_t * cos_t;

	double a = dv_ * dv_ - cos2_t;

	double b = 2 * (dv_ * CO.dot(V) - ray.D.dot(CO) * cos2_t);

	double c = CO.dot(V) * CO.dot(V) - CO.dot(CO) * cos2_t;

	double delta = b * b - 4 * a * c;

	if (delta < 0) {
		return Hit::NO_HIT();
	}
	else if (delta == 0) {
		double t = -b / (2 * a);
		if (t < 0) {
			return Hit::NO_HIT();
		}
		else {
			Point P = O + t * D;
			Vector CP = P - top;
			Vector PB = base - P;
			Vector norm = CP.cross((CP).cross(PB)).normalized();
			if ((CP.dot(CB) < 0) || (CP.length() > max_len)) return Hit::NO_HIT();
			else return Hit(t, norm);
		}
	}
	else {
		double t1 = (-b - sqrt(delta)) / (2 * a);
		double t2 = (-b + sqrt(delta)) / (2 * a);

		if ((t1 < 0) && (t2 < 0)) {
			return Hit::NO_HIT();
		}
		else {
			if (t1 < t2) {
				Point P = O + t1 * D;
				Vector CP = P - top;
				Vector PB = base - P;
				Vector norm = CP.cross((CP).cross(PB)).normalized();
				if ((CP.dot(CB) < 0) || (CP.length() > max_len)) return Hit::NO_HIT();
				else return Hit(t1, norm);
			}
			else {
				Point P = O + t2 * D;
				Vector CP = P - top;
				Vector PB = base - P;
				Vector norm = CP.cross((CP).cross(PB)).normalized();
				if ((CP.dot(CB) < 0) || (CP.length() > max_len)) return Hit::NO_HIT();
				else return Hit(t2, norm);
			}
		}
	}
}

double Cone::getZPos(){
	return (base.z + top.z)/2;
}

Triple Cone::mapTexture(const Ray &ray, const Hit &hit, const Point &point){
	Vector CP = point - top;
	Vector CB = base - top;
	Vector x = CB.cross(CP).cross(CB).normalized();
	double max_len = sqrt(radius * radius + CB.dot(CB));
	double dy = CP.length() / max_len;
	double dx = x.dot(org);

	if (x.cross(org).dot(CB) > 0) { //right	
		dx *= -1;
		dx += 1;
		dx /= 4;		
	}
	else { // left
		dx += 1;
		dx /= 4;
		dx += 0.5;
	}

	return Triple(dx, 0, dy);
	/*return Triple(0, 0, 0);*/
}

string Cone::objType() {
	return "Cone";
}
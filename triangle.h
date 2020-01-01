#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object.h"

class Triangle : public Object
{
public:
	Vector normal;
	Triangle(Point v1, Point v2, Point v3, bool rightSide) : vertex1(v1), vertex2(v2), vertex3(v3), rightSide(rightSide) {
		Vector norm = (vertex1 - vertex2).cross(vertex1 - vertex3);
		normal = norm / norm.length();
	}
	
	Vector getNormal();
	void setNormal(Vector newNorm);

	virtual Hit intersect(const Ray& ray);
    virtual double getZPos();
    virtual Triple mapTexture(const Ray &ray, const Hit &hit, const Point &point);

	const Point vertex1;
	const Point vertex2;
	const Point vertex3;

	const bool rightSide;
};

#endif /* TRIANGLE_H */
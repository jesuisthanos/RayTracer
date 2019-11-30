#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object.h"

class Triangle : public Object
{
public:
	Triangle(Point v1, Point v2, Point v3) : vertex1(v1), vertex2(v2), vertex3(v3) { }

	virtual Hit intersect(const Ray& ray);

	const Point vertex1;
	const Point vertex2;
	const Point vertex3;
};

#endif /* TRIANGLE_H */
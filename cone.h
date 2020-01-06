#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object.h"

class Cone : public Object
{
public:
	Cone(Point base, Point top, double radius) : base(base), top(top), radius(radius) {
		
	}

	virtual Hit intersect(const Ray& ray);
    virtual double getZPos();
    virtual Triple mapTexture(const Ray &ray, const Hit &hit, const Point &point);
	virtual string objType();


	Point base;
	Point top;
	double radius;
};

#endif /* TRIANGLE_H */
#ifndef CONE_H
#define CONE_H

#include "object.h"

class Cone : public Object
{
public:
	Cone(Point base, Point top, double radius, Vector org) : base(base), top(top), radius(radius), org(org) {
		
	}

	virtual Hit intersect(const Ray& ray);
    virtual double getZPos();
    virtual Triple mapTexture(const Ray &ray, const Hit &hit, const Point &point);
	virtual string objType();


	Point base;
	Point top;
	double radius;
	Vector org;
};

#endif /* CONE_H */
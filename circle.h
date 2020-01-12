#ifndef CIRCLE_H
#define CIRCLE_H

#include "object.h"

class Circle : public Object
{
public:
	Vector normal;
	Circle(Point center, double radius, Vector norm) : center(center), radius(radius), norm(norm) {
	}


	virtual Hit intersect(const Ray& ray);
    virtual double getZPos();
    virtual Triple mapTexture(const Ray &ray, const Hit &hit, const Point &point);
	virtual string objType();

	const Point center;
	double radius;
	Vector norm;
};

#endif /* CIRCLE_H */
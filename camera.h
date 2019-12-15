#ifndef TRIPLE_H
#define TRIPLE_H

#include "triple.h"

class Camera
{
public:
	Triple eye;
	Triple center;
	Vector up;
	int viewWidth;
	int viewHeight;
	
	Camera(Triple eye = Triple(200, 200, 1000),
		Triple center = Triple(200, 200, 0),
		Vector up = Vector(0, 1, 0),
		int viewWidth = 400,
		int viewHeight = 400)
		: eye(eye), center(center), up(up), viewWidth(viewWidth), viewHeight(viewHeight){}
	
};

#endif /* TRIPLE_H */
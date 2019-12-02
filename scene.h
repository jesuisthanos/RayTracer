//
//  Framework for a raytracer
//  File: scene.h
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Authors:
//    Maarten Everts
//    Jasper van de Gronde
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#ifndef SCENE_H_KNBLQLP6
#define SCENE_H_KNBLQLP6

#include <vector>
#include "triple.h"
#include "light.h"
#include "object.h"
#include "sphere.h"
#include "image.h"

class Scene
{
private:
    std::vector<Object*> objects;
    std::vector<Light*> lights;
    Triple eye;
    string mode;
    bool shadows;
    int maxRecursionDepth;
public:
    Color trace(const Ray &ray);
    bool traceShadow(const Ray &ray, double lightDistance);
    void render(Image &img);
    void addObject(Object *o);
    void addLight(Light *l);
    void setEye(Triple e);
    void setMode(string s);
    void setShadows(bool s);
    void setRecursionDepth(int d);
    void sortObjects(vector<Object*>&, int, int);
    void sortZBuffer();
    int partition(vector<Object*>&, int, int);
    unsigned int getNumObjects() { return objects.size(); }
    unsigned int getNumLights() { return lights.size(); }
};

#endif /* end of include guard: SCENE_H_KNBLQLP6 */

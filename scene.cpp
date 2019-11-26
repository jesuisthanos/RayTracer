//
//  Framework for a raytracer
//  File: scene.cpp
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

#include "scene.h"
#include "material.h"

Color Scene::trace(const Ray &ray)
{
    int origin = ((Sphere*)objects[0])->position.z - 200;
    int maximum = ((Sphere*)objects[objects.size()-1])->position.z - origin;
    double a3rdcolor;
    // Find hit object and distance
    Hit min_hit(std::numeric_limits<double>::infinity(),Vector());
    Object *obj = NULL;
    for (unsigned int i = 0; i < objects.size(); ++i) {
        Hit hit(objects[i]->intersect(ray));
        if (hit.t<min_hit.t) {
            min_hit = hit;
            obj = objects[i];
        }
    }

    // No hit? Return background color.
    if (!obj) return Color(0.0, 0.0, 0.0);

    Material *material = obj->material;            //the hit objects material
    Point hit = ray.at(min_hit.t);                 //the hit point
    Vector N = min_hit.N;                          //the normal at hit point
    Vector V = -ray.D;                             //the view vector


    /****************************************************
    * This is where you should insert the color
    * calculation (Phong model).
    *
    * Given: material, hit, N, V, lights[]
    * Sought: color
    *
    * Hints: (see triple.h)
    *        Triple.dot(Vector) dot product
    *        Vector+Vector      vector sum
    *        Vector-Vector      vector difference
    *        Point-Point        yields vector
    *        Vector.normalize() normalizes vector, returns length
    *        double*Color        scales each color component (r,g,b)
    *        Color*Color        dito
    *        pow(a,b)           a to the power of b
    ****************************************************/
	Vector L;
	Vector L_norm;
    Color matColor = material->color;
	Color color = Color(0.0, 0.0, 0.0);

    // sortZBuffer(objects, 0, objects.size()-1);
    if (mode != "zbuffer") {
        for (std::size_t i = 0; i < lights.size(); ++i) {
        Color lightColor = lights.at(i)->color;
		L = lights.at(i)->position - hit;
		L_norm = L / L.length();

        // Ambient lighting
		color += matColor * lightColor * material->ka;

        // Diffuse lighting
        if (L_norm.dot(N) > 0) {
            color += matColor * lightColor * material->kd *L_norm.dot(N);
        }

        // Specular lighting
        if ((2 * L_norm.dot(N) * N - L_norm).dot(V) > 0) {
		    color += lightColor * material->ks * pow((2 * L_norm.dot(N) * N - L_norm).dot(V), material->n);
        }
	    }   
    } else {
        a3rdcolor = (((Sphere*)obj)->position.z - origin) / maximum * 1.0;
        color = Triple(a3rdcolor, a3rdcolor, a3rdcolor);
    }
	
	

    return color;
}

void Scene::render(Image &img)
{
    int w = img.width();
    int h = img.height();
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Point pixel(x+0.5, h-1-y+0.5, 0);
            Ray ray(eye, (pixel-eye).normalized());
            Color col = trace(ray);
            col.clamp();
            img(x,y) = col;
        }
    }
}

void Scene::addObject(Object *o)
{
    objects.push_back(o);
}

void Scene::addLight(Light *l)
{
    lights.push_back(l);
}

void Scene::setEye(Triple e)
{
    eye = e;
}

void Scene::setMode(string s) {
    mode = s;
}

void Scene::sortObjects(vector<Object*>& objects, int low, int high) {
    cout << low << " " << high << std::endl;
    if (low < high) {
        int pi = partition(objects, low, high);
        cout << "pi = " << pi << endl;

        sortObjects(objects, low, pi - 1);
        sortObjects(objects, pi + 1, high);
    }
}

int Scene::partition(vector<Object*>& objects, int low, int high) {
    int pivot = ((Sphere*)objects[high])->position.z;
    
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (((Sphere*) objects[j])->position.z < pivot) {
            i++;
            std::cout << "-----------" << std::endl;
    for (int i = 0; i < objects.size(); i++) {
        std::cout << ((Sphere*)objects[i])->position.z << std::endl;
    }
            cout << ((Sphere*)objects[i])->position.z << " " << ((Sphere*)objects[j])->position.z << std::endl;
            std::swap(objects[i], objects[j]);
            std::cout << "-----------" << std::endl;
    for (int i = 0; i < objects.size(); i++) {
        std::cout << ((Sphere*)objects[i])->position.z << std::endl;
    }
            cout << ((Sphere*)objects[i])->position.z << " " << ((Sphere*)objects[j])->position.z << std::endl;
        }
    }
    std::swap(objects[i + 1], objects[high]);
    return (i + 1);
}

void Scene::sortZBuffer() {
    sortObjects(objects, 0, objects.size() - 1);
    std::cout << "-----------" << std::endl;
    for (int i = 0; i < objects.size(); i++) {
        std::cout << ((Sphere*)objects[i])->position.z << std::endl;
    }
}
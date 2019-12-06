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
//    Stephane Gosset
//    Sy-Thanh Ho
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#include "scene.h"
#include "material.h"

Color Scene::trace(const Ray &ray, int recursionDepth)
{
    // Find hit object and distance
    Hit min_hit(std::numeric_limits<double>::infinity(),Vector());
    Object *obj = NULL;
    for (unsigned int i = 0; i < objects.size(); ++i) {
        Hit hit(objects[i]->intersect(ray));
        if (hit.t<min_hit.t && hit.t>=0) {
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
	
    Color color = Color(0.0, 0.0, 0.0);
    Color matColor = material->color;

    if (mode == "zbuffer") {    // Render zbuffer as grayscale image
        // Determine far & near boundaries of the buffer
        int far = ((Sphere *)objects[0])->position.z - 100;
        int near = ((Sphere *)objects[objects.size() - 1])->position.z - far;

        // sortZBuffer(objects, 0, objects.size()-1);

        // Get the depth value of the hit point
        double depthValue = (hit.z - far) / near * 1.0;
        // depthValue = (((Sphere*)obj)->position.z - far) / near * 1.0; //TODO: uncomment to get "interesting error" zbuffer image

        // Convert the value to a grayscale color
        color = Triple(depthValue, depthValue, depthValue);


    } else if (mode == "normal") {      // generate normal buffer
        // Convert each coordinate of the normal vector (range [-1,1]) into a color value (range [0,1])
        double xColor = (N.x + 1) / 2;
        double yColor = (N.y + 1) / 2;
        double zColor = (N.z + 1) / 2;

        // Combine the values into one color
        color = Triple(xColor, yColor, zColor);


    } else {    // render with Phong model
        Vector L;
        Vector L_norm;

        // Take each light into account
        for (std::size_t i = 0; i < lights.size(); ++i){
            Color lightColor = lights.at(i)->color;
            L = lights.at(i)->position - hit;
            L_norm = L / L.length();

            // Ambient lighting
            color += matColor * lightColor * material->ka;

            // Detect shadows
            bool inShadow = false;
            if(shadows){
                Ray shadowRay(hit, L_norm);
                inShadow = traceShadow(shadowRay, L.length());
            }

            if(!inShadow){
                // Diffuse lighting
                if (L_norm.dot(N) > 0){
                    color += matColor * lightColor * material->kd * L_norm.dot(N);
                }

                if (mode != "phongNoSpecular"){
                    // Specular lighting
                    if ((2 * L_norm.dot(N) * N - L_norm).dot(V) > 0){
                        color += lightColor * material->ks * pow((2 * L_norm.dot(N) * N - L_norm).dot(V), material->n);
                    }
                }
            }
        }

        //recursion
        if(recursionDepth < maxRecursionDepth){
            Ray traceRay(hit, ray.D - 2*N.dot(ray.D)*N);
            color += trace(traceRay, recursionDepth + 1) * material->ks;
        }
    }
	
	

    return color;
}

bool Scene::traceShadow(const Ray &ray, double lightDistance){
    // Find hit object and distance
    Hit min_hit(lightDistance,ray.D);
    for (unsigned int i = 0; i < objects.size(); ++i) {
        Hit hit(objects[i]->intersect(ray));
        if (hit.t<min_hit.t && hit.t>=0) {
            return true;
        }
    }

    return false;
}

void Scene::render(Image &img)
{
    int w = img.width();
    int h = img.height();
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
        // for (int z = 0; z < w; z++) {
            Point pixel(x+0.5, h-1-y+0.5, 0);
            // Point pixel(0, h-1-y+0.5, w-1-z+0.5);
            Ray ray(eye, (pixel-eye).normalized());
            Color col = trace(ray);
            col.clamp();
            img(x,y) = col;
            // img(z,y) = col;
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

void Scene::setShadows(bool s) {
    shadows = s;
}

void Scene::setRecursionDepth(int d) {
    maxRecursionDepth = d;
    std::cout << "maxRecursionDepth = " << d << std::endl;
}

void Scene::sortObjects(vector<Object*>& objects, int low, int high) {
    // cout << low << " " << high << std::endl;
    if (low < high) {
        int pi = partition(objects, low, high);

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
    //         std::cout << "-----------" << std::endl;
    // for (int i = 0; i < objects.size(); i++) {
    //     std::cout << ((Sphere*)objects[i])->position.z << std::endl;
    // }
            // cout << ((Sphere*)objects[i])->position.z << " " << ((Sphere*)objects[j])->position.z << std::endl;
            std::swap(objects[i], objects[j]);
            // std::cout << "-----------" << std::endl;
    // for (int i = 0; i < objects.size(); i++) {
    //     std::cout << ((Sphere*)objects[i])->position.z << std::endl;
    // }
    //         cout << ((Sphere*)objects[i])->position.z << " " << ((Sphere*)objects[j])->position.z << std::endl;
        }
    }
    std::swap(objects[i + 1], objects[high]);
    return (i + 1);
}

void Scene::sortZBuffer() {
    sortObjects(objects, 0, objects.size() - 1);
    // std::cout << "-----------" << std::endl;
    // for (int i = 0; i < objects.size(); i++) {
    //     std::cout << ((Sphere*)objects[i])->position.z << std::endl;
    // }
}
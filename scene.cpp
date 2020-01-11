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
#include <string>

Color Scene::trace(const Ray &ray, int recursionDepth, double contribution)
{
    // Find hit object and distance
    Hit min_hit(std::numeric_limits<double>::infinity(),Vector());
    Object *obj = NULL;
    for (unsigned int i = 0; i < objects.size(); ++i) {
        Hit hit(objects[i]->intersect(ray));
        if (hit.t<min_hit.t && hit.t>1e-10) {
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

    if (mode == "zbuffer") {    // Render zbuffer as grayscale image
        // Determine far & near boundaries of the buffer
        int far = (objects[0])->getZPos() - 100;
        int near = (objects[objects.size() - 1])->getZPos() - far;

        // sortZBuffer(objects, 0, objects.size()-1);

        // Get the depth value of the hit point
        double depthValue = (hit.z - far) / near * 1.0;

        // Convert the value to a grayscale color
        color = Triple(depthValue, depthValue, depthValue);


    } else if (mode == "normal") {      // generate normal buffer

        /*
        if(material->bumpmapped){
            Triple bumpmap = obj->mapTexture(ray, min_hit, hit);
            float *dx, *dy;
            material->bumpmap->derivativeAt(bumpmap.x, bumpmap.z, dx, dy);

        }
        */

        // Convert each coordinate of the normal vector (range [-1,1]) into a color value (range [0,1])
        double xColor = (N.x + 1) / 2;
        double yColor = (N.y + 1) / 2;
        double zColor = (N.z + 1) / 2;

        // Combine the values into one color
        color = Triple(xColor, yColor, zColor);
    
    } else if (mode == "texCoords") {
        color = obj->mapTexture(ray, min_hit, hit);

    } else if (mode == "gooch") {   // render with Gooch model

        //calculate object color from material & texture
        Color objColor = material->color;
        if(material->textured){
            Triple textureMap = obj->mapTexture(ray, min_hit, hit);
			objColor = objColor * material->texture->colorAt(textureMap.x, textureMap.z);
        }
        
        // Take each light into account
        Vector L;
        Vector L_norm;
        for (std::size_t i = 0; i < lights.size(); ++i){
            Color lightColor = lights.at(i)->color;
            L = lights.at(i)->position - hit;
            L_norm = L.normalized();

            // Gooch shading
            Color kd = lightColor * objColor * material->kd;
            Color kBlue = Triple(0, 0, goochParameters.b);
            Color kYellow = Triple(goochParameters.y, goochParameters.y, 0);
            Color kCool = kBlue + goochParameters.alpha*kd;
            Color kWarm = kYellow + goochParameters.beta*kd;
            color += kCool*(1 - N.dot(L_norm))/2 + kWarm*(1 + N.dot(L_norm))/2;
            
            // Specular lighting
            if ((2 * L_norm.dot(N) * N - L_norm).dot(V) > 0){
                color += lightColor * material->ks * pow((2 * L_norm.dot(N) * N - L_norm).dot(V), material->n);
            }
        }
        
    } else {    // render with Phong model

        //calculate object color from material & texture
        Color objColor = material->color;
        if(material->textured){
            Triple textureMap = obj->mapTexture(ray, min_hit, hit);
			objColor = objColor * material->texture->colorAt(textureMap.x, textureMap.z);
        }
        
        // Take each light into account
        Vector L;
        Vector L_norm;
        for (std::size_t i = 0; i < lights.size(); ++i){
            Color lightColor = lights.at(i)->color;
            L = lights.at(i)->position - hit;
            L_norm = L.normalized();

            // Ambient lighting
            color += objColor * lightColor * material->ka;

            // Detect shadows
            bool inShadow = false;
            if(shadows){
                Ray shadowRay(hit, L_norm);
                inShadow = traceShadow(shadowRay, L.length());
            }

            if(!inShadow){
                // Diffuse lighting
                if (L_norm.dot(N) > 0){
                    color += objColor * lightColor * material->kd * L_norm.dot(N);
                }

                if (mode != "phongNoSpecular"){
                    // Specular lighting
                    if ((2 * L_norm.dot(N) * N - L_norm).dot(V) > 0){
                        color += lightColor * material->ks * pow((2 * L_norm.dot(N) * N - L_norm).dot(V), material->n);
                    }
                }
            }
        }

        // Recursion
        if(recursionDepth < maxRecursionDepth && contribution*material->ks >= 0.1){
            Ray reflectRay(hit, ray.D - 2*N.dot(ray.D)*N);
            color += trace(reflectRay, recursionDepth + 1, contribution*material->ks) * material->ks;
        }

        // Refraction
        if(recursionDepth < maxRecursionDepth && material->eta > 0 && material->refract > 0){
            double n;
            double cos1 = N.dot(ray.D);
            if (cos1 < 0.0) {
                // The ray goes into the object
                if(contribution*material->refract >= 0.1){
                    n = 1.0/material->eta;
                    if (cos1 < 0.0) cos1 = -cos1;
                    Vector C = cos1 * N;
                    double sin1 = sqrt(1.0 - cos1 * cos1);
                    Vector M = (ray.D + C) / sin1;
                    double sin2 = n*n*(1.0-cos1*cos1);
                    double cos2 = sqrt(1.0 - sin2);
                    sin2 = sqrt(sin2);
                    Vector B = -cos2 * N;
                    Vector T = M * sin2 + B;
                    
                    Point C1 = hit + T * 2 * ((Sphere*)obj)->r * cos2;
                    Vector N1 = ((Sphere*)obj)->position - C1;
                    N1 = N1 / N1.length();
                    C = cos2 * N1;
                    M = (T + C) / sin2;
                    B = -cos1 * N1;
                    Vector T1 = M * sin1 + B;
                    // Ray refractRay(hit, T);
                    Ray refractRay(C1, T1);
                    color += trace(refractRay, recursionDepth, contribution*material->refract) * material->refract;
                    // if(sin2 <= 1.0){
                    //     double cos2 = sqrt(1.0 - sin2);
                    //     Ray refractRay(hit, n*ray.D + (n*cos1-cos2)*N);
                    //     color += trace(refractRay, recursionDepth, contribution*material->refract) * material->refract;
                    // }
                }
            }
        }
    }
	
	

    return color;
}

bool Scene::traceShadow(const Ray &ray, double lightDistance){
    // Find hit object and distance
    Hit min_hit(lightDistance,ray.D);
    for (unsigned int i = 0; i < objects.size(); ++i) {
        Hit hit(objects[i]->intersect(ray));
        if (hit.t<min_hit.t && hit.t>1e-10) {
            return true;
        }
    }

    return false;
}


void Scene::render(Image &img)
{
    //for (unsigned int i = 0; i < objects.size(); ++i) {
    //    if (objects[i]->objType() == "sphere") {
    //        ((Sphere*)objects[i])->rotateSystem();
    //        // ((Sphere*)objects[i])->setArctic(Triple(0, 0, 0));
    //        ((Sphere*)objects[i])->arctic.x = 0;
    //    }
    //}

    Triple offset(0.0, 0.0, 0.0);
    int n = superSampling;
    
    double goldenAngle = camera.apertureSamples * 137.508;  // this is still in degrees
    double c = (double)camera.apertureRadius / (camera.up.length() * sqrt(camera.apertureSamples));
    vector<Image> imgSamples;
    for (int i = 0; i < camera.viewWidth; i++) {
        for (int j = 0; j < camera.viewHeight; j++) {
            img(i, j) = Triple(0.0, 0.0, 0.0);
        }
    }
    for (int sample = 0; sample < camera.apertureSamples; sample++) {
        //Image newSample(camera.viewWidth, camera.viewHeight);
        double r = c * sqrt((double)sample);
        double th = sample * goldenAngle;
        offset = Triple(r * cos(th / 360), r * sin(th / 360), 0.0);
        camera.eye = camera.eye + offset;
        //// calculation for this sample
        // camera setup
        Vector look = camera.center - camera.eye;
        Vector horizontal = look.cross(camera.up);
        Vector orthoUp = horizontal.cross(look.normalized()).normalized() * camera.up.length() * camera.viewHeight / 2; // this will define half-long the vertical size of the view
                                                // because we do not have a viewing angle implemented
        horizontal = horizontal.normalized() * orthoUp.length() * camera.viewWidth / camera.viewHeight;
        double sx(0.0), sy(0.0);
        double pxHeight(1.0 / camera.viewHeight);
        double pxWidth(1.0 / camera.viewWidth);

        for (int SX = 0; SX < camera.viewWidth; SX++) {
            sx = SX * pxWidth;
            for (int SY = 0; SY < camera.viewHeight; SY++) {
                sy = SY * pxHeight;
                Color col(0.0, 0.0, 0.0);
                for (int i = 0; i < n; i++) {
                    for (int j = 0; j < n; j++) {
                        Point pixel = camera.center + (2 * sx - 1.0 + pxWidth / (2 * n) + i * pxWidth / n) * horizontal
                            + (1.0 - 2 * sy + pxHeight / (2 * n) + j * pxHeight / n) * orthoUp;
                        //(x + sx / (2 * n) + i * sx / n, h - 1 - y + 1.0 / (2 * n) + j * 1.0 / n, 0);
                        Ray ray(camera.eye, (pixel - camera.eye).normalized());
                        Color colp = trace(ray);
                        col += colp;
                    }
                }
                col = col / (n * n);
                col.clamp();
                img(SX, SY) += col;
            }
        }
        //// done calculation for this sample
        //imgSamples.push_back(newSample);
    }

    
        
    for (int SX = 0; SX < camera.viewWidth; SX++) {
        for (int SY = 0; SY < camera.viewHeight; SY++) {
            /*Color px(0.0, 0.0, 0.0);
            for (int sample = 0; sample < camera.apertureSamples; sample++) {
                px += imgSamples.at(sample)(SX, SY);
            }*/
            img(SX, SY) /= camera.apertureSamples;
        }
    }
}

void Scene::setMode(string s) {
    mode = s;
    std::cout << "render mode : " << mode << std::endl;
}

void Scene::setGoochParameters(GoochParameters g){
    goochParameters = g;
    std::cout << "Gooch parameters: b=" << goochParameters.b << " y=" << goochParameters.y << " alpha=" << goochParameters.alpha << " beta=" << goochParameters.beta << std::endl;
}

void Scene::setCamera(Camera c)
{
    camera = c;
    std::cout << "eye : " << camera.eye.x << " " << camera.eye.y << " " << camera.eye.z << std::endl;
    std::cout << "center : " << camera.center.x << " " << camera.center.y << " " << camera.center.z << std::endl;
    std::cout << "up : " << camera.up.x << " " << camera.up.y << " " << camera.up.z << std::endl;
    std::cout << "viewSize : " << camera.viewWidth << " " << camera.viewHeight << std::endl;
    std::cout << "apertureRadius : " << camera.apertureRadius << std::endl;
    std::cout << "apertureSamples : " << camera.apertureSamples << std::endl;
}

void Scene::setShadows(bool s) {
    shadows = s;
}

void Scene::setRecursionDepth(int d) {
    maxRecursionDepth = d;
}

void Scene::setSuperSampling(int s){
    superSampling = s;
    std::cout << "SuperSampling factor : " << superSampling << std::endl;
}

void Scene::addObject(Object *o)
{
    objects.push_back(o);
}

void Scene::addLight(Light *l)
{
    lights.push_back(l);
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
    int pivot = (objects[high])->getZPos();
    
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if ((objects[j])->getZPos() < pivot) {
            i++;
    //         std::cout << "-----------" << std::endl;
    // for (int i = 0; i < objects.size(); i++) {
    //     std::cout << (objects[i])->getZPos() << std::endl;
    // }
            // cout << (objects[i])->getZPos() << " " << (objects[j])->getZPos() << std::endl;
            std::swap(objects[i], objects[j]);
            // std::cout << "-----------" << std::endl;
    // for (int i = 0; i < objects.size(); i++) {
    //     std::cout << (objects[i])->getZPos() << std::endl;
    // }
    //         cout << (objects[i])->getZPos() << " " << (objects[j])->getZPos() << std::endl;
        }
    }
    std::swap(objects[i + 1], objects[high]);
    return (i + 1);
}

void Scene::sortZBuffer() {
    sortObjects(objects, 0, objects.size() - 1);
    // std::cout << "-----------" << std::endl;
    // for (int i = 0; i < objects.size(); i++) {
    //     std::cout << (objects[i])->getZPos() << std::endl;
    // }
}

int Scene::getCamHeight() {
    return camera.viewHeight;
}

int Scene::getCamWidth() {
    return camera.viewWidth;
}
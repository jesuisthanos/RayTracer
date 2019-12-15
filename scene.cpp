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

        // Recursion
        if(recursionDepth < maxRecursionDepth && contribution*material->ks >= 0.1){
            Ray reflectRay(hit, ray.D - 2*N.dot(ray.D)*N);
            color += trace(reflectRay, recursionDepth + 1, contribution*material->ks) * material->ks;
        }

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
    Triple offset(0.0, 0.0, 0.0);
    int n = superSampling;
    
    double goldenAngle = camera.apertureSamples * 137.508;  // this is still in degrees
    double c = (double)camera.apertureRadius / (camera.up.length() * sqrt(camera.apertureSamples));
    vector<Image> imgSamples;
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

    /*
    Triple offset(0.0, 0.0, 0.0);
    int n = superSampling;
    
    double goldenAngle = camera.apertureSamples * 137.508;  // this is still in degrees
    double goldenRatio = (1.0 + sqrt(5)) / 2.0;
    double c = (double)camera.apertureRadius / (camera.up.length() * sqrt(camera.apertureSamples));
    int apertureOffset = camera.viewWidth * camera.viewHeight * n * n - 1;
    //for (int sample = 0; sample < camera.apertureSamples; sample++) {
    //    //Image newSample(camera.viewWidth, camera.viewHeight);
    //    double r = c * sqrt((double)sample);// -fmod(apertureOffset * goldenRatio, 1.0);
    //    double th = sample * goldenAngle;// -(double)apertureOffset;
    //    offset = Triple(r * cos(th / 360), r * sin(th / 360), 0.0);
    //    Triple thiseye = camera.eye + offset;
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
                        Color colA(0.0, 0.0, 0.0);
                        Point pixel = camera.center + (2 * sx - 1.0 + pxWidth / (2 * n) + i * pxWidth / n) * horizontal
                            + (1.0 - 2 * sy + pxHeight / (2 * n) + j * pxHeight / n) * orthoUp;
                        int px_idx = (camera.viewHeight - 1 - SY) * n * camera.viewWidth + SX * n + j;
                        for (int sample = 0; sample < camera.apertureSamples; sample++) {
                            double r = c * sqrt((double)sample) -fmod(px_idx * goldenRatio, 1.0);
                            double th = sample * goldenAngle -(double)px_idx;
                            offset = Triple(r * cos(th / 360), r * sin(th / 360), 0.0);
                            Triple thiseye = camera.eye + offset;
                            Ray ray(thiseye, (pixel - thiseye).normalized());
                            Color colp = trace(ray);
                            colA += colp;
                        }
                        colA = colA / camera.apertureSamples;
                        col += colA;
                        //Ray ray(thiseye, (pixel - thiseye).normalized());
                        //Color colp = trace(ray);
                        //col += colp;
                    }
                }
                col = col / (n * n);
                col.clamp();
                img(SX, SY) = col;
            }
        //}
    }
    */

    
    //// camera setup
    //Vector look = camera.center - camera.eye;
    //Vector horizontal = look.cross(camera.up);
    //Vector orthoUp = horizontal.cross(look.normalized()).normalized() * camera.viewHeight / 2; // this will define half-long the vertical size of the view
    //                                        // because we do not have a viewing angle implemented
    //horizontal = horizontal.normalized() * orthoUp.length() * camera.viewWidth / camera.viewHeight;
    //double sx(0.0), sy(0.0);
    //double pxHeight(1.0 / camera.viewHeight);
    //double pxWidth(1.0 / camera.viewWidth);

    //for (int SX = 0; SX < camera.viewWidth; SX++) {
    //    sx = SX * pxWidth;
    //    for (int SY = 0; SY < camera.viewHeight; SY++) {
    //        sy = SY * pxHeight;
    //        Color col(0.0, 0.0, 0.0);
    //        for (int i = 0; i < n; i++) {
    //            for (int j = 0; j < n; j++) {
    //                Point pixel = camera.center + (2 * sx - 1.0 + pxWidth / (2 * n) + i * pxWidth / n) * horizontal 
    //                    + (1.0 - 2 * sy + pxHeight / (2 * n) + j * pxHeight / n) * orthoUp;
    //                //(x + sx / (2 * n) + i * sx / n, h - 1 - y + 1.0 / (2 * n) + j * 1.0 / n, 0);
    //                Ray ray(camera.eye, (pixel - camera.eye).normalized());
    //                Color colp = trace(ray);
    //                col += colp;
    //            }
    //        }
    //        col = col / (n * n);
    //        col.clamp();
    //        img(SX, SY) = col;
    //    }
    //}

    
    //int w = img.width();
    //int h = img.height();
    //for (int y = 0; y < h; y++) {
    //    for (int x = 0; x < w; x++) {
    //    // for (int z = 0; z < w; z++) {
    //        Color col(0.0, 0.0, 0.0);
    //        for (int i = 0; i < n; i++) {
    //            for (int j = 0; j < n; j++) {
    //                Point pixel(x+1.0/(2*n)+i*1.0/n, h-1-y+1.0/(2*n) + j * 1.0/n, 0);
    //                Ray ray(camera.eye, (pixel-camera.eye).normalized());
    //                Color colp = trace(ray);
    //                col += colp;
    //            }
    //        }
    //        col = col / (n * n);
    //        col.clamp();
    //        img(x, y) = col;
    //        // Point pixel(x+0.5, h-1-y+0.5, 0);
    //        // Point pixel(0, h-1-y+0.5, w-1-z+0.5);
    //        // Ray ray(eye, (pixel-eye).normalized());
    //        // Color col = trace(ray);
    //        // col.clamp();
    //        // img(x,y) = col;
    //        // img(z,y) = col;
    //    }
    //}
}

void Scene::addObject(Object *o)
{
    objects.push_back(o);
}

void Scene::addLight(Light *l)
{
    lights.push_back(l);
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

void Scene::setMode(string s) {
    mode = s;
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

int Scene::getCamHeight() {
    return camera.viewHeight;
}

int Scene::getCamWidth() {
    return camera.viewWidth;
}
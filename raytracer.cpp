//
//  Framework for a raytracer
//  File: raytracer.cpp
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Author: Maarten Everts
//    Stephane Gosset
//    Sy-Thanh Ho
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#include "raytracer.h"
#include "object.h"
#include "sphere.h"
#include "triangle.h"
#include "material.h"
#include "light.h"
#include "image.h"
#include "camera.h"
#include "yaml/yaml.h"
#include <ctype.h>
#include <fstream>
#include <assert.h>

// Functions to ease reading from YAML input
void operator >> (const YAML::Node& node, Triple& t);
Triple parseTriple(const YAML::Node& node);

void operator >> (const YAML::Node& node, Triple& t)
{
    assert(node.size()==3);
    node[0] >> t.x;
    node[1] >> t.y;
    node[2] >> t.z;
}

Triple parseTriple(const YAML::Node& node)
{
    Triple t;
    node[0] >> t.x;
    node[1] >> t.y;
    node[2] >> t.z;	
    return t;
}

Camera parseCamera(const YAML::Node& node)
{
    Camera c;
    node["eye"] >> c.eye;
    node["center"] >> c.center;
    node["up"] >> c.up;
    node["viewSize"][0] >> c.viewWidth;
    node["viewSize"][1] >> c.viewHeight;
    try {
        node["apertureRadius"] >> c.apertureRadius;
        node["apertureSamples"] >> c.apertureSamples;
    }
    catch (exception e) {
        c.apertureRadius = 0;
        c.apertureSamples = 1;
    }
    return c;
}

Material* Raytracer::parseMaterial(const YAML::Node& node)
{
    Material *m = new Material();
    try{
    node["color"] >> m->color;
    } catch (exception e) {
        m->color = Color(1, 1, 1);
    }
    try{
    node["color"] >> m->color;
    } catch (exception e) {
        m->color = Color(1, 1, 1);
    }
    try{
        string textureName = node["texture"];
        try{
            std::cout << "Loading texture : " << textureName << std::endl;
            m->texture = new Image(textureName.c_str());
            m->hasTexture = true;
            std::cout << "Texture " << textureName << " loaded" << std::endl << std::endl;
        } catch (exception e) {
            std::cerr << "ERROR: Failed to load texture " << textureName << std::endl <<std::endl;
        }
    } catch (exception e) {
        //no texture to load
    }
    node["ka"] >> m->ka;
    node["kd"] >> m->kd;
    node["ks"] >> m->ks;
    node["n"] >> m->n;
    try{
        node["refract"] >> m->refract;
        node["eta"] >> m->eta;
    }catch(exception e){
        m->refract = 0;
        m->eta = 0;
    }
    return m;
}

Object* Raytracer::parseObject(const YAML::Node& node)
{
    Object *returnObject = NULL;
    std::string objectType;
    node["type"] >> objectType;

    if (objectType == "sphere") {
        Point pos;
        node["position"] >> pos;
        double r;
        node["radius"] >> r;
        Triple arctic;
        Triple greenwich;
        try{
            node["arctic"] >> arctic;
            node["greenwich"] >> greenwich;
        } catch(exception e){
            arctic = Triple(0, 1, 0);
            greenwich = Triple(0, 1, 1);
        }
        Sphere *sphere = new Sphere(pos,r,arctic,greenwich);
        returnObject = sphere;
    }

	if (objectType == "triangle") {
		Point v1, v2, v3;
        bool rightSide;
		node["vertex1"] >> v1;
		node["vertex2"] >> v2;
		node["vertex3"] >> v3;
        try{
            rightSide = (node["rightSide"] == "true");
        }catch(exception e){
            rightSide = false;
        }
		Triangle *t = new Triangle(v1, v2, v3, rightSide);
		returnObject = t;
	}

    if (returnObject) {
        // read the material and attach to object
        returnObject->material = parseMaterial(node["material"]);
    }

    return returnObject;
}

Light* Raytracer::parseLight(const YAML::Node& node)
{
    Point position;
    node["position"] >> position;
    Color color;
    node["color"] >> color;
    return new Light(position,color);
}

/*
* Read a scene from file
*/

bool Raytracer::readScene(const std::string& inputFilename)
{
    // Initialize a new scene
    scene = new Scene();

    // Open file stream for reading and have the YAML module parse it
    std::ifstream fin(inputFilename.c_str());
    if (!fin) {
        cerr << "Error: unable to open " << inputFilename << " for reading." << endl;;
        return false;
    }
    try {
        YAML::Parser parser(fin);
        if (parser) {
            YAML::Node doc;
            parser.GetNextDocument(doc);

            // Read scene configuration options

            // Render mode
            try {
                string renderMode = doc["RenderMode"];
                scene->setMode(renderMode);
            } catch (exception e) {
                // Render mode not specified, default to phong
                scene->setMode("phong");
            }
            
            // Shadows
            try {
                string shadows = doc["Shadows"];
                scene->setShadows(shadows == "true");
            } catch (exception e) {
                // Shadows not specified, don't use them.
                scene->setShadows(false);
            }

            // Max recursion depth
            try {
                int maxRecursionDepth = doc["MaxRecursionDepth"];
                scene->setRecursionDepth(maxRecursionDepth);
            } catch (exception e) {
                // Recursion depth not specified, no recursion.
                scene->setRecursionDepth(0);
            }

            // Super-sampling
            try{
                int superSampling = doc["SuperSampling"]["factor"];
                scene->setSuperSampling(superSampling);
            } catch (exception e) {
                scene->setSuperSampling(1);
            }

            try{
                // Camera parameters
                scene->setCamera(parseCamera(doc["Camera"]));
            } catch (exception e){
                // Eye position
                Camera camera;
                camera.eye = parseTriple(doc["Eye"]);
                scene->setCamera(camera);
            }


            // Read and parse the scene objects
            const YAML::Node& sceneObjects = doc["Objects"];
            if (sceneObjects.GetType() != YAML::CT_SEQUENCE) {
                cerr << "Error: expected a sequence of objects." << endl;
                return false;
            }
            for(YAML::Iterator it=sceneObjects.begin();it!=sceneObjects.end();++it) {
                Object *obj = parseObject(*it);
                // Only add object if it is recognized
                if (obj) {
                    scene->addObject(obj);
                } else {
                    cerr << "Warning: found object of unknown type, ignored." << endl;
                }
            }


            // Read and parse light definitions
            const YAML::Node& sceneLights = doc["Lights"];
            if (sceneObjects.GetType() != YAML::CT_SEQUENCE) {
                cerr << "Error: expected a sequence of lights." << endl;
                return false;
            }
            for(YAML::Iterator it=sceneLights.begin();it!=sceneLights.end();++it) {
                scene->addLight(parseLight(*it));
            }
        }
        if (parser) {
            cerr << "Warning: unexpected YAML document, ignored." << endl;
        }
    } catch(YAML::ParserException& e) {
        std::cerr << "Error at line " << e.mark.line + 1 << ", col " << e.mark.column + 1 << ": " << e.msg << std::endl;
        return false;
    }

    cout << "YAML parsing results: " << scene->getNumObjects() << " objects read." << endl;
    return true;
}

void Raytracer::renderToFile(const std::string& outputFilename)
{
    scene->sortZBuffer();

    // updated version of img initialization instead of dummy (400,400) which is
    // incompatible with newer dynamic sizes
    Image img(scene->getCamWidth(),scene->getCamHeight());
    cout << "Tracing..." << endl;
    scene->render(img);
    cout << "Writing image to " << outputFilename << "..." << endl;
    img.write_png(outputFilename.c_str());
    cout << "Done." << endl;
}

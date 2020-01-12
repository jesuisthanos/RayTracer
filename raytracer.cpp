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
#include "GoochParameters.h"
#include "camera.h"
#include "object.h"
#include "sphere.h"
#include "triangle.h"
#include "cone.h"
#include "circle.h"
#include "material.h"
#include "glm.h"
#include "image.h"
#include "light.h"
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

GoochParameters Raytracer::parseGooch(const YAML::Node& node){
    GoochParameters g;
    node["b"] >> g.b;
    node["y"] >> g.y;
    node["alpha"] >> g.alpha;
    node["beta"] >> g.beta;
    try {
        g.outline = (node["outline"] == "true");
    } catch (exception e) {
        g.outline = false;
    }
    return g;
}

Camera Raytracer::parseCamera(const YAML::Node& node)
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

Triple Raytracer::parseTriple(const YAML::Node& node)
{
    Triple t;
    node[0] >> t.x;
    node[1] >> t.y;
    node[2] >> t.z;	
    return t;
}

Material* Raytracer::parseMaterial(const YAML::Node& node)
{
    Material *m = new Material();
    try{
    node["color"] >> m->color;
    } catch (exception e) {
        m->color = Color(1, 1, 1);
    }

    // load texture
    try{
        string textureName = node["texture"];
        try{
            bool found = false;
            unsigned int i = 0;
            // check if the texture is already loaded
            while(!found && i < textureNames.size()){
                found = (textureNames.at(i) == textureName);
                i++;
            }
            if(found){
                m->texture = textures.at(i-1);
            }else{
                std::cout << std::endl << "Loading texture : " << textureName << std::endl;
                textures.push_back(new Image(textureName.c_str()));
                textureNames.push_back(textureName);
                m->texture = textures.back();
                if(m->texture->size() > 0)
                    std::cout << "Texture " << textureName << " loaded (" << m->texture->width() << "x" << m->texture->height() << ")" << std::endl << std::endl;
                else
                    std::cerr << "ERROR: texture " << textureName << " missing or empty" << std::endl <<std::endl;
            }
            m->textured = (m->texture->size() > 0);
        } catch (exception e) {
            std::cerr << "ERROR: Failed to load texture " << textureName << std::endl <<std::endl;
        }
    } catch (exception e) {
        // no texture to load
        m->textured = false;
    }

    // load bump map
    try{
        string textureName = node["bumpmap"];
        try{
            bool found = false;
            unsigned int i = 0;
            // check if the bump map is already loaded
            while(!found && i < textureNames.size()){
                found = (textureNames.at(i) == textureName);
                i++;
            }
            if(found){
                m->bumpmap = textures.at(i-1);
            }else{
                std::cout << std::endl << "Loading bump map : " << textureName << std::endl;
                textures.push_back(new Image(textureName.c_str()));
                textureNames.push_back(textureName);
                m->bumpmap = textures.back();
                if(m->bumpmap->size() > 0)
                    std::cout << "Bump map " << textureName << " loaded (" << m->bumpmap->width() << "x" << m->bumpmap->height() << ")" << std::endl << std::endl;
                else
                    std::cerr << "ERROR: Bump map " << textureName << " missing or empty" << std::endl <<std::endl;
            }
            m->bumpmapped = (m->bumpmap->size() > 0);
        } catch (exception e) {
            std::cerr << "ERROR: Failed to load bump map " << textureName << std::endl <<std::endl;
        }
    } catch (exception e) {
        // no texture to load
        m->bumpmapped = false;
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
        Vector rotationVect;
        double rotationAngle;
        Sphere* sphere;
        try{
            node["arctic"] >> arctic;
            node["greenwich"] >> greenwich;
        } catch(exception e){
            arctic = Triple(0, 1, 0);
            greenwich = Triple(0, 1, 1);
        }
        try {
            node["rotationVect"] >> rotationVect;
            node["rotationAngle"] >> rotationAngle;
            sphere = new Sphere(pos, r, arctic, greenwich, rotationVect, rotationAngle);
            sphere->rotateSystem();
        }
        catch (exception e) {
            sphere = new Sphere(pos, r, arctic, greenwich);
            // sphere->rotateSystem();
        }
        //Sphere *sphere = new Sphere(pos,r,arctic,greenwich);
        // sphere->rotateSystem();
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

    if (objectType == "cone"){
        Point base, top;
        double radius;
        node["base"] >> base;
        node["top"] >> top;
        node["radius"] >> radius;
        Vector org;
        try {
            org = node["org"];
        }
        catch (exception e) {
            org = Vector(0, 0, 1);
        }
        Cone* c = new Cone(base, top, radius, org);

        std::cout << "Cone" << std::endl;
        std::cout << c->base.x << " " << c->base.y << " " << c->base.z << endl;
        std::cout << c->top.x << " " << c->top.y << " " << c->top.z << endl;
        std::cout << c->radius << endl;
        returnObject = c;
    }

    if (objectType == "circle") {
        Point center;
        double radius;
        Vector norm;
        node["center"] >> center;
        node["radius"] >> radius;
        node["norm"] >> norm;
        Circle* c = new Circle(center, radius, norm);

        std::cout << "Circle" << std::endl;
        std::cout << c->center.x << " " << c->center.y << " " << c->center.z << endl;
        std::cout << c->radius << endl;
        std::cout << c->norm.x << " " << c->norm.y << " " << c->norm.z << endl;
        returnObject = c;
    }

    if (returnObject) {
        // read the material and attach to object
        returnObject->material = parseMaterial(node["material"]);
    }

    return returnObject;
}

bool Raytracer::parseModel(const YAML::Node& node){
    string file;
    node["file"] >> file;
    std::cout << std::endl << "Loading model : " << file << std::endl;
    GLMmodel *model = glmReadOBJ((char*)file.c_str());
    for(int i = 0; i < model->numtriangles; i++){
        float v1x = model->vertices[3*model->triangles[i].vindices[0]];
        float v1y = model->vertices[3*model->triangles[i].vindices[0] + 1];
        float v1z = model->vertices[3*model->triangles[i].vindices[0] + 2];
        float v2x = model->vertices[3*model->triangles[i].vindices[1]];
        float v2y = model->vertices[3*model->triangles[i].vindices[1] + 1];
        float v2z = model->vertices[3*model->triangles[i].vindices[1] + 2];
        float v3x = model->vertices[3*model->triangles[i].vindices[2]];
        float v3y = model->vertices[3*model->triangles[i].vindices[2] + 1];
        float v3z = model->vertices[3*model->triangles[i].vindices[2] + 2];

        Point v1 = Triple(v1x*50 + 175, v1y*50 + 100, v1z*50 + 200);
        Point v2 = Triple(v2x*50 + 175, v2y*50 + 100, v2z*50 + 200);
        Point v3 = Triple(v3x*50 + 175, v3y*50 + 100, v3z*50 + 200);
        
        std::cout << v1.x << " " << v1.y << " " << v1.z << "; ";
        std::cout << v2.x << " " << v2.y << " " << v2.z << "; ";
        std::cout << v3.x << " " << v3.y << " " << v3.z << std::endl;

        Triangle *t = new Triangle(v1, v2, v3, false);
        t->material = new Material();
        t->material->color = Color(1, 1, 1);
        t->material->kd = 1.0;
        scene->addObject(t);
    }
    return true;
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

            // Gooch parameters
            try {
                scene->setGoochParameters(parseGooch(doc["GoochParameters"]));
            } catch (exception e) {
                if (scene->getMode() == "gooch"){
                    std::cerr << "ERROR: Gooch parameters missing or invalid, using default values instead." << std::endl;
                    GoochParameters goochParameters;
                    goochParameters.b = 0.55;
                    goochParameters.y = 0.3;
                    goochParameters.alpha = 0.25;
                    goochParameters.beta = 0.5;
                    scene->setGoochParameters(goochParameters);
                }
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
                // superSampling not specified, no superSampling.
                scene->setSuperSampling(1);
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

            // Read and parse the scene models
            try{
                const YAML::Node& sceneModels = doc["Models"];
                if (sceneModels.GetType() != YAML::CT_SEQUENCE) {
                    cerr << "Error: expected a sequence of Models." << endl;
                    return false;
                }
                for(YAML::Iterator it=sceneModels.begin();it!=sceneModels.end();++it) {
                    bool obj = parseModel(*it);
                    // Only add object if it is recognized
                    if (obj) {
                        //scene->addObject(obj);
                    } else {
                        cerr << "Warning: found object of unknown type, ignored." << endl;
                    }
                }
            }catch (exception e){
                // No models
            }

            // Read and parse light definitions
            const YAML::Node& sceneLights = doc["Lights"];
            if (sceneLights.GetType() != YAML::CT_SEQUENCE) {
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

    cout << endl << "YAML parsing results: " << endl << scene->getNumObjects() << " objects and " << scene->getNumLights() << " lights read." << endl;
    cout << textures.size() << " textures loaded." << endl << endl;
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
    if(scene->getMode() == "gooch" && scene->getGoochParameters().outline){
        
		// outline from zbuffer
        Image zbuffer(scene->getCamWidth(),scene->getCamHeight());
        scene->setMode("zbuffer");
        scene->render(zbuffer);
        for(int y = 0; y < scene->getCamHeight(); y++){
            for(int x = 0; x < scene->getCamWidth(); x++){
				Color diff = Triple(0, 0, 0);
                if(y > 0){
                    if(x > 0) diff += 1 * zbuffer.get_pixel(x-1, y-1);
                    diff += 2 * zbuffer.get_pixel(x, y-1);
                    if(x < scene->getCamWidth()-1) diff += 1 * zbuffer.get_pixel(x+1, y-1);
                }
                if(x > 0) diff += 2 * zbuffer.get_pixel(x-1, y);
                diff -= 12 * zbuffer.get_pixel(x, y);
                if(x < scene->getCamWidth()-1) diff += 2 * zbuffer.get_pixel(x+1, y);
                if(y < scene->getCamHeight()-1){
                    if(x > 0) diff += 1 * zbuffer.get_pixel(x-1, y+1);
                    diff += 2 * zbuffer.get_pixel(x, y+1);
                    if(x < scene->getCamWidth()-1) diff += 1 * zbuffer.get_pixel(x+1, y+1);
                }
				if(diff.x < -0.1 || diff.y < -0.1 || diff.z < -0.1 || diff.x > 0.1 || diff.y > 0.1 || diff.z > 0.1)
					img.put_pixel(x, y, Triple(0, 0, 0));
            }
        }
        
		// outline from normal buffer
		Image normalBuffer(scene->getCamWidth(), scene->getCamHeight());
		scene->setMode("normal");
		scene->render(normalBuffer);
		for (int y = 0; y < scene->getCamHeight(); y++) {
			for (int x = 0; x < scene->getCamWidth(); x++) {
				Color diff = Triple(0, 0, 0);
				if (y > 0) {
					if (x > 0) diff += 1 * normalBuffer.get_pixel(x - 1, y - 1);
					diff += 2 * normalBuffer.get_pixel(x, y - 1);
					if (x < scene->getCamWidth() - 1) diff += 1 * normalBuffer.get_pixel(x + 1, y - 1);
				}
				if (x > 0) diff += 2 * normalBuffer.get_pixel(x - 1, y);
				diff -= 12 * normalBuffer.get_pixel(x, y);
				if (x < scene->getCamWidth() - 1) diff += 2 * normalBuffer.get_pixel(x + 1, y);
				if (y < scene->getCamHeight() - 1) {
					if (x > 0) diff += 1 * normalBuffer.get_pixel(x - 1, y + 1);
					diff += 2 * normalBuffer.get_pixel(x, y + 1);
					if (x < scene->getCamWidth() - 1) diff += 1 * normalBuffer.get_pixel(x + 1, y + 1);
				}
				if(diff.x < -0.1 || diff.y < -0.1 || diff.z < -0.1 || diff.x > 0.1 || diff.y > 0.1 || diff.z > 0.1)
					img.put_pixel(x, y, Triple(0, 0, 0));
			}
		}
        
    }
    cout << "Writing image to " << outputFilename << "..." << endl;
    img.write_png(outputFilename.c_str());
    cout << "Done." << endl;
}

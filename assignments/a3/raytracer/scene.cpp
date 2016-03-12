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
    double diffuse;
    Point LPos;
    Point LColor;
    Point L;
    double specular = 0;
    bool isInShadow = false;
    Color color;
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
    *        double*Color       scales each color component (r,g,b)
    *        Color*Color        dito
    *        pow(a,b)           a to the power of b
    ****************************************************/
    for(Light* CurrLight : lights){
        LPos += CurrLight->position;
        LColor += CurrLight->color;
    }
    Ray shadows = Ray(hit, LPos);
    
    Point LDir = (LPos - hit);
    LDir.normalize();

   // Ray* temp = new Ray();

    for (unsigned int i = 0; i < objects.size(); ++i) {
        Hit hit(objects[i]->intersect(shadows));
        if (hit.t<min_hit.t) {
                  isInShadow = true; 
            break;
        } 
    }

    if (isInShadow == true)
    { 
       color = ((material->ka*material->color) + (material->color*LColor*material->kd) + (LColor*material->ks)) ;// place holder
    } else {
        L = (LPos+LColor).normalized();
        diffuse = max(N.dot(L), 0.0);

        if (diffuse > 0.0){
            specular = pow(max(0.0, N.dot(L)), material->n);
        }

       color = ((material->ka*material->color) + (diffuse*material->color*LColor*material->kd) + (LColor*specular*material->ks)) ;// place holder
       color.clamp();
   }
   return color;
}

void Scene::render(Image &img)
{
    int w = img.width();
    int h = img.height();
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Point pixel(x, h-1-y, 0);
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

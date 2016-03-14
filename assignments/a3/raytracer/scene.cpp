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
    Hit min_hit(std::numeric_limits<double>::infinity(), Vector());
    Object *obj = NULL;
    for (unsigned int i = 0; i < objects.size(); ++i) {
        Hit hit(objects[i]->intersect(ray));
        if (hit.t < min_hit.t) {
            min_hit = hit;
            obj = objects[i];
        }
    }

    // No hit? Return background color.
    if (!obj) return Color(0.0, 0.0, 0.0);

    Material *material = obj->material;            //the hit objects material
    Vector N = min_hit.N;                          //the normal at hit point
    Point hit = ray.at(min_hit.t);                 //the hit point from eye
    Vector V = -ray.D;                             //the view vector (eye)
    double diffuse = 0;
    Point light_position;
    Point light_colour;
    Point eye_distance;
    Point L;
    Point light_direction;
    double specular = 0;
    Color color;
    bool in_shadow = true;

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
    for (Light* CurrLight : lights) {
        light_position = CurrLight->position;
        light_colour = CurrLight->color;

        light_direction = (hit - light_position);//.normalized();

        L = (light_position + light_colour).normalized();
        diffuse = max(N.dot(L), 0.0);

        if (diffuse > 0.0) {
            specular = pow(max(0.0, N.dot(L)), material->n);
        }

        Ray shadow_ray(hit, light_direction);
        // Ray shadow_ray(light_position, shadow_ray.at(pow(2, -32)));
        Hit shadow_min_hit(std::numeric_limits<double>::infinity(), Vector());
        Object *min_obj = NULL;
        for (int j = 0; j < objects.size(); ++j) {
            Hit shadow_hit(objects[j]->intersect(shadow_ray));
            if (shadow_hit.t < shadow_min_hit.t) {
                shadow_min_hit = shadow_hit;
                min_obj = objects[j];
            }
        }

        if (min_obj != obj) {
            in_shadow = true;
        } else {
            in_shadow = false;
        }


    if (in_shadow == false) {
        color += ((material->ka * material->color) + (diffuse * material->color * light_colour * material->kd) + (light_colour * specular * material->ks));
    } else {
        color += (material->ka * material->color);
    }
}
    color.clamp();
    return color;
}

void Scene::render(Image & img)
{
    int w = img.width();
    int h = img.height();
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Point pixel(x, h - 1 - y, 0);
            Ray ray(eye, (pixel - eye).normalized());
            Color col = trace(ray);
            col.clamp();
            img(x, y) = col;
        }
    }
}

void Scene::addObject(Object * o)
{
    objects.push_back(o);
}

void Scene::addLight(Light * l)
{
    lights.push_back(l);
}

void Scene::setEye(Triple e)
{
    eye = e;
}

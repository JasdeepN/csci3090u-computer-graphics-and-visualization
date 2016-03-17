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


Color Scene::trace(const Ray &ray, int reflection_count, int refraction_count, double prev_eta)
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

    Material *material = obj->material;          //the hit objects material
    Vector N = min_hit.N;                        //the normal at hit point
    Point hit = ray.at(min_hit.t);               //the hit point from eye
    Vector V = -ray.D;                           //the view vector (eye)
    double diffuse = 0;
    Point light_position;
    Color light_colour;
    Point L;
    Vector light_direction;
    Vector reflection_direction;
    Vector refraction_direction;                 //this is t
    double specular = 0;
    Color color;
    Color final_colour;
    Color reflection_colour;
    Color refraction_colour;
    Color specular_reflection_color;
    bool in_shadow = false;
    bool trace_reflections = true;               //toggle for reflections
    bool trace_shadows = true;                   //toggle for shadows
    bool trace_refractions = true;               //toggle for refractions
    Vector temp_normal;

    double curr_eta = material->eta;


    if (prev_eta == curr_eta) {
        curr_eta = 1.0;
        temp_normal = N;
    } else {
        temp_normal = -N;
    }

    /****************************************************
    * This is where you should insert the color
    * calculation (Phong model).
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
        in_shadow = false;
        light_position = CurrLight->position;
        light_colour = CurrLight->color;

        light_direction = (hit - light_position);//.normalized();

        L = (light_position + light_colour).normalized();
        diffuse = max(N.dot(L), 0.0);

        Vector R = -L + (2.0 * (L.dot(N)) * N);

        if (diffuse > 0.0) {
            specular = pow(max(0.0, R.dot(V)), (material->n));
        }

        Ray shadow_ray(hit, -light_direction.normalized());
        Point hit_shadow_jiggle = shadow_ray.at(pow(2, -32)); //do not change this line
        shadow_ray = Ray(hit_shadow_jiggle, -light_direction);

        Hit shadow_min_hit(std::numeric_limits<double>::infinity(), Vector());
        Object *min_obj = NULL;

        for (int j = 0; j <  objects.size(); ++j) {
            //shadow stuff
            Hit shadow_hit(objects[j]->intersect(shadow_ray));
            if (shadow_hit.t < shadow_min_hit.t) {
                shadow_min_hit = shadow_hit;
                min_obj = objects[j];
                if (shadow_min_hit.t >= 0) {
                    in_shadow = true;
                } else {
                    in_shadow = false;
                }
            }

            if (trace_shadows) {
                if (!in_shadow) { //not in shadow
                    color = ((material->ka * material->color) + (diffuse * material->color * light_colour * material->kd) + (light_colour * specular * material->ks));
                } else { //in shadow
                    color = (material->ka * material->color);//Color(0,0,0));
                }
            } else { //shadows off
                color = ((material->ka * material->color) + (diffuse * material->color * light_colour * material->kd) + (light_colour * specular * material->ks));
            }
            //end shadow stuff
        }
        final_colour += color;
    }

    //RECURSION (reflections)
    if (trace_reflections) {
        if (reflection_count < 5) {
            reflection_direction = (ray.D - (2 * (ray.D.dot(N)) * N)).normalized();

            Ray reflection_ray(hit, reflection_direction);

            // Point hit_jiggle = reflection_ray.at(pow(2, -10)); //do not change this
            Point hit_jiggle = reflection_ray.at(pow(2, -32));

            reflection_ray = Ray(hit_jiggle, reflection_ray.D);

            reflection_colour = (trace(reflection_ray, reflection_count + 1, refraction_count, prev_eta));
        }
    }
    //END reflections

    //REFRACTIONS
    if (trace_refractions) {
        if (material->refract > 0.0f) {

            Triple term1 = (-ray.D - temp_normal * (-ray.D.dot(temp_normal)));
            double term2 = (1 - pow(-ray.D.dot(temp_normal), 2));

            refraction_direction = ((prev_eta * term1) / curr_eta) - (temp_normal) * (sqrt(1 - ((pow(prev_eta, 2)) * term2) / pow(curr_eta, 2)));

            if (refraction_count < 5) {

                // refraction_direction = refraction_direction.normalized();

                Ray refraction_ray(hit, -refraction_direction);

                Point refrac_hit_jiggle = refraction_ray.at(pow(2, -32));

                refraction_ray = Ray(refrac_hit_jiggle, refraction_ray.D);

                refraction_colour = (trace(refraction_ray, reflection_count, refraction_count + 1, curr_eta));
            }
        }
    }
    // }
    //REFRACTIONS END
    final_colour += (refraction_colour * material->refract) + (reflection_colour * material->reflect) ;

    final_colour.clamp();
    return final_colour;
}

void Scene::render(Image & img)
{
    int w = img.width();
    int h = img.height();
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Point pixel(x, h - 1 - y, 0);
            int count = 0;
            Ray ray(eye, (pixel - eye).normalized());
            Color col = trace(ray, count, count, 1.0);
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

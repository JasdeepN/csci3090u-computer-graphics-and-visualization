//
//  Framework for a raytracer
//  File: sphere.cpp
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

#include "sphere.h"
#include <iostream>
#include <math.h>
#include "triple.h"

/************************** Sphere **********************************/

Hit Sphere::intersect(const Ray &ray)
{
    /****************************************************
    * RT1.1: INTERSECTION CALCULATION
    *
    * Given: ray, position, r
    * Sought: intersects? if true: *t
    *
    * Insert calculation of ray/sphere intersection here.
    *
    * You have the sphere's center (C) and radius (r) as well as
    * the ray's origin (ray.O) and direction (ray.D).
    *
    * If the ray does not intersect the sphere, return false.
    * Otherwise, return true and place the distance of the
    * intersection point from the ray origin in *t (see example).
    ****************************************************/

    // place holder for actual intersection calculation
    Triple c = position;
    Triple e = ray.O;

    double A = ray.D.dot(ray.D);
    double B = 2 * (ray.D.dot(e - c));
    double C = ((e - c).dot(e - c) - (r * r));
    float root;

    float discriminant = B * B + (-4.0f) * A * C;

    if (discriminant < 0)
    {
        return Hit::NO_HIT();
    }
    discriminant = sqrtf(discriminant);

    float root1 = ((-1 * B) + discriminant) / 2 * A;
    float root2 = ((-1 * B) - discriminant) / 2 * A;

    // if (root1 <= 0 && root2 <= 0) {
    //     return Hit::NO_HIT();
    // } else if (root1 <= 0 && root2 > 0) {
    //     root = root2;
    // } else if (root1 > 0 && root2 <= 0) {
    //     root = root1;
    // } else
    // {
    //     root = min(root1, root2);
    // }

    if (root1 <= 0 || root2 <= 0)
    {
        root = max(root1, root2);
    } else
    {
        root = min(root1, root2);
    }

    double t = sqrtf(A) * root;

    // std::cout << discriminant << std::endl;

    // Vector OC = (position - ray.O).normalized();
    // if (OC.dot(ray.D) < 0.999) {
    //     return Hit::NO_HIT();
    // }
    // double t = 1000;

    /****************************************************
    * RT1.2: NORMAL CALCULATION
    *
    * Given: t, C, r
    * Sought: N
    *
    * Insert calculation of the sphere's normal at the intersection point.
    ****************************************************/

    Vector H = ray.O + t * ray.D;
    // Vector H = ray.O * t + ray.D;

    Vector N = (H - c) / r;

    // Vector N = (C - ray.at(t));
    // N = N.normalized();
    N.normalize();

    //Vector N /* = ... */;

    return Hit(t, N);
}

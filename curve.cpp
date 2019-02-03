#include <iostream>
#include <cmath>
#include "curve.h"

using namespace std;

vec3 LineSegment::integrate(vec3 (*integrand)(vec3 s, vec3 ds), double dl)
{
    vec3 diff = this->b - this->a, sum = 0;

    double len = diff.magnitude();

    vec3 diffnorm = diff / len, s = this->a, ds = diffnorm * dl;

    double l;

    for(l = 0; l < len; l += dl, s += ds)
        sum += integrand(s, ds);

    if(l < len)
        sum += integrand(s, diffnorm * (len - l));

    return sum;
}

vec3 Arc::integrate(vec3 (*integrand)(vec3 s, vec3 ds), double dl)
{
    //cout << "Integrating loop of length " << this->angle << " radians" << endl;

    double r = this->radius.magnitude(), dtheta = dl / r;

    //cout << "R = " << r << ", dtheta = " << dtheta << endl;

    quat rot = quat::from_angleaxis(dtheta, normal), crot = rot.conjugate();

    //cout << "rot = " << rot << ", crot = " << crot << endl;

    double len = this->angle * r, l;

    vec3 sum = 0;

    quat radius = this->radius;

    for(l = 0; l < len; l += dl)
    {
        vec3 ds = this->normal.cross(radius).normalize() * dl;
        sum += integrand(this->center + radius, ds);

        radius = rot * radius * crot;
    }

    if(l < len)
    {
        dl = len - l;
        dtheta = dl / r;
        rot = quat::from_angleaxis(dtheta, normal);
        crot = rot.conjugate();

        vec3 ds = this->normal.cross(radius).normalize() * dl;
        sum += integrand(this->center + radius, ds);
    }

    return sum;
}

vec3 Spiral::integrate(vec3 (*integrand)(vec3 s, vec3 ds), double dl)
{
    //cout << "Integrating loop of length " << this->angle << " radians" << endl;

    double r = this->radius.magnitude(), dtheta = dl / r;

    //cout << "R = " << r << ", dtheta = " << dtheta << endl;

    quat rot = quat::from_angleaxis(dtheta, normal), crot = rot.conjugate();

    //cout << "rot = " << rot << ", crot = " << crot << endl;

    double len = this->angle * r, l;

    vec3 sum = 0;

    quat radius = this->radius;

    /* how far along the axis we've moved */
    vec3 axis = 0;

    /* we add this axial component each iteration */
    vec3 dp = this->normal * dtheta * pitch / (2 * M_PI);

    for(l = 0; l < len; l += dl, axis += dp)
    {
        vec3 ds = this->normal.cross(radius).normalize() * dl + dp;
        sum += integrand(this->origin + axis + radius, ds);

        /* rotate by dtheta about normal */
        radius = rot * radius * crot;
    }

    if(l < len)
    {
        dl = len - l;
        dtheta = dl / r;
        rot = quat::from_angleaxis(dtheta, normal);
        crot = rot.conjugate();
        dp = this->normal * dtheta * pitch / (2 * M_PI);

        vec3 ds = this->normal.cross(radius).normalize() * dl + dp;
        sum += integrand(this->origin + axis + radius, ds);
    }

    return sum;
}

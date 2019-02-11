#include <iostream>
#include <cmath>
#include "curve.h"

using namespace std;

vec3 LineSegment::integrate(vec3 (*integrand)(vec3 s, vec3 ds), scalar dl) const
{
    vec3 diff = this->b - this->a, sum = 0;

    scalar len = diff.magnitude();

    vec3 diffnorm = diff / len, s = this->a, ds = diffnorm * dl;

    scalar l;

    for(l = 0; l < len; l += dl, s += ds)
        sum += integrand(s, ds);

    if(l < len)
        sum += integrand(s, diffnorm * (len - l));

    return sum;
}

vec3 Arc::integrate(vec3 (*integrand)(vec3 s, vec3 ds), scalar dl) const
{
    //cout << "Integrating loop of length " << this->angle << " radians" << endl;

    scalar r = this->radius.magnitude(), dtheta = dl / r;

    //cout << "R = " << r << ", dtheta = " << dtheta << endl;

    quat rot = quat::from_angleaxis(dtheta, normal), crot = rot.conjugate();

    //cout << "rot = " << rot << ", crot = " << crot << endl;

    scalar len = this->angle * r, l;

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

vec3 Spiral::integrate(vec3 (*integrand)(vec3 s, vec3 ds), scalar dl) const
{
    //cout << "Integrating loop of length " << this->angle << " radians" << endl;

    scalar r = this->radius.magnitude(), dtheta = dl / r;

    //cout << "R = " << r << ", dtheta = " << dtheta << endl;

    quat rot = quat::from_angleaxis(dtheta, normal), crot = rot.conjugate();

    //cout << "rot = " << rot << ", crot = " << crot << endl;

    /* only flat (doesn't include stretched length) */
    scalar len = this->angle * r, l;

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

vec3 Toroid::integrate(vec3 (*integrand)(vec3 s, vec3 ds), scalar dl) const
{
    /* this applies to the minor rotation */
    scalar minor_r = this->minor_r;

    scalar major_r = this->major_radius.magnitude();

    /* how far parallel to the major median each dl takes us */
    scalar pitch_length = this->pitch * major_r;

    scalar dpar = this->pitch * dl / (2 * M_PI * sqrt(minor_r * minor_r + pitch_length * pitch_length)) * major_r;

    /* how far each dl rotates the major vector */
    scalar major_dtheta = dpar / major_r;

    /* how long perpendicular to the major median each dl takes us (from
     * Pythagoras) */
    scalar dper = sqrt(dl * dl - dpar * dpar);

    scalar minor_dtheta = dper / minor_r;

    /* minor normal is binormal to major normal and radius */
    vec3 minor_normal = this->major_normal.cross(this->major_radius).normalize();

    /* quaternion describing rotation of minor vector */
    quat minor_rot = quat::from_angleaxis(minor_dtheta, minor_normal), minor_crot = minor_rot.conjugate();

    quat major_rot = quat::from_angleaxis(major_dtheta, major_normal), major_crot = major_rot.conjugate();

    vec3 sum = 0;

    quat major_radius = this->major_radius;
    quat minor_radius = this->major_radius.normalize() * this->minor_r;

    //cout << "minor_dtheta = " << minor_dtheta << endl;
    //cout << "minor_dper = " << dper << endl;
    //cout << "dl = " << dl << endl;
    //cout << "dpar = " << dpar << endl;
    //cout << "pitch = " << this->pitch << endl;

    for(scalar theta = 0; theta < this->major_angle; theta += major_dtheta)
    {
        vec3 dpar_vec = minor_normal * dpar;
        vec3 ds = minor_normal.cross(minor_radius).normalize() * dper + dpar_vec;
        sum += integrand(this->origin + major_radius + minor_radius, ds);

        /* we need to rotate some vectors: first the major radius and
         * minor normal about the major normal, and then the minor
         * radius */
        /* we also need to rotate the minor radius rotation quaternion
         * about the origin */
        major_radius = major_rot * major_radius * major_crot;
        minor_normal = major_rot * minor_normal * major_crot;

        minor_radius = minor_rot * minor_radius * minor_crot;

        minor_rot = quat::from_angleaxis(minor_dtheta, minor_normal);
        minor_crot = minor_rot.conjugate();
        //minor_rot = major_rot * minor_rot;
        //minor_crot = minor_rot.conjugate();

        //cout << "Minor radius is " << minor_radius << endl;
        //cout << "Minor rot quat is " << minor_rot << endl;
    }

#if 0
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
#endif

    return sum;
}


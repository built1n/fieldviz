#include <iostream>
#include <cmath>
#include "surface.h"

vec3 Plane::integrate(vec3 (*integrand)(vec3 s, vec3 dA), scalar d) const
{
    vec3 sum = 0;

    vec3 dA = (d * this->v1).cross(d * this->v2);

    for(scalar s = 0; s < 1; s += d)
        for(scalar t = 0; t < 1; t += d)
        {
            vec3 p = this->p0 + s * this->v1 + t * this->v2;
            sum += integrand(p, dA);
        }
    return sum;
}

vec3 Disk::integrate(vec3 (*integrand)(vec3 s, vec3 dA), scalar dr) const
{
    vec3 sum = 0;

    scalar radius = this->radius.magnitude();
    vec3 radnorm = this->radius.normalize();

    /* chosen so that the outermost ring will consist of square area
     * elements */
    scalar dtheta = dr / radius;

    quat rot = quat::from_angleaxis(dtheta, this->normal);

    for(scalar r = 0; r < radius; r += dr)
    {
        vec3 s = this->center + radnorm * r;

        /* area element is constant for given r */
        vec3 dA = this->normal * r * dr * dtheta;

        for(scalar theta = 0; theta < this->angle; theta += dtheta)
        {
            sum += integrand(s, dA);

            s = s.rotateby(rot);
        }
    }

    return sum;
}

vec3 Sphere::integrate(vec3 (*integrand)(vec3 s, vec3 dA), scalar d) const
{
    vec3 sum = 0;
    /*
     * Coordinate reference (right-handed):
     *
     *  ^ z
     *  |
     *  |    ^ y
     *  |   /
     *  |  /
     *  | /
     *  |/
     *  O---------> x
     *
     */

    /* we will rotate this unit vector clockwise in the X-Z plane by d
     * each outer loop (scale and offset later) */
    vec3 rad = vec3(0, 0, 1.0);

    scalar r_sq = this->radius * this->radius;

    quat roty = quat::from_angleaxis(d, vec3(0, 1, 0));
    quat rotz = quat::from_angleaxis(d, vec3(0, 0, 1));

    for(scalar phi = 0; phi < M_PI; phi += d)
    {
        /* operate on a copy to avoid accumulating roundoff error */
        vec3 rad2 = rad;

        /* from Jacobian: dA = r^2 * sin(phi) * dphi * dtheta */
        scalar dA = r_sq * sin(phi) * d * d;

        for(scalar theta = 0; theta < 2*M_PI; theta += d)
        {
            sum += integrand(this->center + this->radius * rad2, dA * rad2);

            /* rotate rad2 around the z axis */
            rad2 = rad2.rotateby(rotz);
        }

        /* rotate radius clockwise around y */
        rad = rad.rotateby(roty);
    }

    return sum;
}

vec3 OpenCylinder::integrate(vec3 (*integrand)(vec3 s, vec3 dA), scalar d) const
{
    /*
     * We loop along the axis length, rotating a vector around it as
     * we go.
     *
     * Offset is trivial.
     *
     *        rad/v
     *          ^       x
     *         .|x     . x
     *        . | x   .   x
     *    O-------x-------------> axis
     *    x   .   x   .   x   .
     *     x .     x .     x .
     *      x       x       x
     *
     */

    /* TODO: normalize d so all integrals run in 1/d^n time? */

    vec3 v = vec3::any_unit_normal(this->axis);
    quat rot = quat::from_angleaxis(d, this->axis);

    scalar axis_len = this->axis.magnitude();
    vec3 norm_axis = this->axis.normalize();

    vec3 sum = 0;

    for(scalar l = 0; l < axis_len; l += d)
    {
        vec3 rad = v;
        for(scalar theta = 0; theta < 2*M_PI; theta += d)
        {
            sum += integrand(this->origin + l * norm_axis + this->radius * rad, rad);
            rad = rad.rotateby(rot);
        }
    }

    return sum;
}

vec3 ClosedCylinder::integrate(vec3 (*integrand)(vec3 s, vec3 dA), scalar d) const
{
    vec3 sum = OpenCylinder::integrate(integrand, d);

    sum += cap1.integrate(integrand, d) + cap2.integrate(integrand, d);

    return sum;
}

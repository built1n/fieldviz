#ifndef SURFACE_H
#define SURFACE_H

#include <fml/fml.h>
#include "manifold.h"

using namespace fml;

/* All surfaces inherit this class */
/* The exact meaning of d is surface-dependent (see class
 * definitions below); the limit of the calculated integral must
 * approach its true value as d->0. d must be > 0. */
class Surface : public Manifold {
public:
    const int dimension() const { return 2; }
};

class Plane : public Surface {
private:
    /* The surface is specified by all points p = p0 + s v1 + t v2,
     * such that 0 <= {s, t} < 1
     *
     * v1 and v2 must NOT be parallel.
     *
     * v1 and v2 should (but do not have to be) be normal (this is
     * motivated primarily by usability; if the two vectors are indeed
     * normal, then m1 and m2 have the nice geometric meaning of side
     * length).
     *
     * d = ds = dt.
     * dA will be in the direction of v1 x v2.
     */
    vec3 p0, v1, v2;

public:
    Plane(vec3 p, vec3 _v1, vec3 _v2) : p0(p), v1(_v1), v2(_v2) {};

    vec3 integrate(vec3 (*integrand)(vec3 s, vec3 dA), scalar d) const;
    const char *name() const { return "Plane"; }
};

/* Flat, circular disk (of varying extent) */
class Disk : public Surface {
private:
    /* This represents a (possibly incomplete) circular disk in space,
     * with a center, radius, and normal vector as specified.
     *
     * `angle' specifies the extent of the disk; angle=2pi for a full
     * circle.
     *
     * We use:
     * d = dr.
     * d_theta = d / r.
     *
     * This makes it so that differential area elements on the edge of
     * the disk are square.
     *
     * dA will be in the direction of normal.
     */
    vec3 center, radius, normal;
    scalar angle;

public:
    Disk(vec3 c, vec3 r, vec3 n, scalar a) : center(c), radius(r), normal(n), angle(a) {};

    vec3 integrate(vec3 (*integrand)(vec3 s, vec3 dA), scalar d) const;
    const char *name() const { return "Disk"; }
};

/* Hollow, spherical shell */
class Sphere : public Surface {
private:
    /* d = dtheta = dphi */

    vec3 center;
    scalar radius;

public:
    Sphere(vec3 c, scalar r) : center(c), radius(r) {};

    vec3 integrate(vec3 (*integrand)(vec3 s, vec3 dA), scalar d) const;
    const char *name() const { return "Sphere"; }
};

/* Cylinder without end caps */
class OpenCylinder : public Surface {
private:
    /* Like this:
     *
     *         ___________________
     *        / \                 \
     *       origin ---------------> axis
     *        \_/_________________/
     *
     */

    vec3 origin, axis;
    scalar radius;

public:
    OpenCylinder(vec3 o, vec3 a, scalar r) : origin(o), axis(a), radius(r) {}

    vec3 integrate(vec3 (*integrand)(vec3 s, vec3 dA), scalar d) const;
    const char *name() const { return "OpenCylinder"; }
};

/* Capped cylinder */
class ClosedCylinder : public OpenCylinder {
private:
    Disk cap1, cap2;

public:
    ClosedCylinder(vec3 o, vec3 a, scalar r) : OpenCylinder(o, a, r),
                                               cap1(o, vec3::any_unit_normal(a), -a.normalize(), 2*M_PI),
                                               cap2(o + a, vec3::any_unit_normal(a), a.normalize(), 2*M_PI) {}

    vec3 integrate(vec3 (*integrand)(vec3 s, vec3 dA), scalar d) const;
    const char *name() const { return "ClosedCylinder"; }
};

#endif

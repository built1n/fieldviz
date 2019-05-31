#ifndef CURVE_H
#define CURVE_H

#include <cmath>
#include <iostream>
#include <fml/fml.h>

#include "manifold.h"

using namespace fml;

/* All curves inherit this class (which is empty because Manifold has
 * everything we need). */
class Curve : public Manifold {
public:
    const int dimension() const { return 1; }
};

class LineSegment : public Curve {
private:
    vec3 a, b;
public:
    LineSegment(vec3 a_, vec3 b_) : a(a_), b(b_) {};

    vec3 integrate(vec3 (*integrand)(vec3 s, vec3 ds), scalar delta) const;

    const char *name() const { return "LineSegment"; }

    friend std::istream operator>>(std::istream &is, LineSegment &ls);
};

std::istream operator>>(std::istream &is, LineSegment &ls);

class Arc : public Curve {
private:
    vec3 center;

    /* these are relative to the center (direction will be determined
     * by RHR of normal), and should be orthonormal */
    vec3 radius, normal;

    /* how many radians the arc extends for (can be greater than 2pi) */
    scalar angle;
public:
    Arc(vec3 c_, vec3 r_, vec3 n_, scalar th) : center(c_), radius(r_), normal(n_), angle(th) {};

    vec3 integrate(vec3 (*integrand)(vec3 s, vec3 ds), scalar delta) const;

    const char *name() const { return "Arc"; }

    friend std::istream operator>>(std::istream &is, LineSegment &ls);
};

std::istream operator>>(std::istream &is, LineSegment &ls);

class Spiral : public Curve {
private:
    vec3 origin;

    /* these are relative to the center (direction will be determined
     * by RHR of normal), and should be orthonormal */
    vec3 radius, normal;

    /* how many radians the arc extends for (can be greater than 2pi) */
    scalar angle;

    /* linear distance between turns (2pi) */
    scalar pitch;
public:
    Spiral(vec3 c_, vec3 r_, vec3 n_, scalar th, scalar p) : origin(c_), radius(r_), normal(n_), angle(th), pitch(p) {};

    vec3 integrate(vec3 (*integrand)(vec3 s, vec3 ds), scalar delta) const;

    const char *name() const { return "Solenoid"; }

    friend std::istream operator>>(std::istream &is, LineSegment &ls);
};

std::istream operator>>(std::istream &is, LineSegment &ls);

class Toroid : public Curve {
private:
    vec3 origin;

    /* these are relative to the center (direction will be determined
     * by RHR of normal), and should be orthonormal */
    vec3 major_radius, major_normal;

    /* "thickness" of toroid */
    scalar minor_r;

    /* how many radians (about the center) the toroid extends for
     * (can be greater than 2pi) */
    scalar major_angle;

    /* central angle between successive turns (2pi rotation of small
     * radius vector) */
    scalar pitch;
public:
    Toroid() {};
    Toroid(vec3 o, vec3 maj_r, vec3 maj_n, scalar ang, scalar min_r, scalar p) : origin(o), major_radius(maj_r), major_normal(maj_n), major_angle(ang), minor_r(min_r), pitch(p) {};

    vec3 integrate(vec3 (*integrand)(vec3 s, vec3 ds), scalar delta) const;

    const char *name() const { return "Toroid"; };

    friend std::istream operator>>(std::istream &is, LineSegment &ls);
};

std::istream operator>>(std::istream &is, LineSegment &ls);
#endif

#ifndef CURVE_H
#define CURVE_H

#include <cmath>
#include "vec3.h"
#include "quat.h"

using namespace std;

class Curve {
public:
    virtual vec3 integrate(vec3 (*integrand)(vec3 s, vec3 ds), double delta) = 0;
};

class LineSegment : Curve {
private:
    vec3 a, b;
public:
    LineSegment(vec3 a_, vec3 b_) : a(a_), b(b_) {};

    vec3 integrate(vec3 (*integrand)(vec3 s, vec3 ds), double delta);
};

class Arc : Curve {
private:
    vec3 center;

    /* these are relative to the center (direction will be determined
     * by RHR of normal), and should be orthonormal */
    vec3 radius, normal;

    /* how many radians the arc extends for (can be greater than 2pi) */
    double angle;
public:
    Arc(vec3 c_, vec3 r_, vec3 n_, double th) : center(c_), radius(r_), normal(n_), angle(th) {};

    vec3 integrate(vec3 (*integrand)(vec3 s, vec3 ds), double delta);
};

class Spiral : Curve {
private:
    vec3 origin;

    /* these are relative to the center (direction will be determined
     * by RHR of normal), and should be orthonormal */
    vec3 radius, normal;

    /* how many radians the arc extends for (can be greater than 2pi) */
    double angle;

    /* space between turns (2pi) */
    double pitch;
public:
    Spiral(vec3 c_, vec3 r_, vec3 n_, double th, double p) : origin(c_), radius(r_), normal(n_), angle(th), pitch(p) {};

    vec3 integrate(vec3 (*integrand)(vec3 s, vec3 ds), double delta);
};
#endif

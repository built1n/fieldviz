#include <cmath>
#include "vec3.h"
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
    double radius;
    double angle[2]; /* start and end angles */
public:
    LineSegment(vec3 a_, vec3 b_) : a(a_), b(b_) {};

    vec3 integrate(vec3 (*integrand)(vec3 s, vec3 ds), double delta);
};

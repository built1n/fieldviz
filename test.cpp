#include <cmath>
#include <iostream>
#include "curve.h"

vec3 integrand(vec3 s, vec3 ds)
{
    return ds;
}

vec3 point;

/* dl x r / (|r| ^ 2) */
vec3 dB(vec3 s, vec3 ds)
{
    vec3 r = s - point;

    double r2 = r.magnitudeSquared();

    vec3 rnorm = r / std::sqrt(r2);

    return ds.cross(rnorm) / r2;
}

int main()
{
    LineSegment wire(vec3(0, -100, 0), vec3(0, 100, 0));

    vec3 dx = .001;

    point = .01;

    double I = 1;

    for(int i = 0; i < 100; i++, point += dx)
        std::cout << point[0] << " " << wire.integrate(dB, 1e-2)[2] << endl;
}

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

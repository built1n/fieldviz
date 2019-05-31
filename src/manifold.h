#ifndef MANIFOLD_H
#define MANIFOLD_H

#include <cmath>
#include <iostream>
#include <fml/fml.h>
using namespace fml;

/* All manifolds inherit this class */
class Manifold {
public:
    virtual vec3 integrate(vec3 (*integrand)(vec3 s, vec3 ds), scalar delta) const = 0;
    virtual const char *name() const = 0;
    virtual const int dimension() const = 0; // 0 = point, 1 = curve, 2 = surface, 3 = solid
};

#endif

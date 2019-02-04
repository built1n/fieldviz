#ifndef QUAT_H
#define QUAT_H
#include "vec3.h"
#include <iostream>

class quat {
public:
    scalar w, x, y, z;
public:
    quat(scalar w, scalar x, scalar y, scalar z);
    quat(scalar x, scalar y, scalar z);
    quat(scalar w, vec3 vec);
    quat(vec3 vec);
    quat();

    operator vec3();

    quat conjugate() const;

    static quat from_angleaxis(scalar angle, vec3 axis);
};

quat operator*(const quat &, const quat &);
std::ostream &operator<<(std::ostream &os, const quat &);
#endif

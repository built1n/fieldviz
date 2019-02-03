#ifndef QUAT_H
#define QUAT_H
#include "vec3.h"
#include <iostream>

class quat {
public:
    double w, x, y, z;
public:
    quat(double w, double x, double y, double z);
    quat(double x, double y, double z);
    quat(double w, vec3 vec);
    quat(vec3 vec);
    quat();

    operator vec3();

    quat conjugate() const;

    static quat from_angleaxis(double angle, vec3 axis);
};

quat operator*(const quat &, const quat &);
std::ostream &operator<<(std::ostream &os, const quat &);
#endif

#include "quat.h"
#include <cmath>

quat::quat(double w_, double x_, double y_, double z_) : w(w_), x(x_), y(y_), z(z_) { }
quat::quat(double x_, double y_, double z_) : w(0), x(x_), y(y_), z(z_) { }
quat::quat(double w_, vec3 vec) : w(w_), x(vec[0]), y(vec[1]), z(vec[2]) { }
quat::quat(vec3 vec) : w(0), x(vec[0]), y(vec[1]), z(vec[2]) { }
quat::quat() : w(0), x(0), y(0), z(0) { }

quat::operator vec3()
{
    return vec3(this->x, this->y, this->z);
}

quat operator*(const quat &lhs, const quat &rhs)
{
    return quat(lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z,
                lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
                lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x,
                lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w);
}

quat quat::conjugate() const
{
    return quat(this->w, -this->x, -this->y, -this->z);
}

quat quat::from_angleaxis(double angle, vec3 axis)
{
    double si = std::sin(angle / 2);
    double co = std::cos(angle / 2);
    return quat(co, si * axis[0], si * axis[1], si * axis[2]);
}

std::ostream &operator<<(std::ostream &os, const quat &q)
{
    return os << "(" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
}

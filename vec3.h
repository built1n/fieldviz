#ifndef VEC3_H
#define VEC3_H
#include <iostream>

typedef float scalar;

class vec3 {
    public:
        scalar v[3];
    public:
        vec3();
        vec3(scalar x);
        vec3(scalar x, scalar y, scalar z);
        scalar &operator[](int index);
        scalar operator[](int index) const;
        vec3 operator*(scalar scale) const;
        vec3 operator/(scalar scale) const;
        vec3 operator+(const vec3 &other) const;
        vec3 operator-(const vec3 &other) const;
        vec3 operator-() const;
        const vec3 &operator*=(scalar scale);
        const vec3 &operator/=(scalar scale);
        const vec3 &operator+=(const vec3 &other);
        const vec3 &operator-=(const vec3 &other);
        scalar magnitude() const;
        scalar magnitudeSquared() const;
        vec3 normalize() const;
        scalar dot(const vec3 &other) const;
        vec3 cross(const vec3 &other) const;
};
vec3 operator*(scalar scale, const vec3 &v);

std::ostream &operator<<(std::ostream &output, const vec3 &v);
#endif

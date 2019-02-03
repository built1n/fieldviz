#ifndef VEC3_H
#define VEC3_H
#include <iostream>
class vec3 {
    public:
        double v[3];
    public:
        vec3();
        vec3(double x);
        vec3(double x, double y, double z);
        double &operator[](int index);
        double operator[](int index) const;
        vec3 operator*(double scale) const;
        vec3 operator/(double scale) const;
        vec3 operator+(const vec3 &other) const;
        vec3 operator-(const vec3 &other) const;
        vec3 operator-() const;
        const vec3 &operator*=(double scale);
        const vec3 &operator/=(double scale);
        const vec3 &operator+=(const vec3 &other);
        const vec3 &operator-=(const vec3 &other);
        double magnitude() const;
        double magnitudeSquared() const;
        vec3 normalize() const;
        double dot(const vec3 &other) const;
        vec3 cross(const vec3 &other) const;
};

std::ostream &operator<<(std::ostream &output, const vec3 &v);
#endif

/* copy-pasted from:
 * https://www.programming-techniques.com/2013/05/basic-euclidean-vector-operations-in-c.htm
 */

#include <iostream>
#include <cmath>
#include "vec3.h"
using std::ostream;
vec3::vec3() {
    v[0] = 0;
    v[1] = 0;
    v[2] = 0;
}
vec3::vec3(scalar x) {
    v[0] = x;
    v[1] = 0;
    v[2] = 0;
}
vec3::vec3(scalar x, scalar y, scalar z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
}
scalar &vec3::operator[](int index) {
    return v[index];
}
scalar vec3::operator[](int index) const {
    return v[index];
}
vec3 vec3::operator*(scalar scale) const {
    return vec3(v[0] * scale, v[1] * scale, v[2] * scale);
}
vec3 vec3::operator/(scalar scale) const {
    return vec3(v[0] / scale, v[1] / scale, v[2] / scale);
}
vec3 vec3::operator+(const vec3 &other)  const{
    return vec3(v[0] + other.v[0], v[1] + other.v[1], v[2] + other.v[2]);
}
vec3 vec3::operator-(const vec3 &other) const {
    return vec3(v[0] - other.v[0], v[1] - other.v[1], v[2] - other.v[2]);
}
vec3 vec3::operator-() const {
    return vec3(-v[0], -v[1], -v[2]);
}
const vec3 &vec3::operator*=(scalar scale) {
    v[0] *= scale;
    v[1] *= scale;
    v[2] *= scale;
    return *this;
}
const vec3 &vec3::operator/=(scalar scale) {
    v[0] /= scale;
    v[1] /= scale;
    v[2] /= scale;
    return *this;
}
const vec3 &vec3::operator+=(const vec3 &other) {
    v[0] += other.v[0];
    v[1] += other.v[1];
    v[2] += other.v[2];
    return *this;
}
const vec3 &vec3::operator-=(const vec3 &other) {
    v[0] -= other.v[0];
    v[1] -= other.v[1];
    v[2] -= other.v[2];
    return *this;
}
scalar vec3::magnitude() const {
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}
scalar vec3::magnitudeSquared() const {
    return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}
vec3 vec3::normalize() const {
    scalar m = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    return vec3(v[0] / m, v[1] / m, v[2] / m);
}
scalar vec3::dot(const vec3 &other) const {
    return v[0] * other.v[0] + v[1] * other.v[1] + v[2] * other.v[2];
}
vec3 vec3::cross(const vec3 &other) const {
    return vec3(v[1] * other.v[2] - v[2] * other.v[1],
                 v[2] * other.v[0] - v[0] * other.v[2],
                 v[0] * other.v[1] - v[1] * other.v[0]);
}
std::ostream &operator<<(std::ostream &output, const vec3 &v) {
    return output << v[0] << " " << v[1] << " " << v[2];
}

vec3 operator*(scalar scale, const vec3 &v)
{
    return v * scale;
}

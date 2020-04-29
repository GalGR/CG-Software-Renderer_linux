#include "Vector4.h"

Vector4::Vector4(const Vector3 &vec3) : Vector4(vec3.x, vec3.y, vec3.z) {}
Vector4::operator Vector3() const { return Vector4::euclid(*this); }
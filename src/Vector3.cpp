#include "Vectors.h"

Vector3::operator Vector4() const { return Vector4(*this); }
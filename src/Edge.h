#pragma once

#include <array>
#include <vector>

#include "Vectors.h"

class Edge {
	std::array<size_t, 2> indices;

public:
	Edge() = default;
	Edge(size_t v1, size_t v2) : indices{ v1, v2 } {
		if (v1 > v2) {
			indices[0] = v2;
			indices[1] = v1;
		}
	}
	Edge(std::array<size_t, 2> indices) : indices{ indices } {
		if (indices[0] > indices[1]) {
			this->indices[0] = indices[1];
			this->indices[1] = indices[0];
		}
	}

	friend bool operator ==(const Edge &lhs, const Edge &rhs) { return lhs.indices[0] == rhs.indices[0] && lhs.indices[1] == rhs.indices[1]; }
	size_t operator [](short i) const { return indices[i]; }
	size_t operator ()(short i) const { return indices[i]; }
	const Vector4 &operator ()(const std::vector<Vector4> &vector4s, short i) const { return vector4s[indices[i]]; }
	Vector4 &operator ()(std::vector<Vector4> &vector4s, short i) const { return vector4s[indices[i]]; }
};
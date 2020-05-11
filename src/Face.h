#pragma once

#include <array>
#include <vector>
#include "Vectors.h"

#define FACE_VERTICES 3

struct Face {
	std::array<size_t, FACE_VERTICES> indices;
	Vector4 face_normal;
	bool counter_clock_wise = true;

	Face() = default;
	Face(std::array<size_t, FACE_VERTICES> indices, const std::vector<Vector4> &vertices, bool counter_clock_wise = true);
	Face(std::array<size_t, FACE_VERTICES> indices, Vector4 face_normal, bool counter_clock_wise = true) :
		indices{ indices }, face_normal{ face_normal }, counter_clock_wise{ counter_clock_wise } {}

	// Access indices operator
	size_t &operator [](short i) {
		return this->indices[i];
	}
	const size_t &operator [](short i) const {
		return this->indices[i];
	}
	size_t &operator ()(short i) {
		return this->indices[i];
	}
	const size_t &operator ()(short i) const {
		return this->indices[i];
	}

	// Access vertices
	Vector4 &operator ()(std::vector<Vector4> &vertices, short i) const {
		return vertices[this->indices[i]];
	}
	const Vector4 &operator ()(const std::vector<Vector4> &vertices, short i) const {
		return vertices[this->indices[i]];
	}

	// Calculate a orthogonal (perpendicular vector not length 1)
	Vector4 calc_ortho(const std::vector<Vector4> &vertices) const {
		Vector4 euclid_left, euclid_right;
		if (counter_clock_wise) {
			euclid_left = (*this)(vertices, 0).to((*this)(vertices, 2));
			euclid_right = (*this)(vertices, 0).to((*this)(vertices, 1));
		}
		else {
			euclid_left = (*this)(vertices, 0).to((*this)(vertices, 1));
			euclid_right = (*this)(vertices, 0).to((*this)(vertices, 2));
		}
		return euclid_right ^ euclid_left;
	}

	// Calculate a normal
	Vector4 calc_normal(const std::vector<Vector4> &vertices) const {
		Vector4 ortho = calc_ortho(vertices);
		Vector4 normal = ortho | ortho.euclid_length();
		return normal;
	}

	// Get the face normal
	Vector4 &normal() {
		return this->face_normal;
	}
	const Vector4 &normal() const {
		return this->face_normal;
	}

	// Get the vertices normal
	Vector4 &normal(std::vector<Vector4> &normals, short i) const {
		return normals[this->indices[i]];
	}
	const Vector4 &normal(const std::vector<Vector4> &normals, short i) const {
		return normals[this->indices[i]];
	}
};
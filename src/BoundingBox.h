#pragma once

#include "Vector4.h"

#define BOUNDING_BOX_VERTICES 8

struct BoundingBox {
	Vector4 min_v;
	Vector4 max_v;

	BoundingBox() = default;
	BoundingBox(Vector4 min_v, Vector4 max_v) : min_v(min_v.euclid()), max_v(max_v.euclid()) {}
	BoundingBox(const std::vector<Vector4> &vertices) {
		size_t numVertices = vertices.size();
		for (short j = 0; j < 3; ++j) {
			min_v[j] = vertices[0](j);
			max_v[j] = vertices[0](j);
		}
		for (size_t i = 1; i < numVertices; ++i) {
			const Vector4 &vertex = vertices[i];
			for (short j = 0; j < 3; ++j) {
				double axis = vertex(j);
				if (min_v[j] > axis) min_v[j] = axis;
				if (max_v[j] < axis) max_v[j] = axis;
			}
		}
	}

	void bound_mesh(const std::vector<Vector4> &vertices) { *this = BoundingBox(vertices); }

	double min(short i) {
		return min_v[i];
	}
	double max(short i) {
		return max_v[i];
	}
	double min_axis() {
		double min_axis = min_v[0];
		for (short i = 1; i < 3; ++i) {
			if (min_axis > min_v[i]) min_axis = min_v[i];
		}
		return min_axis;
	}
	double max_axis() {
		double max_axis = max_v[0];
		for (short i = 1; i < 3; ++i) {
			if (max_axis < max_v[i]) max_axis = max_v[i];
		}
		return max_axis;
	}

	/* Returns in this order:

	                 max
	        7---------6
	       /|        /|
	      3---------2 |
	      | |       | |
	      | |       | |
	      | 4-------|-5
	      |/        |/
	      0---------1
	     min

	  y
	  | z
	  |/
	  .----x

	*/
	std::array<Vector4, 8> calcBoxVertices() const {
		std::array<Vector4, 8> boxVertices;
		boxVertices[0] = min_v;
		boxVertices[1] = Vector4(max_v[0], min_v[1], min_v[2]);
		boxVertices[2] = Vector4(max_v[0], max_v[1], min_v[2]);
		boxVertices[3] = Vector4(min_v[0], max_v[1], min_v[2]);
		boxVertices[4] = Vector4(min_v[0], min_v[1], max_v[2]);
		boxVertices[5] = Vector4(max_v[0], min_v[1], max_v[2]);
		boxVertices[6] = max_v;
		boxVertices[7] = Vector4(min_v[0], max_v[1], max_v[2]);
		return boxVertices;
	}
};
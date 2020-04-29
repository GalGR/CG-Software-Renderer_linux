#pragma once

#include <vector>
#include <memory>
#include <mutex>

#include "Vector4.h"
#include "Matrix4.h"
#include "Face.h"
#include "Edge.h"
#include "Obj_Parser/wavefront_obj.h"

#define NORMALIZE_SCALE 10.0

struct MeshModel {
	std::vector<Vector4> vertices;
	std::vector<Vector4> normals;
	std::vector<Face> faces;
	std::vector<Edge> edges;

	MeshModel() = default;
	MeshModel(const Wavefront_obj &obj, bool normalize = true);
	MeshModel(const std::vector<Vector4> &vertices, const std::vector<Face> &faces);
	MeshModel(const std::vector<Vector4> &vertices, const std::vector<Vector4> &normals, const std::vector<Face> &faces);

	void importOBJ(const Wavefront_obj &obj, bool normalize = true);
	void normalize();
	void calc_vertices_normals(const std::vector<bool> &normals_available);

	// Gets
	// ----

	Vector4 &vertex(size_t i) {
		return vertices[i];
	}
	const Vector4 &vertex(size_t i) const {
		return vertices[i];
	}

	Face &face(size_t i) {
		return faces[i];
	}
	const Face &face(size_t i) const {
		return faces[i];
	}

	Vector4 &vertex(size_t face_i, short vertex_i) {
		return faces[face_i](vertices, vertex_i);
	}
	const Vector4 &vertex(size_t face_i, short vertex_i) const {
		return faces[face_i](vertices, vertex_i);
	}
};
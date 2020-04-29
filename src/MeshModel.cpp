#include "MeshModel.h"

#include <assert.h>
#include <algorithm>
#include <cmath>

static bool check_faces(const std::vector<Vector4> &vertices, const std::vector<Face> &faces) {
	size_t numVertices = vertices.size();
	size_t numFaces = faces.size();
	std::vector<bool> touched_vertices(numVertices, false);
	for (size_t i = 0; i < numFaces; ++i) {
		for (short j = 0; j < FACE_VERTICES; ++j) {
			touched_vertices[faces[i][j]] = true;
		}
	}
	for (size_t i = 0; i < numVertices; ++i) {
		if (!touched_vertices[i]) return false;
	}
	return true;
}

static void calc_all_vertices_normals(const std::vector<Vector4> &vertices, const std::vector<Face> &faces, std::vector<Vector4> &normals) {
	size_t numVertices = vertices.size();
	size_t numFaces = faces.size();
	for (size_t i = 0; i < numFaces; ++i) {
		const Face &face = faces[i];
		for (short j = 0; j < FACE_VERTICES; ++j) {
			Vector4 &normal = normals[face[j]];
			normal = normal + face.calc_ortho(vertices);
		}
	}
	for (size_t i = 0; i < numVertices; ++i) {
		normals[i].normal();
	}
}

void MeshModel::calc_vertices_normals(const std::vector<bool> &normals_available) {
	size_t numVertices = this->vertices.size();
	size_t numFaces = this->faces.size();
	for (size_t i = 0; i < numFaces; ++i) {
		const Face &face = this->faces[i];
		for (short j = 0; j < FACE_VERTICES; ++j) {
			if (!normals_available[face[j]]) {
				Vector4 &normal = normals[face[j]];
				normal = normal + face.calc_ortho(this->vertices);
			}
		}
	}
	for (size_t i = 0; i < numVertices; ++i) {
		if (!normals_available[i]) {
			normals[i].normal();
		}
	}
}

static void calc_edges(const size_t numVertices, const std::vector<Face> &faces, std::vector<Edge> &edges) {
	size_t numFaces = faces.size();
	std::vector<std::vector<size_t>> edges_sparse_matrix(numVertices);
	for (size_t i = 0; i < numFaces; ++i) {
		const Face &face = faces[i];
		for (short j = 0; j < FACE_VERTICES - 1; ++j) {
			size_t v1 = face[j];
			size_t v2 = face[j + 1];
			if (v1 > v2) std::swap(v1, v2);
			std::vector<size_t> &v1Edges = edges_sparse_matrix[v1];
			if (!std::binary_search(v1Edges.begin(), v1Edges.end(), v2)) {
				v1Edges.push_back(v2);
				std::sort(v1Edges.begin(), v1Edges.end());
			}
		}
		size_t v1 = face[FACE_VERTICES - 1];
		size_t v2 = face[0];
		if (v1 > v2) std::swap(v1, v2);
		std::vector<size_t> &v1Edges = edges_sparse_matrix[v1];
		if (!std::binary_search(v1Edges.begin(), v1Edges.end(), v2)) {
			v1Edges.push_back(v2);
			std::sort(v1Edges.begin(), v1Edges.end());
		}
	}
	for (size_t v1 = 0; v1 < numVertices; ++v1) {
		const std::vector<size_t> &v1Edges = edges_sparse_matrix[v1];
		short v1NumEdges = (short)v1Edges.size();
		for (short i = 0; i < v1NumEdges; ++i) {
			size_t v2 = v1Edges[i];
			Edge edge(v1, v2);
			edges.push_back(edge);
		}
	}
}

MeshModel::MeshModel(const std::vector<Vector4> &vertices, const std::vector<Face> &faces) :
	vertices(vertices), normals(vertices.size(), Vector4()), faces(faces) {

	assert(check_faces(vertices, faces));

	calc_all_vertices_normals(this->vertices, this->faces, this->normals);
	assert(this->vertices.size() == this->normals.size());

	calc_edges(this->vertices.size(), this->faces, this->edges);
}
MeshModel::MeshModel(const std::vector<Vector4> &vertices, const std::vector<Vector4> &normals, const std::vector<Face> &faces) :
	vertices(vertices), normals(normals), faces(faces) {

	assert(check_faces(this->vertices, this->faces));
	assert(this->vertices.size() == this->normals.size());

	calc_edges(this->vertices.size(), this->faces, this->edges);
}

MeshModel::MeshModel(const Wavefront_obj &obj, bool normalize) {
	size_t numPoints = obj.m_points.size();
	size_t numNormals = obj.m_normals.size();
	size_t numFaces = obj.m_faces.size();

	for (size_t i = 0; i < numPoints; ++i) { /* Import vertices */
		const Wavefront_obj::Vector &point = obj.m_points[i];
		Vector4 vertex(point[0], point[1], point[2]);
		this->vertices.push_back(vertex);
	}
	std::vector<Vector4> temp_normals(numNormals, Vector4());
	for (size_t i = 0; i < numNormals; ++i) { /* Import normals */
		const Wavefront_obj::Vector &obj_normal = obj.m_normals[i];
		Vector4 normal(obj_normal[0], obj_normal[1], obj_normal[2]);
		temp_normals[i] = normal;
	}
	std::vector<std::array<size_t, FACE_VERTICES>> indices(numFaces);
	this->normals = std::vector<Vector4>(numPoints, Vector4());
	std::vector<bool> normals_available(numPoints, false);
	for (size_t i = 0; i < numFaces; ++i) { /* Import faces */
		const Wavefront_obj::Face &obj_face = obj.m_faces[i];
		const int *face_v = obj_face.v;
		const int *face_n = obj_face.n;
		std::array<size_t, FACE_VERTICES> &face_indices = indices[i];;
		for (short j = 0; j < FACE_VERTICES; ++j) {
			face_indices[j] = face_v[j];
			if (face_n[j] > 0) {
				this->normals[face_v[j]] = temp_normals[face_n[j]]; // Synchronize the vertex indices with the normal indices
				normals_available[face_v[j]] = true;
			}
		}
		Face face(indices[i], this->vertices);
		this->faces.push_back(face);
	}

	calc_vertices_normals(normals_available);

	if (normalize) this->normalize();

	assert(check_faces(this->vertices, this->faces));

	calc_edges(this->vertices.size(), this->faces, this->edges);
}

void MeshModel::importOBJ(const Wavefront_obj &obj, bool normalize) {
	*this = MeshModel(obj, normalize);
}

static Vector4 calc_mass_center(const MeshModel &meshModel) {
	size_t numVertices = meshModel.vertices.size();
	Vector4 center;
	for (size_t i = 0; i < numVertices; ++i) {
		for (short j = 0; j < 3; ++j) {
			center[j] += meshModel.vertices[i](j) / (double)numVertices;
		}
	}
	return center;
}

static double calc_max_bound(const MeshModel &meshModel) {
	double max = 0.0;
	size_t numVertices = meshModel.vertices.size();
	for (size_t i = 0; i < numVertices; ++i) {
		for (short j = 0; j < 3; ++j) {
			double abs_axis = abs(meshModel.vertices[i](j));
			if (max < abs_axis) max = abs_axis;
		}
	}
	return max;
}

void MeshModel::normalize() {
	Vector4 center = calc_mass_center(*this);
	Matrix4 translate = Matrix4::translation(-center(0), -center(1), -center(2));

	size_t numVertices = this->vertices.size();
	for (size_t i = 0; i < numVertices; ++i) {
		this->vertices[i] = translate * this->vertices[i];
	}

	double bound = calc_max_bound(*this);
	Matrix4 scale = Matrix4::iso_scaling(NORMALIZE_SCALE / bound);

	for (size_t i = 0; i < numVertices; ++i) {
		this->vertices[i] = scale * this->vertices[i];
	}
}
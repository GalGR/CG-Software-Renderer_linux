#pragma once

#include <memory>

#include "MeshModel.h"
#include "Matrix4.h"
#include "Obj_Parser/wavefront_obj.h"
#include "BoundingBox.h"
#include "Material.h"

struct Object {
	std::shared_ptr<MeshModel> p_meshModel = std::make_shared<MeshModel>(); // The mesh model
	Matrix4 model = Matrix4::I(); // Model to model transformation
	Matrix4 world = Matrix4::I(); // Model to world transformation
	Vector4 world_pos; // The position of the object in the world coordinates
	Vector4 model_pos; // The position of the object in the model coordinates
	Matrix4 rot = Matrix4::I(); // The composed rotation of model and world
	BoundingBox bBox;
	Material material;
	double normals_length = 1.0;

	Object() = default;
	Object(const MeshModel &meshModel, Matrix4 model = Matrix4::I(), Matrix4 world = Matrix4::I()) : p_meshModel(std::make_shared<MeshModel>(meshModel)), model(model), world(world), bBox(this->meshModel().vertices) {}
	Object(const Wavefront_obj &wave_obj, bool normalize = true, Matrix4 model = Matrix4::I(), Matrix4 world = Matrix4::I()) : p_meshModel(std::make_shared<MeshModel>(wave_obj, normalize)), model(model), world(world), bBox(this->meshModel().vertices) {}

	// Gets
	// ----

	inline const MeshModel &meshModel() const { return *p_meshModel; }
	inline MeshModel &meshModel() { return *p_meshModel; }

	Vector4 &vertex(size_t i) {
		return meshModel().vertices[i];
	}
	const Vector4 &vertex(size_t i) const {
		return meshModel().vertices[i];
	}

	Face &face(size_t i) {
		return meshModel().faces[i];
	}
	const Face &face(size_t i) const {
		return meshModel().faces[i];
	}

	Vector4 &vertex(size_t face_i, short vertex_i) {
		return meshModel().faces[face_i](meshModel().vertices, vertex_i);
	}
	const Vector4 &vertex(size_t face_i, short vertex_i) const {
		return meshModel().faces[face_i](meshModel().vertices, vertex_i);
	}
};
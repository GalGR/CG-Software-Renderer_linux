#pragma once

#include "MeshModel.h"
#include "Matrix4.h"
#include "Obj_Parser/wavefront_obj.h"
#include "BoundingBox.h"
#include "Material.h"

struct Object {
	MeshModel meshModel; // The mesh model
	Matrix4 model = Matrix4::I(); // Model to model transformation
	Matrix4 world = Matrix4::I(); // Model to world transformation
	Vector4 world_pos; // The position of the object in the world coordinates
	Vector4 model_pos; // The position of the object in the model coordinates
	Matrix4 rot = Matrix4::I(); // The composed rotation of model and world
	BoundingBox bBox;
	Material material;
	double normals_length = 1.0;

	Object() = default;
	Object(const MeshModel &meshModel, Matrix4 model = Matrix4::I(), Matrix4 world = Matrix4::I()) : meshModel(meshModel), model(model), world(world), bBox(this->meshModel.vertices) {}
	Object(const Wavefront_obj &wave_obj, bool normalize = true, Matrix4 model = Matrix4::I(), Matrix4 world = Matrix4::I()) : meshModel(wave_obj, normalize), model(model), world(world), bBox(this->meshModel.vertices) {}

	// Gets
	// ----

	Vector4 &vertex(size_t i) {
		return meshModel.vertices[i];
	}
	const Vector4 &vertex(size_t i) const {
		return meshModel.vertices[i];
	}

	Face &face(size_t i) {
		return meshModel.faces[i];
	}
	const Face &face(size_t i) const {
		return meshModel.faces[i];
	}

	Vector4 &vertex(size_t face_i, short vertex_i) {
		return meshModel.faces[face_i](meshModel.vertices, vertex_i);
	}
	const Vector4 &vertex(size_t face_i, short vertex_i) const {
		return meshModel.faces[face_i](meshModel.vertices, vertex_i);
	}
};
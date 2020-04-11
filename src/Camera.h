#pragma once

#include "Vector4.h"
#include "Matrix4.h"
#include "Screen.h"
#include <assert.h>
#include "PI.h"

enum class ProjectionType {
	ORTHOGRAPHIC,
	PERSPECTIVE
};

struct Camera {
	double n = 0.001; // Near
	double f = 10'000.0; // Far
	double fovy = 60.0; // Y-axis field of view
	double ortho_fov_scale = 0.11477777777; // The orthographic projection fov scale (arbitrary)
	//double ortho_width = 10.33; // The orthographic projection width (arbitrary)
	double aspect_ratio; // The screen's aspect ratio
	Matrix4 inv_view = Matrix4::I(); // The inverse view to world transformation
	//Vector4 dir = Vector4(0.0, 0.0, 1.0); // Direction the camera is pointing at
	double u_angle = 0.0; // The u (x, right) direction's rotation angle
	Matrix4 rot; // The rotation matrix
	// Vector u = v ^ w // The u (x, right) direction of the camera
	Vector4 v = Vector4(0.0, 1.0, 0.0); // The v (y, upwards) direction of the camera
	Vector4 w = Vector4(0.0, 0.0, 1.0); // The w (z, forwards) direction of the camera
	Vector4 pos = Vector4(0.0, 0.0, 0.0); // The camera start position
	ProjectionType proj_type; // Type of the projection
	Matrix4 proj_perspective; // Perspecitve projection
	Matrix4 proj_orthographic; // Orthogrphic projection

	Camera() = default;

	void update_inv_view();
	const Matrix4 &projection() const;
	Matrix4 &projection();
	void update_projections();
	void update_fps_rot();
	void update_u_angle(double a);
	void look_at(const Vector4 &obj_pos);

	// Toggle the between the projection types
	void toggle() {
		proj_type = (ProjectionType)(!(int)proj_type);
	}
};
#include "Camera.h"

#include <cmath>

void Camera::update_inv_view() {
	Matrix4 inv_rot = Matrix4::transpose(this->rot);
	Matrix4 inv_translation = Matrix4::inv_translation(this->pos[0], this->pos[1], this->pos[2]);

	// Update the inverse view matrix
	this->inv_view = inv_rot * inv_translation;
}

const Matrix4 &Camera::projection() const {
	switch (proj_type) {
	case ProjectionType::PERSPECTIVE:
		return proj_perspective;
	case ProjectionType::ORTHOGRAPHIC:
		return proj_orthographic;
	default:
		assert(0);
	}
}

Matrix4 &Camera::projection() {
	switch (proj_type) {
	case ProjectionType::PERSPECTIVE:
		return proj_perspective;
	case ProjectionType::ORTHOGRAPHIC:
		return proj_orthographic;
	default:
		assert(0);
	}
}

void Camera::update_projections() {
	this->proj_perspective = Matrix4::perspective(n, f, fovy, aspect_ratio);
	double t = ortho_fov_scale * fovy;
	double r = t * aspect_ratio;
	this->proj_orthographic = Matrix4::orthographic(n, f, -r, r, -t, t);
}

void Camera::update_fps_rot() {
	Vector4 u = this->v ^ this->w;
	Vector4 w = Vector4::euclid_add(cos(this->u_angle) & this->w, sin(this->u_angle) & this->v);
	Vector4 v = w ^ u;
	this->rot = Matrix4::rotation(u, v, w);
}

void Camera::update_u_angle(double a) {
	double u_angle = this->u_angle + a;
	this->u_angle = (u_angle < -(PI / 2)) ? (-PI / 2) :
		((u_angle > (PI / 2)) ? (PI / 2) : (u_angle));
}

void Camera::look_at(const Vector4 &obj_pos) {
	Vector4 w = Vector4::normal(obj_pos.to(this->pos));
	Vector4 u = Vector4::normal(this->v ^ w);
	Vector4 v = Vector4::normal(w ^ u);
	this->rot = Matrix4::rotation(u, v, w);
	this->w = Vector4::normal(Vector4(w(0), 0.0, w(2)));
	double u_angle = acos(this->v & v);
	u_angle = (w(1) < 0.0) ? (-u_angle) : u_angle;
	this->u_angle = u_angle;
}
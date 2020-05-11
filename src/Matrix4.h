#pragma once

#include "Vectors.h"
#include "PI.h"

#include <array>
#include <cmath>

struct Matrix4 {
	std::array<Vector4, 4> arr;

	// Constructors
	Matrix4(Vector4 v0, Vector4 v1, Vector4 v2, Vector4 v3) : arr{
		v0,
		v1,
		v2,
		v3
	} {}
	Matrix4(const std::array<double, 16> &arr) : Matrix4(
		Vector4(arr[0], arr[1], arr[2], arr[3]),
		Vector4(arr[4], arr[5], arr[6], arr[7]),
		Vector4(arr[8], arr[9], arr[10], arr[11]),
		Vector4(arr[12], arr[13], arr[14], arr[15])
	) {}
	Matrix4(
		double d0, double d1, double d2, double d3,
		double d4, double d5, double d6, double d7,
		double d8, double d9, double d10, double d11,
		double d12, double d13, double d14, double d15
	) : Matrix4(
		Vector4(d0, d1, d2, d3),
		Vector4(d4, d5, d6, d7),
		Vector4(d8, d9, d10, d11),
		Vector4(d12, d13, d14, d15)
	) {}
	Matrix4(const std::array<double, 9> &arr) : Matrix4(
		Vector4(arr[0], arr[1], arr[2], 0.0),
		Vector4(arr[3], arr[4], arr[5], 0.0),
		Vector4(arr[6], arr[7], arr[8], 0.0),
		Vector4(0.0, 0.0, 0.0, 1.0)
	) {}
	Matrix4(
		double d0, double d1, double d2,
		double d3, double d4, double d5,
		double d6, double d7, double d8
	) : Matrix4(
		Vector4(d0, d1, d2, 0.0),
		Vector4(d3, d4, d5, 0.0),
		Vector4(d6, d7, d8, 0.0),
		Vector4(0.0, 0.0, 0.0, 1.0)
	) {}
	Matrix4(double x = 0.0, double y = 0.0, double z = 0.0, double w = 1.0) : Matrix4{
		Vector4{ x, 0.0, 0.0, 0.0 },
		Vector4{ 0.0, y, 0.0, 0.0 },
		Vector4{ 0.0, 0.0, z, 0.0 },
		Vector4{ 0.0, 0.0, 0.0, w }
	} {}


	// Special Matrices
	// ----------------

	// Identity Matrix
	static Matrix4 I() { return Matrix4(1.0, 1.0, 1.0, 1.0); }

	// Zero Matrix
	static Matrix4 zero() { return Matrix4(0.0, 0.0, 0.0, 0.0); }

	// Rotation Matrices
	static Matrix4 rotationZ(double theta) {
		return Matrix4(
			cos(theta), -sin(theta), 0.0,
			sin(theta), cos(theta), 0.0,
			0.0, 0.0, 1.0
		);
	}
	static Matrix4 rotationY(double theta) {
		return Matrix4(
			cos(theta), 0.0, sin(theta),
			0.0, 1.0, 0.0,
			-sin(theta), 0.0, cos(theta)
		);
	}
	static Matrix4 rotationX(double theta) {
		return Matrix4(
			1.0, 0.0, 0.0,
			0.0, cos(theta), -sin(theta),
			0.0, sin(theta), cos(theta)
		);
	}
	static Matrix4 rotation(Vector4 u, Vector4 v, Vector4 w) {
		u.euclid();
		v.euclid();
		w.euclid();
		return Matrix4(
			u[0], v[0], w[0],
			u[1], v[1], w[1],
			u[2], v[2], w[2]
		);
	}

	// Inverse Rotation Matrices
	static Matrix4 inv_rotationZ(double theta) { return rotationZ(-theta); }
	static Matrix4 inv_rotationY(double theta) { return rotationY(-theta); }
	static Matrix4 inv_rotationX(double theta) { return rotationX(-theta); }
	static Matrix4 inv_rotation(Vector4 u, Vector4 v, Vector4 w) {
		u.euclid();
		v.euclid();
		w.euclid();
		return Matrix4(
			u[0], u[1], u[2],
			v[0], v[1], v[2],
			w[0], w[1], w[2]
		);
	}

	// Scaling Matrices
	static Matrix4 scaling(double x, double y = 1.0, double z = 1.0) { return Matrix4(x, y, z); }
	static Matrix4 iso_scaling(double s) { return Matrix4(s, s, s); }

	// Inverse Scaling Matrices
	static Matrix4 inv_scaling(double x, double y = 1.0, double z = 1.0) { return scaling(1.0 / x, 1.0 / y, 1.0 / z); }
	static Matrix4 inv_iso_scaling(double s) { return Matrix4(1.0 / s, 1.0 / s, 1.0 / s); }

	// Mirroring Matrices
	static Matrix4 mirroringXZ() { return scaling(1.0, -1.0, 1.0); }
	static Matrix4 mirroringYZ() { return scaling(-1.0, 1.0, 1.0); }
	static Matrix4 mirroringXY() { return scaling(1.0, 1.0, -1.0); }

	// Translation Matrices
	static Matrix4 translation(double x, double y = 0.0, double z = 0.0) {
		return Matrix4(
			1.0, 0.0, 0.0, x,
			0.0, 1.0, 0.0, y,
			0.0, 0.0, 1.0, z,
			0.0, 0.0, 0.0, 1.0
		);
	}
	static Matrix4 translation(const Vector4 &tr) {
		return translation(tr(0), tr(1), tr(2));
	}

	// Inverse Translation Matrices
	static Matrix4 inv_translation(double x, double y = 0.0, double z = 0.0) { return translation(-x, -y, -z); }
	static Matrix4 inv_translation(const Vector4 &tr) { return translation(-tr(0), -tr(1), -tr(2)); }

	// General Perspective Frustum (column vector)
	static Matrix4 perspective(double n, double f, double l, double r, double b, double t) {
		return Matrix4(
			(2.0 * n) / (r - l), 0.0, (r + l) / (r - l), 0.0,
			0.0, (2.0 * n) / (t - b), (t + b) / (t - b), 0.0,
			0.0, 0.0, -(f + n) / (f - n), (-2.0 * f * n) / (f - n),
			0.0, 0.0, -1.0, 0.0
		);
	}
	static Matrix4 perspective(double n, double f, double fovy, double aspect_ratio) {
		double fovy_rads = fovy * (2 * PI / 360.0);
		double tang = tan(fovy_rads / 2);
		double h = n * tang;
		double w = h * aspect_ratio;
		return perspective(n, f, -w, w, -h, h);
	}

	// General Orthographic Frustum (column vector)
	static Matrix4 orthographic(double n, double f, double l, double r, double b, double t) {
		return Matrix4(
			2.0 / (r - l), 0.0, 0.0, -(r + l) / (r - l),
			0.0, 2.0 / (t - b), 0.0, -(t + b) / (t - b),
			0.0, 0.0, -2.0 / (f - n), -(f + n) / (f - n),
			0.0, 0.0, 0.0, 1.0
		);
	}

	// General Perspective Frustum (row vector)
	static Matrix4 perspective_row_(double n, double f, double l, double r, double b, double t) {
		return Matrix4(
			(2.0 * n) / (r - l), 0.0, 0.0, 0.0,
			0.0, (2.0 * n) / (t - b), 0.0, 0.0,
			(r + l) / (r - l), (t + b) / (t - b), -(f + n) / (f - n), -1.0,
			0.0, 0.0, (-2.0 * f * n) / (f - n), 0.0
		);
	}

	// General Orthographic Frustum (row vector)
	static Matrix4 orthographic_row_(double n, double f, double l, double r, double b, double t) {
		return Matrix4(
			2.0 / (r - l), 0.0, 0.0, 0.0,
			0.0, 2.0 / (t - b), 0.0, 0.0,
			0.0, 0.0, -2.0 / (f - n), 0.0,
			-(r + l) / (r - l), -(t + b) / (t - b), -(f + n) / (f - n), 1.0
		);
	}


	// Operators
	// ---------

	// Get the row vector
	Vector4 &operator [](size_t i) { return arr[i]; }
	const Vector4 &operator [](size_t i) const { return arr[i]; }
	Vector4 &operator ()(size_t i) { return arr[i]; }
	const Vector4 &operator ()(size_t i) const { return arr[i]; }

	// Get element in the matrix
	double &operator ()(size_t i, size_t j) { return arr[i][j]; }
	const double &operator ()(size_t i, size_t j) const { return arr[i][j]; }

	// Transpose Copy
	static Matrix4 transpose(const Matrix4 &matrix) {
		Matrix4 matrix_T;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				matrix_T(j, i) = matrix(i, j);
			}
		}
		return matrix_T;
	}

	// Transpose Self
	Matrix4 &T() {
		return (*this) = transpose(*this);
	}

	// Scalar Multiplication
	friend Matrix4 operator *(double lhs, const Matrix4 &rhs) {
		Matrix4 matrix;
		for (int i = 0; i < 4; ++i) {
			matrix[i] = lhs * rhs[i];
		}
		return matrix;
	}
	friend Matrix4 operator *(const Matrix4 &lhs, double rhs) { return rhs * lhs; }

	// Matrix Vector Multiplication
	friend Vector4 operator *(const Matrix4 &lhs, const Vector4 &rhs) {
		Vector4 vector;
		for (int i = 0; i < 4; ++i) {
			vector[i] = lhs[i] * rhs;
		}
		return vector;
	}

	// Vector Matrix Multiplication
	friend Vector4 operator *(const Vector4 &lhs, const Matrix4 &rhs) { return transpose(rhs) * lhs; }

	// Matrix Matrix Multiplication
	friend Matrix4 operator *(const Matrix4 &lhs, const Matrix4 &rhs) {
		Matrix4 matrix;
		Matrix4 rhs_T = transpose(rhs);
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				matrix(i, j) = lhs[i] * rhs_T[j];
			}
		}
		return matrix;
	}

	// Matrix Addition/Subtraction
	friend Matrix4 operator +(const Matrix4 &lhs, const Matrix4 &rhs) {
		Matrix4 matrix;
		for (int i = 0; i < 4; ++i) {
			matrix[i] = lhs[i] + rhs[i];
		}
		return matrix;
	}
	friend Matrix4 operator -(const Matrix4 &lhs, const Matrix4 &rhs) {
		Matrix4 matrix;
		for (int i = 0; i < 4; ++i) {
			matrix[i] = lhs[i] - rhs[i];
		}
		return matrix;
	}

	// Matrix Equals
	friend bool operator ==(const Matrix4 &lhs, const Matrix4 &rhs) {
		for (short i = 0; i < 4; ++i) {
			for (short j = 0; j < 4; ++j) {
				if (lhs(i, j) != rhs(i, j)) return false;
			}
		}
		return true;
	}
	friend bool operator !=(const Matrix4 &lhs, const Matrix4 &rhs) {
		return !(lhs == rhs);
	}

	// Rotate
	static Matrix4 rotateX(Matrix4 matrix, double theta) { return rotationX(theta) * matrix; }
	static Matrix4 rotateY(Matrix4 matrix, double theta) { return rotationY(theta) * matrix; }
	static Matrix4 rotateZ(Matrix4 matrix, double theta) { return rotationZ(theta) * matrix; }

	// Inverse Rotate
	static Matrix4 inv_rotateX(Matrix4 matrix, double theta) { return inv_rotationX(theta) * matrix; }
	static Matrix4 inv_rotateY(Matrix4 matrix, double theta) { return inv_rotationY(theta) * matrix; }
	static Matrix4 inv_rotateZ(Matrix4 matrix, double theta) { return inv_rotationZ(theta) * matrix; }

	// Scale
	static Matrix4 scale(Matrix4 matrix, double x, double y = 0.0, double z = 0.0) { return scaling(x, y, z) * matrix; }
	static Matrix4 iso_scale(Matrix4 matrix, double s) { return iso_scaling(s) * matrix; }

	// Inverse Scale
	static Matrix4 inv_scale(Matrix4 matrix, double x, double y = 0.0, double z = 0.0) { return inv_scaling(x, y, z) * matrix; }
	static Matrix4 inv_iso_scale(Matrix4 matrix, double s) { return inv_iso_scaling(s) * matrix; }

	// Mirror
	static Matrix4 mirrorXZ(Matrix4 matrix) { return mirroringXZ() * matrix; }
	static Matrix4 mirrorYZ(Matrix4 matrix) { return mirroringYZ() * matrix; }
	static Matrix4 mirrorXY(Matrix4 matrix) { return mirroringXY() * matrix; }

	// Translate
	static Matrix4 translate(Matrix4 matrix, double x, double y = 0.0, double z = 0.0) { return translation(x, y, z) * matrix; }

	// Inverse Translate
	static Matrix4 inv_translate(Matrix4 matrix, double x, double y = 0.0, double z = 0.0) { return inv_translation(x, y, z) * matrix; }

	// Rotate Self
	Matrix4 &rotateX(double theta) { return (*this) = rotateX(*this, theta); }
	Matrix4 &rotateY(double theta) { return (*this) = rotateY(*this, theta); }
	Matrix4 &rotateZ(double theta) { return (*this) = rotateZ(*this, theta); }

	// Inverse Rotate Self
	Matrix4 &inv_rotateX(double theta) { return (*this) = inv_rotateX(*this, theta); }
	Matrix4 &inv_rotateY(double theta) { return (*this) = inv_rotateY(*this, theta); }
	Matrix4 &inv_rotateZ(double theta) { return (*this) = inv_rotateZ(*this, theta); }

	// Scale Self
	Matrix4 &scale(double x, double y = 0.0, double z = 0.0) { return (*this) = scale(*this, x, y, z); }
	Matrix4 &iso_scale(double s) { return (*this) = iso_scale(*this, s); }

	// Inverse Scale Self
	Matrix4 &inv_scale(double x, double y = 0.0, double z = 0.0) { return (*this) = inv_scale(*this, x, y, z); }
	Matrix4 &inv_iso_scale(double s) { return (*this) = inv_iso_scale(*this, s); }

	// Mirror Self
	Matrix4 &mirrorXZ() { return (*this) = mirrorXZ(*this); }
	Matrix4 &mirrorYZ() { return (*this) = mirrorYZ(*this); }
	Matrix4 &mirrorXY() { return (*this) = mirrorXY(*this); }

	// Translate Self
	Matrix4 &translate(double x, double y = 0.0, double z = 0.0) { return (*this) = translate(*this, x, y, z); }

	// Inverse Translate Self
	Matrix4 &inv_translate(double x, double y = 0.0, double z = 0.0) { return (*this) = inv_translate(*this, x, y, z); }
};
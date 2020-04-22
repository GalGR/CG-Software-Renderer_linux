#pragma once

#include <array>

#define EPS 1.0E-10

struct Vector3 {
	double x, y, z;

	// Constructors
	Vector3(double x = 0.0, double y = 0.0, double z = 0.0) : x(x), y(y), z(z) {}

	// Get the coordinates
	double &operator [](size_t i) { return (&x)[i]; }
	const double &operator [](size_t i) const { return (&x)[i]; }
	double operator ()(size_t i) { return (&x)[i]; }
	double operator ()(size_t i) const { return (&x)[i]; }

	// Addition/Subtraction
	static Vector3 add(const Vector3 &lhs, const Vector3 &rhs) {
		Vector3 vector;
		for (int i = 0; i < 3; ++i) {
			vector[i] = lhs[i] + rhs[i];
		}
		return vector;
	}
	static Vector3 sub(const Vector3 &lhs, const Vector3 &rhs) {
		Vector3 vector;
		for (int i = 0; i < 3; ++i) {
			vector[i] = lhs[i] - rhs[i];
		}
		return vector;
	}
	friend Vector3 operator +(const Vector3 &lhs, const Vector3 &rhs) { return add(lhs, rhs); }
	friend Vector3 operator -(const Vector3 &lhs, const Vector3 &rhs) { return sub(lhs, rhs); }

	// Scalar Multiplication
	friend Vector3 operator *(double lhs, const Vector3 &rhs) {
		Vector3 vector;
		for (int i = 0; i < 3; ++i) {
			vector[i] = lhs * rhs[i];
		}
		return vector;
	}
	friend Vector3 operator *(const Vector3 &lhs, double rhs) { return rhs * lhs; }

	// Scalar Division
	friend Vector3 operator /(double lhs, const Vector3 &rhs) {
		Vector3 vector;
		for (int i = 0; i < 3; ++i) {
			vector[i] = lhs / rhs[i];
		}
		return vector;
	}
	friend Vector3 operator /(const Vector3 &lhs, double rhs) {
		Vector3 vector;
		for (int i = 0; i < 3; ++i) {
			vector[i] = lhs[i] / rhs;
		}
		return vector;
	}

	// Dot Product
	friend double operator *(const Vector3 &lhs, const Vector3 &rhs) {
		double sum = 0;
		for (int i = 0; i < 3; ++i) {
			sum += lhs[i] * rhs[i];
		}
		return sum;
	}

	// Cross Product
	friend Vector3 operator ^(const Vector3 &lhs, const Vector3 &rhs) {
		Vector3 vector;
		vector[0] = (lhs(1) * rhs(2)) - (lhs(2) * rhs(1));
		vector[1] = (lhs(2) * rhs(0)) - (lhs(0) * rhs(2));
		vector[2] = (lhs(0) * rhs(1)) - (lhs(1) * rhs(0));
		return vector;
	}

	// Vector Euclidean length
	double length() const {
		return sqrt((*this) * (*this));
	}

	// Vector Homogeneous length
	double length() const {
		return sqrt((*this) * (*this));
	}

	// Normal vector
	static Vector3 normal(const Vector3 &vector) {
		return vector / vector.length();
	}

	// Normalize vector
	Vector3 &normal() {
		return (*this) = normal(*this);
	}

	// Normal of two vectors
	static Vector3 normal(const Vector3 &lhs, const Vector3 &rhs) {
		Vector3 vector_ortho = rhs ^ lhs;
		return vector_ortho / vector_ortho.length();
	}
	friend Vector3 operator %(const Vector3 &lhs, const Vector3 &rhs) {
		return normal(lhs, rhs);
	}

	// Calculate a "from" "to" vector
	Vector3 to(const Vector3 &end) const {
		const Vector3 &start = *this;
		Vector3 vec;
		for (int i = 0; i < 3; ++i) {
			vec[i] = end(i) - start(i);
		}
		return vec;
	}
	Vector3 from(const Vector3 &start) const {
		const Vector3 &end = *this;
		Vector3 vec;
		for (int i = 0; i < 3; ++i) {
			vec[i] = end(i) - start(i);
		}
		return vec;
	}

	// Vector equality
	friend bool equals(const Vector3 &lhs, const Vector3 &rhs) {
		Vector3 diff = sub(lhs, rhs);
		return diff[0] < EPS && diff[1] < EPS && diff[2] < EPS;
	}
	friend bool operator ==(const Vector3 &lhs, const Vector3 &rhs) { return equals(lhs, rhs); }

	// Negation
	static Vector3 neg(const Vector3 &vec) {
		Vector3 vector;
		for (int i = 0; i < 3; ++i) {
			vector[i] = -vec[i];
		}
		return vector;
	}
	friend Vector3 operator -(const Vector3 &vec) { return neg(vec); }
	
	// Self negation
	Vector3 &neg() { return (*this) = neg(*this); }
};
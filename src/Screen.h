#pragma once

#include "Point.h"

struct MyScreen : public Point {

	inline MyScreen() = default;
	inline MyScreen(Point p) : Point(p) {}
	inline MyScreen(int x, int y) : Point(x, y) {}
	inline const int &operator[](const short i) const { return (&x)[i]; }
	inline int &operator[](const short i) { return (&x)[i]; }
	inline friend MyScreen operator+(const MyScreen &lhs, const MyScreen &rhs) { return MyScreen(lhs.x + rhs.x, lhs.y + rhs.y); }
	inline friend MyScreen operator-(const MyScreen &lhs, const MyScreen &rhs) { return MyScreen(lhs.x - rhs.x, lhs.y - rhs.y); }

	inline Point mid_point() const { return MyScreen(x / 2, y / 2); }
	inline int mid_x_int() const { return x / 2; }
	inline int mid_y_int() const { return y / 2; }
	inline double mid_x_double() const { return x / 2.0; }
	inline double mid_y_double() const { return y / 2.0; }
	inline double aspect_ratio() const { return (double)x / (double)y; }
};
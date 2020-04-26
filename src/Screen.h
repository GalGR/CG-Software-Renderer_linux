#pragma once

#include "Point.h"

struct ScreenState : public PointI {

	inline ScreenState() = default;
	inline ScreenState(PointI p) : PointI(p) {}
	inline ScreenState(int x, int y) : PointI(x, y) {}
	inline const int &operator[](const short i) const { return (&x)[i]; }
	inline int &operator[](const short i) { return (&x)[i]; }
	inline friend ScreenState operator+(const ScreenState &lhs, const ScreenState &rhs) { return ScreenState(lhs.x + rhs.x, lhs.y + rhs.y); }
	inline friend ScreenState operator-(const ScreenState &lhs, const ScreenState &rhs) { return ScreenState(lhs.x - rhs.x, lhs.y - rhs.y); }

	inline PointI mid_point() const { return ScreenState(x / 2, y / 2); }
	inline int mid_x_int() const { return x / 2; }
	inline int mid_y_int() const { return y / 2; }
	inline double mid_x_double() const { return x / 2.0; }
	inline double mid_y_double() const { return y / 2.0; }
	inline double aspect_ratio() const { return (double)x / (double)y; }
};
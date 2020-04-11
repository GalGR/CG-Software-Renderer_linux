#pragma once

struct Point {
	int x;
	int y;

	inline Point() = default;
	inline Point(int x, int y) : x(x), y(y) {}
	inline const int &operator[](const short i) const { return (&x)[i]; }
	inline int &operator[](const short i) { return (&x)[i]; }
	inline friend Point operator+(const Point &lhs, const Point &rhs) { return Point(lhs.x + rhs.x, lhs.y + rhs.y); }
	inline friend Point operator-(const Point &lhs, const Point &rhs) { return Point(lhs.x - rhs.x, lhs.y - rhs.y); }
};
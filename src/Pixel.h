#pragma once

#include "Point.h"
#include "Color.h"


struct Pixel : public Point
{
	Color color; //RGBA color format - 4 components of 8 bits each - 0xAABBGGRR - AA alpha, BB blue, GG green, RR red

	inline Pixel() = default;
	inline Pixel(int x, int y, UINT32 color) : Point(x, y) { this->color = color; }
	inline Pixel(int x, int y, Color color) : Point(x, y), color(color) {}
	inline Pixel(Point point, UINT32 color) : Point(point) { this->color = color; }
	inline Pixel(Point point, Color color) : Point(point), color(color) {}

	inline const int &operator[](const short i) const { return (&x)[i]; }
	inline int &operator[](const short i) { return (&x)[i]; }

	inline Pixel& operator=(Point point) { *((Point*)this) = point; return *this; }
	inline Pixel& operator=(Color color) { this->color = color; return *this; }
};



//struct RGBA {
//	UINT8 r;
//	UINT8 g;
//	UINT8 b;
//	UINT8 a;
//};
//
//union Color {
//	UINT32 data;
//	RGBA rgba;
//
//	Color& operator=(UINT32 data);
//	operator UINT32() const;
//};
//
//struct Point {
//	int x;
//	int y;
//
//	Point();
//	Point(int x, int y);
//	friend Point operator+(const Point &lhs, const Point &rhs);
//	friend Point operator-(const Point &lhs, const Point &rhs);
//};
//
//struct Pixel : public Point
//{
//	Color color; //RGBA color format - 4 components of 8 bits each - 0xAABBGGRR - AA alpha, BB blue, GG green, RR red
//
//	Pixel();
//	Pixel(int x, int y, UINT32 color);
//	Pixel(int x, int y, Color color);
//	Pixel(Point point, UINT32 color);
//	Pixel(Point point, Color color);
//
//	Pixel& operator=(Point point);
//	Pixel& operator=(Color color);
//};
#pragma once

#include "basetsd.h"
#include "Point.h"
#include "Color.h"


struct Pixel : public PointI
{
	Color color; //RGBA color format - 4 components of 8 bits each - 0xAABBGGRR - AA alpha, BB blue, GG green, RR red

	inline Pixel() = default;
	inline Pixel(int x, int y, UINT32 color) : PointI(x, y) { this->color = color; }
	inline Pixel(int x, int y, Color color) : PointI(x, y), color(color) {}
	inline Pixel(PointI point, UINT32 color) : PointI(point) { this->color = color; }
	inline Pixel(PointI point, Color color) : PointI(point), color(color) {}

	inline const int &operator[](const short i) const { return (&x)[i]; }
	inline int &operator[](const short i) { return (&x)[i]; }

	inline Pixel& operator=(PointI point) { *((PointI*)this) = point; return *this; }
	inline Pixel& operator=(Color color) { this->color = color; return *this; }
};
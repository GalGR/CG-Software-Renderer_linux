#include "Pixel.h"

// Color
Color& Color::operator=(UINT32 data) { this->data = data; return *this; }
Color::operator UINT32() const { return data; }


// Point
Point::Point() {}
Point::Point(int x, int y) : x(x), y(y) {}
Point operator+(const Point &lhs, const Point &rhs) { return Point(lhs.x + rhs.x, lhs.y + rhs.y); }
Point operator-(const Point &lhs, const Point &rhs) { return Point(lhs.x - rhs.x, lhs.y - rhs.y); }


// Pixel
Pixel::Pixel() {}
Pixel::Pixel(int x, int y, UINT32 color) : Point(x, y) { this->color = color; }
Pixel::Pixel(int x, int y, Color color) : Point(x, y), color(color) {}
Pixel::Pixel(Point point, UINT32 color) : Point(point) { this->color = color; }
Pixel::Pixel(Point point, Color color) : Point(point), color(color) {}

Pixel& Pixel::operator=(Point point) { *((Point*)this) = point; return *this; }
Pixel& Pixel::operator=(Color color) { this->color = color; return *this; }
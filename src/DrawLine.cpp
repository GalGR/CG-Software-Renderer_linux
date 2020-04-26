#include "DrawLine.h"
#include "Renderer.h"
#include "Pixel.h"
#include "ScreenPixels.h"

#include <vector>

static int dx;
static int dy;
static int dx2;
static int dy2;

static void draw_e_ne(const PointI &p1, const PointI &p2, const Color &color, ScreenPixels &pixels, double depth = NAN) {
	int d = -dx + dy2;
	int a = dy2;
	int b = -dx2 + dy2;
	Pixel pixel(p1, color);
	pixels.push_back(pixel, depth);
	while (pixel.x < p2.x) {
		++pixel.x;
		if (d < 0) {
			d += a;
		}
		else /* (d >= 0) */ {
			++pixel.y;
			d += b;
		}
		pixels.push_back(pixel, depth);
	}
}

static void draw_n_ne(const PointI &p1, const PointI &p2, const Color &color, ScreenPixels &pixels, double depth = NAN) {
	int d = -dx2 + dy;
	int a = -dx2;
	int b = -dx2 + dy2;
	Pixel pixel(p1, color);
	pixels.push_back(pixel, depth);
	while (pixel.y < p2.y) {
		++pixel.y;
		if (d > 0) {
			d += a;
		}
		else /* (d <= 0) */ {
			++pixel.x;
			d += b;
		}
		pixels.push_back(pixel, depth);
	}
}

static void draw_e_se(const PointI &p1, const PointI &p2, const Color &color, ScreenPixels &pixels, double depth = NAN) {
	int d = dx + dy2;
	int a = dy2;
	int b = dx2 + dy2;
	Pixel pixel(p1, color);
	pixels.push_back(pixel, depth);
	while (pixel.x < p2.x) {
		++pixel.x;
		if (d > 0) {
			d += a;
		}
		else /* (d <= 0) */ {
			--pixel.y;
			d += b;
		}
		pixels.push_back(pixel, depth);
	}
}

static void draw_s_se(const PointI &p1, const PointI &p2, const Color &color, ScreenPixels &pixels, double depth = NAN) {
	int d = dx2 + dy;
	int a = dx2;
	int b = dx2 + dy2;
	Pixel pixel(p1, color);
	pixels.push_back(pixel, depth);
	while (pixel.y > p2.y) {
		--pixel.y;
		if (d < 0) {
			d += a;
		}
		else /* (d >= 0) */ {
			++pixel.x;
			d += b;
		}
		pixels.push_back(pixel, depth);
	}
}

void calcLine(PointI p1, PointI p2, Color color, ScreenPixels &pixels, double depth) {
	// If the first point is after the second point swap between them
	if (p1.x > p2.x) {
		std::swap(p1, p2);
	}

	// Follow Midpoint algorithm
	dx2 = 2 * (dx = p2.x - p1.x);
	dy2 = 2 * (dy = p2.y - p1.y);
	if (dy >= 0) {
		if (dy <= dx) {
			draw_e_ne(p1, p2, color, pixels, depth);
		}
		else {
			draw_n_ne(p1, p2, color, pixels, depth);
		}
	}
	else {
		if (-dy <= dx) {
			draw_e_se(p1, p2, color, pixels, depth);
		}
		else {
			draw_s_se(p1, p2, color, pixels, depth);
		}
	}
}

void drawLine(PointI p1, PointI p2, Color color, ScreenPixels &pixels, double depth)
{
	Renderer renderer;
	
	calcLine(p1, p2, color, pixels, depth);

	// Finished calculating the pixels -- now drawing them
	renderer.drawScreenPixels(pixels);
}
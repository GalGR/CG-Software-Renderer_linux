#include "DrawPolygon.h"
#include "DrawLine.h"

//#define PIXELS_PUSH_BACK(PIXEL) pixels.push_back(PIXEL)
#define PIXELS_PUSH_BACK(PIXEL) ((void)0)

static Poly sortPoly(const Poly &poly) {
	Poly sorted_poly = poly;
	for (short i = 0; i < 3; ++i) {
		short max_i = i;
		for (short j = i + 1; j < 3; ++j) {
			if (sorted_poly[max_i].y < sorted_poly[j].y) max_i = j;
		}
		std::swap(sorted_poly[i], sorted_poly[max_i]);
	}
	return sorted_poly;
}

struct Line {
	int dx;
	int dy;
	int dx2;
	int dy2;
	int d;
	int a;
	int b;
	Pixel p;
};

static void draw_e_se(Line &line, const Color &color, ScreenPixels &pixels) {
	int &dx = line.dx;
	int &dy = line.dy;
	int &dx2 = line.dx2;
	int &dy2 = line.dy2;
	int &d = line.d;
	int &a = line.a;
	int &b = line.b;
	Pixel &pixel = line.p;

	//if (a < 0) {
	//	int d_div_a = 0;
	//	if (d > 0) d_div_a = d / a; // d divided by a rounded down
	//	pixel.x += d_div_a + 1;
	//	--pixel.y;
	//	d += d_div_a * a + b;
	//}
	while (d > 0 && a < 0) {
		++pixel.x;
		d += a;
	}
	/*else*/ /* (d <= 0) */ {
		++pixel.x;
		--pixel.y;
		d += b;
	}
	PIXELS_PUSH_BACK(pixel);
}

static void draw_s_se(Line &line, const Color &color, ScreenPixels &pixels) {
	int &dx = line.dx;
	int &dy = line.dy;
	int &dx2 = line.dx2;
	int &dy2 = line.dy2;
	int &d = line.d;
	int &a = line.a;
	int &b = line.b;
	Pixel &pixel = line.p;

	--pixel.y;
	if (d < 0) {
		d += a;
	}
	else /* (d >= 0) */ {
		++pixel.x;
		d += b;
	}
	PIXELS_PUSH_BACK(pixel);
}


static void draw_s_sw(Line &line, const Color &color, ScreenPixels &pixels) {
	int &dx = line.dx;
	int &dy = line.dy;
	int &dx2 = line.dx2;
	int &dy2 = line.dy2;
	int &d = line.d;
	int &a = line.a;
	int &b = line.b;
	Pixel &pixel = line.p;

	--pixel.y;
	if (d < 0) {
		d += a;
	}
	else /* (d >= 0) */ {
		--pixel.x;
		d += b;
	}
	PIXELS_PUSH_BACK(pixel);
}

static void draw_w_sw(Line &line, const Color &color, ScreenPixels &pixels) {
	int &dx = line.dx;
	int &dy = line.dy;
	int &dx2 = line.dx2;
	int &dy2 = line.dy2;
	int &d = line.d;
	int &a = line.a;
	int &b = line.b;
	Pixel &pixel = line.p;

	//if (a != 0) {
	//	int d_div_a = 0;
	//	if (d > 0) d_div_a = (int)std::abs(d / a); // d divided by a rounded down
	//	pixel.x -= d_div_a + 1;
	//	--pixel.y;
	//	d += d_div_a * a + b;
	//}
	while (d > 0 && a < 0) {
		--pixel.x;
		d += a;
	}
	/*else*/ /* (d <= 0) */ {
		--pixel.x;
		--pixel.y;
		d += b;
	}
	PIXELS_PUSH_BACK(pixel);
}

Line initStepLine(PointI p1, PointI p2, Color color, ScreenPixels &pixels) {
	Line line;

	int &dx = line.dx;
	int &dy = line.dy;
	int &dx2 = line.dx2;
	int &dy2 = line.dy2;
	int &d = line.d;
	int &a = line.a;
	int &b = line.b;
	Pixel &pixel = line.p;

	dx2 = 2 * (dx = p2.x - p1.x);
	dy2 = 2 * (dy = p2.y - p1.y);

	if (dx >= 0) {
		if (-dy <= dx) {
			d = dx + dy2;
			a = dy2;
			b = dx2 + dy2;
			pixel = Pixel(p1, color);
			PIXELS_PUSH_BACK(pixel);
		}
		else {
			d = dx2 + dy;
			a = dx2;
			b = dx2 + dy2;
			pixel = Pixel(p1, color);
			PIXELS_PUSH_BACK(pixel);
		}
	}
	else {
		if (-dy <= -dx) {
			d = dx - dy2;
			a = -dy2;
			b = dx2 - dy2;
			pixel = Pixel(p1, color);
			PIXELS_PUSH_BACK(pixel);
		}
		else {
			d = dx2 - dy;
			a = dx2;
			b = dx2 - dy2;
			pixel = Pixel(p1, color);
			PIXELS_PUSH_BACK(pixel);
		}
	}

	return line;
}

void calcStepLine(Line &line, Color color, ScreenPixels &pixels) {
	int &dx = line.dx;
	int &dy = line.dy;
	int &dx2 = line.dx2;
	int &dy2 = line.dy2;
	int &d = line.d;
	int &a = line.a;
	int &b = line.b;
	Pixel &pixel = line.p;

	if (dx >= 0) {
		if (-dy <= dx) {
			draw_e_se(line, color, pixels);
		}
		else {
			draw_s_se(line, color, pixels);
		}
	}
	else {
		if (-dy <= -dx) {
			draw_w_sw(line, color, pixels);
		}
		else {
			draw_s_sw(line, color, pixels);
		}
	}
}

Vector4 center_of_vertices(const Vertices &vertices) {
	Vector4 center = { 0.0, 0.0, 0.0 };
	for (short i = 0; i < 3; ++i) {
		assert(vertices[i][3] == 1.0);
		for (short j = 0; j < 3; ++j) {
			center[j] += vertices[i][j] / 3.0;
		}
	}
	return center;
}

//void calcFlatPolygon(const PolyData &poly_data, const Lighting &lighting, const Material &material, const Vector4 &face_normal, const Vector4 &cam_pos, ScreenPixels &pixels) {
//
//	// Calculate the center of mass (flat shading)
//	Vector4 center = center_of_vertices(poly_data.vertices);
//
//	// Calculate the color (flat shading)
//	Color color = Light::calc(lighting, material, face_normal, center, cam_pos);
//
//	Poly sorted_poly = sortPoly(poly_data.poly);
//
//	//PointI p3;
//	//p3.x = round(sorted_poly[0].x + ((double)(sorted_poly[1].y - sorted_poly[0].y) / (double)(sorted_poly[2].y - sorted_poly[0].y)) * (sorted_poly[2].x - sorted_poly[0].x));
//	//p3.y = sorted_poly[1].y;
//	//Poly upper_poly = { sorted_poly[0], sorted_poly[1], p3 };
//	//Poly lower_poly = { sorted_poly[1], p3, poly[2] };
//
//	if (sorted_poly[1].x <= sorted_poly[2].x) {
//		Poly poly = { sorted_poly[0], sorted_poly[1], sorted_poly[2] };
//
//		Line line_p0p1 = initStepLine(poly[0], poly[1], color, pixels);
//		Line line_p0p2 = initStepLine(poly[0], poly[2], color, pixels);
//
//		// Draw the upper part of the polygon
//		//while (line_p0p1.p.y > poly[1].y && line_p0p2.p.y > poly[1].y) {
//		//	calcStepLine(line_p0p1, color, pixels);
//		//	calcStepLine(line_p0p2, color, pixels);
//		//	calcLine(line_p0p1.p, line_p0p2.p, color, pixels);
//		//}
//		do {
//			if (line_p0p1.p.x >= 0 && line_p0p2.p.x >= 0) calcLine(line_p0p1.p, line_p0p2.p, color, pixels);
//			calcStepLine(line_p0p1, color, pixels);
//			calcStepLine(line_p0p2, color, pixels);
//		} while (line_p0p1.p.y > poly[1].y && line_p0p2.p.y > poly[1].y);
//
//		Line line_p1p2 = initStepLine(poly[1], poly[2], color, pixels);
//
//		// Draw the lower part of the polygon
//		//while (line_p1p2.p.y > poly[2].y && line_p0p2.p.y > poly[2].y) {
//		//	calcStepLine(line_p1p2, color, pixels);
//		//	calcStepLine(line_p0p2, color, pixels);
//		//	calcLine(line_p1p2.p, line_p0p2.p, color, pixels);
//		//}
//		while (line_p1p2.p.y > poly[2].y && line_p0p2.p.y > poly[2].y) {
//			if (line_p1p2.p.x >= 0 && line_p0p2.p.x >= 0) calcLine(line_p1p2.p, line_p0p2.p, color, pixels);
//			calcStepLine(line_p1p2, color, pixels);
//			calcStepLine(line_p0p2, color, pixels);
//		}
//		//do {
//		//	if (line_p1p2.p.x >= 0 && line_p0p2.p.x >= 0) calcLine(line_p1p2.p, line_p0p2.p, color, pixels);
//		//	calcStepLine(line_p1p2, color, pixels);
//		//	calcStepLine(line_p0p2, color, pixels);
//		//} while (line_p1p2.p.y > poly[2].y && line_p0p2.p.y > poly[2].y);
//	}
//	else {
//		Poly poly = { sorted_poly[0], sorted_poly[2], sorted_poly[1] };
//
//		Line line_p0p1 = initStepLine(poly[0], poly[1], color, pixels);
//		Line line_p0p2 = initStepLine(poly[0], poly[2], color, pixels);
//
//		// Draw the upper part of the polygon
//		//while (line_p0p1.p.y > poly[1].y && line_p0p2.p.y > poly[1].y) {
//		//	calcStepLine(line_p0p1, color, pixels);
//		//	calcStepLine(line_p0p2, color, pixels);
//		//	calcLine(line_p0p1.p, line_p0p2.p, color, pixels);
//		//}
//		do {
//			if (line_p0p1.p.x >= 0 && line_p0p2.p.x >= 0) calcLine(line_p0p1.p, line_p0p2.p, color, pixels);
//			calcStepLine(line_p0p1, color, pixels);
//			calcStepLine(line_p0p2, color, pixels);
//		} while (line_p0p1.p.y > poly[1].y && line_p0p2.p.y > poly[1].y);
//
//		Line line_p1p2 = initStepLine(poly[1], poly[2], color, pixels);
//
//		// Draw the lower part of the polygon
//		//while (line_p1p2.p.y > poly[2].y && line_p0p2.p.y > poly[2].y) {
//		//	calcStepLine(line_p1p2, color, pixels);
//		//	calcStepLine(line_p0p2, color, pixels);
//		//	calcLine(line_p1p2.p, line_p0p2.p, color, pixels);
//		//}
//		while (line_p1p2.p.y > poly[2].y && line_p0p2.p.y > poly[2].y) {
//			if (line_p1p2.p.x >= 0 && line_p0p2.p.x >= 0) calcLine(line_p1p2.p, line_p0p2.p, color, pixels);
//			calcStepLine(line_p1p2, color, pixels);
//			calcStepLine(line_p0p2, color, pixels);
//		}
//		//do {
//		//	if (line_p1p2.p.x >= 0 && line_p0p2.p.x >= 0) calcLine(line_p1p2.p, line_p0p2.p, color, pixels);
//		//	calcStepLine(line_p1p2, color, pixels);
//		//	calcStepLine(line_p0p2, color, pixels);
//		//} while (line_p1p2.p.y > poly[2].y && line_p0p2.p.y > poly[2].y);
//	}
//}

void calcBoundRect(const PolyData &poly_data, std::array<PointI, 2> &bound_rect) {
	for (short i = 0; i < 3; ++i) {
		if (poly_data.poly[i].x < bound_rect[0].x) {
			bound_rect[0].x = poly_data.poly[i].x;
		}
		if (poly_data.poly[i].y < bound_rect[0].y) {
			bound_rect[0].y = poly_data.poly[i].y;
		}
		if (poly_data.poly[i].x > bound_rect[1].x) {
			bound_rect[1].x = poly_data.poly[i].x;
		}
		if (poly_data.poly[i].y > bound_rect[1].y) {
			bound_rect[1].y = poly_data.poly[i].y;
		}
	}
}

//extern int iteration;
//void test_color(Color &color) {
//	switch (iteration) {
//	case 0:
//		color = RED;
//		break;
//	case 1:
//		color = GREEN;
//		break;
//	case 2:
//		color = BLUE;
//		break;
//	case 3:
//		color = YELLOW;
//		break;
//	case 4:
//		color = CYAN;
//		break;
//	case 5:
//		color = MAGENTA;
//		break;
//	default:
//		color = WHITE;
//		break;
//	}
//}

void calcFlatPolygon(const PolyData &poly_data, const Lighting &lighting, const Material &material, const Vector4 &face_normal, const Vector4 &cam_pos, ScreenPixels &pixels) {

	// Calculate the center of mass (flat shading)
	Vector4 center = center_of_vertices(poly_data.vertices);

	// Calculate the color (flat shading)
	Color color = Light::calc(lighting, material, face_normal, center, cam_pos);
	//Color color;
	//test_color(color);

	// Calculate the bounding rectangle of the polygon
	std::array<PointI, 2> bound_rect = { poly_data.poly[0], poly_data.poly[0] };
	calcBoundRect(poly_data, bound_rect);

	// Calculate each pixel in the polygon
	for (int y = bound_rect[0].y; y < bound_rect[1].y; ++y) {
		for (int x = bound_rect[0].x; x < bound_rect[1].x; ++x) {
			PointI p = { x, y };
			Bary bary(p, poly_data.poly);
			if (bary.isInside()) {
				double depth = bary(poly_data.zbuff); // Interpolate the depth value from each vertex
				Pixel pixel = { p, color };
				pixels.push_back(pixel, depth);
			}
		}
	}
}

void calcGouraudPolygon(const PolyData &poly_data, const Lighting &lighting, const Material &material, const Vector4 &face_normal, const Vector4 &cam_pos, ScreenPixels &pixels) {
	// Calculate the color for each vertex (Gouraud shading)
	std::array<Color, 3> colors;
	for (short i = 0; i < 3; ++i) {
		colors[i] = Light::calc(lighting, material, poly_data.norms[i], poly_data.vertices[i], cam_pos);
	}

	// Calculate the bounding rectangle of the polygon
	std::array<PointI, 2> bound_rect = { poly_data.poly[0], poly_data.poly[0] };
	calcBoundRect(poly_data, bound_rect);

	// Calculate each pixel in the polygon
	for (int y = bound_rect[0].y; y < bound_rect[1].y; ++y) {
		for (int x = bound_rect[0].x; x < bound_rect[1].x; ++x) {
			PointI p = { x, y };
			Bary bary(p, poly_data.poly);
			if (bary.isInside()) {
				double depth = bary(poly_data.zbuff); // Interpolate the depth value from each vertex
				Color color = bary(colors); // Interpolate the color from each vertex
				Pixel pixel = { p, color };
				pixels.push_back(pixel, depth);
			}
		}
	}
}

void calcPhongPolygon(const PolyData &poly_data, const Lighting &lighting, const Material &material, const Vector4 &face_normal, const Vector4 &cam_pos, ScreenPixels &pixels) {
	// Calculate the bounding rectangle of the polygon
	std::array<PointI, 2> bound_rect = { poly_data.poly[0], poly_data.poly[0] };
	calcBoundRect(poly_data, bound_rect);

	// Calculate each pixel in the polygon
	for (int y = bound_rect[0].y; y < bound_rect[1].y; ++y) {
		for (int x = bound_rect[0].x; x < bound_rect[1].x; ++x) {
			PointI p = { x, y };
			Bary bary(p, poly_data.poly);
			if (bary.isInside()) {
				double depth = bary(poly_data.zbuff); // Interpolate the depth value from each vertex
				Vector4 normal = bary(poly_data.norms); // Interpolate the normal from each vertex
				Vector4 pos = bary(poly_data.vertices); // Interpolate the position from each vertex
				Color color = Light::calc(lighting, material, normal, pos, cam_pos);
				Pixel pixel = { p, color };
				pixels.push_back(pixel, depth);
			}
		}
	}
}
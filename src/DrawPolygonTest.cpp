#include "DrawPolygon.h"

Vector4 center_of_vertices(const Vertices &vertices);
void calcBoundRect(const PolyData &poly_data, std::array<PointI, 2> &bound_rect);

extern int iteration;
void test_color(Color &color) {
	switch (iteration) {
	case 0:
		color = RED;
		break;
	case 1:
		color = GREEN;
		break;
	case 2:
		color = BLUE;
		break;
	case 3:
		color = YELLOW;
		break;
	case 4:
		color = CYAN;
		break;
	case 5:
		color = MAGENTA;
		break;
	default:
		color = WHITE;
		break;
	}
}

void calcFlatPolygonTest(const PolyData &poly_data, const Lighting &lighting, const Material &material, const Vector4 &face_normal, const Vector4 &cam_pos, ScreenPixels &pixels) {

	// Calculate the center of mass (flat shading)
	Vector4 center = center_of_vertices(poly_data.vertices);

	// Calculate the color (flat shading test)
	Color color;
	test_color(color);

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

void calcGouraudPolygonTest(const PolyData &poly_data, const Lighting &lighting, const Material &material, const Vector4 &face_normal, const Vector4 &cam_pos, ScreenPixels &pixels) {
	// Calculate the color for each vertex (Gouraud shading test)
	std::array<Color, 3> colors;
	int prev_iteration = iteration;
	for (short i = 0; i < 3; ++i) {
		test_color(colors[i]);
		iteration = (iteration + 1) % 6;
	}
	iteration = prev_iteration;

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
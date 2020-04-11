#pragma once

#include "Pixel.h"
#include "ScreenPixels.h"
#include "Vector4.h"
#include "Light.h"
#include "Barycentric.h"

typedef std::array<Point, 3> Poly;
typedef std::array<Vector4, 3> Vertices;
typedef std::array<Vector4, 3> Norms;
typedef std::array<double, 3> PolyZBuff;

struct PolyData {
	Poly poly;
	Vertices vertices;
	Norms norms;
	PolyZBuff zbuff;
};

void calcFlatPolygon(const PolyData &poly_data, const Lighting &lighting, const Material &material, const Vector4 &face_normal, const Vector4 &cam_pos, ScreenPixels &pixels);
void calcGouraudPolygon(const PolyData &poly_data, const Lighting &lighting, const Material &material, const Vector4 &face_normal, const Vector4 &cam_pos, ScreenPixels &pixels);
void calcPhongPolygon(const PolyData &poly_data, const Lighting &lighting, const Material &material, const Vector4 &face_normal, const Vector4 &cam_pos, ScreenPixels &pixels);

void calcFlatPolygonTest(const PolyData &poly_data, const Lighting &lighting, const Material &material, const Vector4 &face_normal, const Vector4 &cam_pos, ScreenPixels &pixels);
void calcGouraudPolygonTest(const PolyData &poly_data, const Lighting &lighting, const Material &material, const Vector4 &face_normal, const Vector4 &cam_pos, ScreenPixels &pixels);
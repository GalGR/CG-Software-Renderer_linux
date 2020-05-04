#pragma once

#include <vector>

#include "Object.h"
#include "Camera.h"
#include "Vector4.h"
#include "Vector3.h"
#include "Point.h"
#include "Pixel.h"
#include "Screen.h"
#include "ScreenPixels.h"
#include "Light.h"
#include "DrawBuffer.h"
#include "ShadingEnum.h"
#include "Variables.h"

class Draw {
public:
	//template <typename Vertices, typename Polygons>
	//static void calcPolygon(
	//	const Vertices &vertices,
	//	const Polygons &polygons,
	//	const Matrix4 &proj,
	//	DrawBuffer &polygons_buffer,
	//	ScreenPixels &pixels,
	//	const ScreenState &screen
	//);
	//static void drawScene(
	//	const Camera &camera,
	//	const std::vector<Object> &objects,
	//	const Matrix4 &proj,
	//	std::vector<bool> &need_draw,
	//	std::vector<Vector4> &points_3d,
	//	std::vector<PointI> &points_2d,
	//	std::vector<Pixel> &pixels,
	//	double screen_width,
	//	double screen_height,
	//	bool draw_bounding_box = false,
	//	bool draw_vertex_normals = false
	//);
	template <typename Vertices>
	static void calcProjection(
		const Vertices &vertices,
		const Matrix4 &trans,
		DrawBuffer &draw_buffer,
		const ScreenState &screen
	);
	static void drawScene(
		VarsUnique &uVars,
		VarsShared &sVars,
		ScreenPixels &pixels
	);
	static void drawScene(
		const Camera &camera,
		const Object &object,
		const Lighting &lighting,
		ShadingEnum shading_mode,
		DrawBuffer &mesh_buffer,
		DrawBuffer &bbox_buffer,
		DrawBuffer &normals_buffer,
		DrawBuffer &axes_buffer,
		ScreenPixels &pixels,
		const ScreenState &screen,
		bool draw_bounding_box = false,
		bool draw_vertex_normals = false,
		bool draw_world_axes = false,
		bool draw_object_axes = false,
		Color obj_color = WHITE,
		Color bbox_color = BLUE,
		Color normals_color = YELLOW
	);
	static void calcScene(
		const Camera &camera,
		const Object &object,
		const Lighting &lighting,
		ShadingEnum shading_mode,
		DrawBuffer &mesh_buffer,
		DrawBuffer &bbox_buffer,
		DrawBuffer &normals_buffer,
		DrawBuffer &axes_buffer,
		ScreenPixels &pixels,
		const ScreenState &screen,
		bool draw_bounding_box = false,
		bool draw_vertex_normals = false,
		bool draw_world_axes = false,
		bool draw_object_axes = false,
		Color obj_color = WHITE,
		Color bbox_color = BLUE,
		Color normals_color = YELLOW
	);
	static void calcObject(
		const Camera &camera,
		const Object &object,
		const Lighting &lighting,
		ShadingEnum shading_mode,
		const Matrix4 &trans,
		DrawBuffer &mesh_buffer,
		DrawBuffer &normals_buffer,
		ScreenPixels &pixels,
		const ScreenState &screen,
		Color color = WHITE
	);
	static void calcBoundingBox(
		const BoundingBox &bBox,
		const Matrix4 &trans,
		DrawBuffer &bbox_buffer,
		ScreenPixels &pixels,
		const ScreenState &screen,
		Color color = BLUE
	);
	static void calcVertexNormals(
		const MeshModel &meshModel,
		const Matrix4 &trans,
		DrawBuffer &mesh_buffer,
		DrawBuffer &normals_buffer,
		ScreenPixels &pixels,
		const ScreenState &screen,
		double length = 1.0,
		Color color = YELLOW
	);
	static void calcAxes(
		const Matrix4 &trans,
		DrawBuffer &axes_buffer,
		ScreenPixels &pixels,
		const ScreenState &screen,
		double length = 3.0, // Length for the axes
		std::array<Color, 3> colors = { RED, GREEN, BLUE } // x=RED, y=GREEN, z=BLUE
	);
};
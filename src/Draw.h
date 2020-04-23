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

// Shading mode enum
typedef enum { SHADING_WIRE, SHADING_FLAT, SHADING_GOURAUD, SHADING_PHONG, SHADING_FLAT_TEST, SHADING_GOURAUD_TEST, NUM_OF_SHADERS } ShadingEnum;

struct DrawBuffers {
	typedef std::vector<bool> NeedDraw;
	typedef std::vector<Vector3> Points3D;
	typedef std::vector<PointI> Points2D;

	NeedDraw need_draw;
	Points3D points_3d;
	Points2D points_2d;

	void clear_reserve_resize(size_t reserveAmount) {
		// Reserve the number of points
		need_draw.clear();
		if (need_draw.capacity() < reserveAmount) need_draw.reserve(reserveAmount);
		need_draw.resize(reserveAmount);

		// Reserve the number of 3D points
		points_3d.clear();
		if (points_3d.capacity() < reserveAmount) points_3d.reserve(reserveAmount);
		points_3d.resize(reserveAmount);

		// Reserve the number of 2D points
		points_2d.clear();
		if (points_2d.capacity() < reserveAmount) points_2d.reserve(reserveAmount);
		points_2d.resize(reserveAmount);
	}
};

class Draw {
public:
	//template <typename Vertices, typename Polygons>
	//static void calcPolygon(
	//	const Vertices &vertices,
	//	const Polygons &polygons,
	//	const Matrix4 &proj,
	//	DrawBuffers &polygons_buffer,
	//	ScreenPixels &pixels,
	//	const MyScreen &screen
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
		DrawBuffers &draw_buffers,
		const MyScreen &screen
	);
	static void drawScene(
		std::vector<Pixel> &list_pixels,
		const Camera &camera,
		const Object &object,
		const Lighting &lighting,
		ShadingEnum shading_mode,
		DrawBuffers &mesh_buffers,
		DrawBuffers &bbox_buffers,
		DrawBuffers &normals_buffers,
		DrawBuffers &axes_buffers,
		ScreenPixels &pixels,
		const MyScreen &screen,
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
		DrawBuffers &mesh_buffers,
		DrawBuffers &bbox_buffers,
		DrawBuffers &normals_buffers,
		DrawBuffers &axes_buffers,
		ScreenPixels &pixels,
		const MyScreen &screen,
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
		DrawBuffers &mesh_buffers,
		DrawBuffers &normals_buffers,
		ScreenPixels &pixels,
		const MyScreen &screen,
		Color color = WHITE
	);
	static void calcBoundingBox(
		const BoundingBox &bBox,
		const Matrix4 &trans,
		DrawBuffers &bbox_buffers,
		ScreenPixels &pixels,
		const MyScreen &screen,
		Color color = BLUE
	);
	static void calcVertexNormals(
		const MeshModel &meshModel,
		const Matrix4 &trans,
		DrawBuffers &mesh_buffers,
		DrawBuffers &normals_buffers,
		ScreenPixels &pixels,
		const MyScreen &screen,
		double length = 1.0,
		Color color = YELLOW
	);
	static void calcAxes(
		const Matrix4 &trans,
		DrawBuffers &axes_buffers,
		ScreenPixels &pixels,
		const MyScreen &screen,
		double length = 3.0, // Length for the axes
		std::array<Color, 3> colors = { RED, GREEN, BLUE } // x=RED, y=GREEN, z=BLUE
	);
};
#include "Draw.h"

#include "Renderer.h"

#include "DrawLine.h"
#include "DrawPolygon.h"
#include "Barycentric.h"

#include <cmath>

static Renderer renderer;

#define ROUND(X) floor(X)
#define INT_ROUND(X) (int)ROUND(X)

template <typename V>
static inline bool clip(const V &v) {
	return
		//abs(v[0]) <= abs(v[3]) &&
		//abs(v[1]) <= abs(v[3]) &&
		//abs(v[2]) <= abs(v[3]);
		abs(v[0]) < abs(v[3]) &&
		abs(v[1]) < abs(v[3]) &&
		abs(v[2]) < abs(v[3]);
}

template <typename V>
static inline PointI to_screen_coords(const V &v, const ScreenState &screen) {
	return PointI{
		INT_ROUND((v[0] + 1) * (double)(screen.x) / 2.0),
		INT_ROUND((v[1] + 1) * (double)(screen.y) / 2.0)
	};
}

template <typename V>
static inline std::array<double, 2> to_screen_coords_double(const V &v, const ScreenState &screen) {
	return std::array<double, 2>{
		((v[0] + 1) * (double)(screen.x) / 2.0),
		((v[1] + 1) * (double)(screen.y) / 2.0)
	};
}

static inline Vector4 to_view_coords(const PointI &p, const ScreenState &screen) {
	return Vector4{
		((double)p.x * 2.0 / (double)(screen.x)) - 1.0,
		((double)p.y * 2.0 / (double)(screen.y)) - 1.0
	};
}

template <typename Vertices>
void Draw::calcProjection(
	const Vertices &vertices,
	const Matrix4 &trans,
	DrawBuffer &draw_buffers,
	const ScreenState &screen
) {
	DrawBuffer::NeedDraw &need_draw = draw_buffers.need_draw;
	DrawBuffer::Points3D &points_3d = draw_buffers.points_3d;
	DrawBuffer::Points2D &points_2d = draw_buffers.points_2d;

	// Transform each 3D point and clip the coordinates outside the normalized box
	size_t numPoints = vertices.size();
	for (size_t i = 0; i < numPoints; ++i) {
		Vector4 point4d = trans * (Vector4)vertices[i]; // Transform each vertex
		need_draw[i] = clip(point4d); // Clip coordinates using the homogeneous coordinates
		points_3d[i] = point4d; // Transform into euclidean space (perspective division)
	}

	// Viewport transformation
	for (size_t i = 0; i < numPoints; ++i) {
		if (need_draw[i]) {
			points_2d[i] = to_screen_coords(points_3d[i], screen);
		}
	}
}

//void Draw::drawScene(
//	const Camera &camera,
//	const std::vector<Object> &objects,
//	const Matrix4 &proj,
//	std::vector<bool> &need_draw,
//	std::vector<Vector4> &points_3d,
//	std::vector<PointI> &points_2d,
//	std::vector<Pixel> &pixels,
//	double screen_width,
//	double screen_height,
//	bool draw_bounding_box,
//	bool draw_vertex_normals
//) {
//	size_t numObjects = objects.size();
//	for (size_t i = 0; i < numObjects; ++i) {
//		const Object &object = objects[i];
//		Draw::drawObject(
//			camera,
//			object,
//			proj,
//			need_draw,
//			points_3d,
//			points_2d,
//			pixels,
//			screen_width,
//			screen_height,
//			draw_bounding_box,
//			draw_vertex_normals
//		);
//	}
//}

void Draw::drawScene(
	const Camera &camera,
	const Object &object,
	const Lighting &lighting,
	ShadingEnum shading_mode,
	DrawBuffer &mesh_buffers,
	DrawBuffer &bbox_buffers,
	DrawBuffer &normals_buffers,
	DrawBuffer &axes_buffers,
	ScreenPixels &pixels,
	const ScreenState &screen,
	bool draw_bounding_box,
	bool draw_vertex_normals,
	bool draw_world_axes,
	bool draw_object_axes,
	Color obj_color,
	Color bbox_color,
	Color normals_color
) {
	// Reset the screen pixels buffer
	pixels.reset();

	// Calculate the scene
	calcScene(
		camera,
		object,
		lighting,
		shading_mode,
		mesh_buffers,
		bbox_buffers,
		normals_buffers,
		axes_buffers,
		pixels,
		screen,
		draw_bounding_box,
		draw_vertex_normals,
		draw_world_axes,
		draw_object_axes,
		obj_color,
		bbox_color,
		normals_color
	);

#ifdef SCREEN_PIXELS_IMPLEMENT_BUFFER
	renderer.drawScreenPixels(pixels);
#endif
}

void Draw::calcScene(
	const Camera &camera,
	const Object &object,
	const Lighting &lighting,
	ShadingEnum shading_mode,
	DrawBuffer &mesh_buffers,
	DrawBuffer &bbox_buffers,
	DrawBuffer &normals_buffers,
	DrawBuffer &axes_buffers,
	ScreenPixels &pixels,
	const ScreenState &screen,
	bool draw_bounding_box,
	bool draw_vertex_normals,
	bool draw_world_axes,
	bool draw_object_axes,
	Color obj_color,
	Color bbox_color,
	Color normals_color
) {
	// View-Projection matrix transformation (for world entities)
	Matrix4 trans = camera.projection() * camera.inv_view;

	// Draw the world axes
	pixels.clear();
	if (draw_world_axes) calcAxes(
		trans,
		axes_buffers,
		pixels,
		screen
	);
#ifdef SCREEN_PIXELS_IMPLEMENT_LIST
	// Render the pixels
	if (pixels.size() > 0) {
		renderer.drawPixels(pixels);
	}
#endif

	// Render the pixels and return if there is no object to render
	if (object.meshModel().vertices.size() == 0) return;

	// World-View-Projection matrix transformation (for object-world-bound entities)
	trans = trans * object.world;

	// Draw the object axes
	pixels.clear();
	if (draw_object_axes) calcAxes(
		trans,
		axes_buffers,
		pixels,
		screen
	);
#ifdef SCREEN_PIXELS_IMPLEMENT_LIST
	// Render the pixels
	if (pixels.size() > 0) {
		renderer.drawPixels(pixels);
	}
#endif

	// Model-View-Projection matrix transformation (for object-bound entities)
	trans = trans * object.model;

	// Draw the mesh model
	pixels.clear();
	calcObject(
		camera,
		object,
		lighting,
		shading_mode,
		trans,
		mesh_buffers,
		normals_buffers,
		pixels,
		screen,
		obj_color
	);
#ifdef SCREEN_PIXELS_IMPLEMENT_LIST
	// Render the pixels
	if (pixels.size() > 0) {
		renderer.drawPixels(pixels);
	}
#endif

	// Draw the bounding box
	pixels.clear();
	if (draw_bounding_box) calcBoundingBox(
		object.bBox,
		trans,
		bbox_buffers,
		pixels,
		screen,
		bbox_color
	);
#ifdef SCREEN_PIXELS_IMPLEMENT_LIST
	// Render the pixels
	if (pixels.size() > 0) {
		renderer.drawPixels(pixels);
	}
#endif

	// Draw the vertex normals
	pixels.clear();
	if (draw_vertex_normals) calcVertexNormals(
		object.meshModel(),
		trans,
		mesh_buffers,
		normals_buffers,
		pixels,
		screen,
		object.normals_length,
		normals_color
	);
#ifdef SCREEN_PIXELS_IMPLEMENT_LIST
	// Render the pixels
	if (pixels.size() > 0) {
		renderer.drawPixels(pixels);
	}
#endif
}

int iteration = 0;

void Draw::calcObject(
	const Camera &camera,
	const Object &object,
	const Lighting &lighting,
	ShadingEnum shading_mode,
	const Matrix4 &trans,
	DrawBuffer &mesh_buffers,
	DrawBuffer &normals_buffers,
	ScreenPixels &pixels,
	const ScreenState &screen,
	Color color
) {
	DrawBuffer::NeedDraw &need_draw = mesh_buffers.need_draw;
	DrawBuffer::Points3D &points_3d = mesh_buffers.points_3d;
	DrawBuffer::Points2D &points_2d = mesh_buffers.points_2d;

	const MeshModel &meshModel = object.meshModel();

	// Check if there is an object to render
	if (meshModel.vertices.size() == 0) return;

	// Calculate the projection
	calcProjection(meshModel.vertices, trans, mesh_buffers, screen);

	//// Calculate the projection for the normals
	//calcProjection(meshModel.normals, trans, normals_buffers, screen);

	// Calculate pixels
	//pixels.clear();
	//size_t numEdges = meshModel.edges.size();
	//for (size_t i = 0; i < numEdges; ++i) {
	//	const Edge &edge = meshModel.edges[i];
	//	if (need_draw[edge[0]] && need_draw[edge[1]]) {
	//		calcLine(points_2d[edge[0]], points_2d[edge[1]], color, pixels);
	//	}
	//}

	iteration = 0;
	size_t numFaces = meshModel.faces.size();
	for (size_t i = 0; i < numFaces; ++i) {
		const Face &face = meshModel.faces[i];
		if (need_draw[face[0]] && need_draw[face[1]] && need_draw[face[2]]) {
			// The polygon in 2D
			Poly poly = { points_2d[face[0]], points_2d[face[1]], points_2d[face[2]] };

			// The polygon in 3D
			Vertices vertices = { object.world * object.model * meshModel.vertices[face[0]], object.world * object.model * meshModel.vertices[face[1]], object.world * object.model * meshModel.vertices[face[2]] };

			// The normals
			Norms norms = { Matrix4::transpose(object.rot) * meshModel.normals[face[0]], Matrix4::transpose(object.rot) * meshModel.normals[face[1]], Matrix4::transpose(object.rot) * meshModel.normals[face[2]] };

			// The Z-buffer
			PolyZBuff zbuff = { points_3d[face[0]][2], points_3d[face[1]][2], points_3d[face[2]][2] };

			// Calculate the polygon
			PolyData poly_data = { poly, vertices, norms, zbuff };

			// Call the shading function
			switch (shading_mode) {
			case SHADING_WIRE:
				for (short i = 0; i < 2; ++i) {
					calcLine(poly[i], poly[i + 1], color, pixels, -INFINITY);
				}
				calcLine(poly[2], poly[0], color, pixels, -INFINITY);
				break;
			case SHADING_FLAT:
				calcFlatPolygon(poly_data, lighting, object.material, Matrix4::transpose(object.rot) * face.face_normal, camera.pos, pixels);
				break;
			case SHADING_GOURAUD:
				calcGouraudPolygon(poly_data, lighting, object.material, face.face_normal, camera.pos, pixels);
				break;
			case SHADING_PHONG:
				calcPhongPolygon(poly_data, lighting, object.material, face.face_normal, camera.pos, pixels);
				break;
			case SHADING_FLAT_TEST:
				calcFlatPolygonTest(poly_data, lighting, object.material, Matrix4::transpose(object.rot) * face.face_normal, camera.pos, pixels);
				break;
			case SHADING_GOURAUD_TEST:
				calcGouraudPolygonTest(poly_data, lighting, object.material, face.face_normal, camera.pos, pixels);
				break;
			}
		}
		iteration = (iteration + 1) % 6;
	}
}

void Draw::calcBoundingBox(
	const BoundingBox &bBox,
	const Matrix4 &trans,
	DrawBuffer &bbox_buffers,
	ScreenPixels &pixels,
	const ScreenState &screen,
	Color color
) {
	DrawBuffer::NeedDraw &need_draw = bbox_buffers.need_draw;
	DrawBuffer::Points3D &points_3d = bbox_buffers.points_3d;
	DrawBuffer::Points2D &points_2d = bbox_buffers.points_2d;

	// Calculate the bounding box vertices
	std::array<Vector4, 8> boxVertices = bBox.calcBoxVertices();

	// Calculate the projection
	calcProjection(boxVertices, trans, bbox_buffers, screen);

	/* Connection of the points in the bounding box:

				 max
		7---------6
	   /|        /|
	  3---------2 |
	  | |       | |
	  | |       | |
	  | 4-------|-5
	  |/        |/
	  0---------1
	 min

  y
  | z
  |/
  .----x

*/
	// Calculate pixels
	//pixels.clear();
	short i0, i1;
	i0 = 0; i1 = 1;
	if (need_draw[i0] && need_draw[i1]) {
		calcLine(points_2d[i0], points_2d[i1], color, pixels);
	}
	i0 = 1; i1 = 2;
	if (need_draw[i0] && need_draw[i1]) {
		calcLine(points_2d[i0], points_2d[i1], color, pixels);
	}
	i0 = 2; i1 = 3;
	if (need_draw[i0] && need_draw[i1]) {
		calcLine(points_2d[i0], points_2d[i1], color, pixels);
	}
	i0 = 3; i1 = 0;
	if (need_draw[i0] && need_draw[i1]) {
		calcLine(points_2d[i0], points_2d[i1], color, pixels);
	}
	i0 = 0; i1 = 4;
	if (need_draw[i0] && need_draw[i1]) {
		calcLine(points_2d[i0], points_2d[i1], color, pixels);
	}
	i0 = 1; i1 = 5;
	if (need_draw[i0] && need_draw[i1]) {
		calcLine(points_2d[i0], points_2d[i1], color, pixels);
	}
	i0 = 2; i1 = 6;
	if (need_draw[i0] && need_draw[i1]) {
		calcLine(points_2d[i0], points_2d[i1], color, pixels);
	}
	i0 = 3; i1 = 7;
	if (need_draw[i0] && need_draw[i1]) {
		calcLine(points_2d[i0], points_2d[i1], color, pixels);
	}
	i0 = 4; i1 = 5;
	if (need_draw[i0] && need_draw[i1]) {
		calcLine(points_2d[i0], points_2d[i1], color, pixels);
	}
	i0 = 5; i1 = 6;
	if (need_draw[i0] && need_draw[i1]) {
		calcLine(points_2d[i0], points_2d[i1], color, pixels);
	}
	i0 = 6; i1 = 7;
	if (need_draw[i0] && need_draw[i1]) {
		calcLine(points_2d[i0], points_2d[i1], color, pixels);
	}
	i0 = 7; i1 = 4;
	if (need_draw[i0] && need_draw[i1]) {
		calcLine(points_2d[i0], points_2d[i1], color, pixels);
	}
}

void Draw::calcVertexNormals(
	const MeshModel &meshModel,
	const Matrix4 &trans,
	DrawBuffer &mesh_buffers,
	DrawBuffer &normals_buffers,
	ScreenPixels &pixels,
	const ScreenState &screen,
	double length,
	Color color
) {
	// Calculate the normals' segments other end vertices
	size_t numVertices = meshModel.vertices.size();
	for (size_t i = 0; i < numVertices; ++i) {
		normals_buffers.points_3d[i] = Vector3::add(meshModel.normals[i] & length, meshModel.vertices[i]);
	}

	// Calculate the projection
	calcProjection(normals_buffers.points_3d, trans, normals_buffers, screen);

	// Calculate pixels
	//pixels.clear();
	for (size_t i = 0; i < numVertices; ++i) {
		if (mesh_buffers.need_draw[i] && normals_buffers.need_draw[i]) {
			calcLine(mesh_buffers.points_2d[i], normals_buffers.points_2d[i], color, pixels);
		}
	}
}

void Draw::calcAxes(
	const Matrix4 &trans,
	DrawBuffer &axes_buffers,
	ScreenPixels &pixels,
	const ScreenState &screen,
	double length,
	std::array<Color, 3> colors
) {
	// Calculate the coordinates
	std::array<Vector4, 4> xyz = {
		Vector4(length, 0.0, 0.0),
		Vector4(0.0, length, 0.0),
		Vector4(0.0, 0.0, length),
		Vector4(0.0, 0.0, 0.0)
	};

	// Calculate the projection
	calcProjection(xyz, trans, axes_buffers, screen);

	// Calculate pixels
	//pixels.clear();
	for (short i = 0; i < 3; ++i) {
		if (axes_buffers.need_draw[3] && axes_buffers.need_draw[i]) {
			calcLine(axes_buffers.points_2d[3], axes_buffers.points_2d[i], colors[i], pixels, -INFINITY);
		}
	}
}

//template <typename Vertices, typename Polygons>
//void Draw::calcPolygon(
//	const Vertices &vertices,
//	const Polygons &polygons,
//	const Matrix4 &proj,
//	DrawBuffer &polygons_buffer,
//	ScreenPixels &pixels,
//	const ScreenState &screen
//) {
//	
//}
#pragma once

struct DrawBuffer {
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
#pragma once

#include <array>
#include <mutex>
#include "Pack.h"

// Current DrawBuffers mesh_buffer, bbox_buffer, normals_buffer, axes_buffer
#define NUM_DRAW_BUFFERS 4
// The resizeable buffers must reside at the beginning of the DrawBufferArr::DrawBuffers s struct
#define NUM_RESIZEABLE_BUFFERS 2

struct DrawBuffer {
	typedef std::vector<bool> NeedDraw;
	typedef std::vector<Vector3> Points3D;
	typedef std::vector<PointI> Points2D;

	NeedDraw need_draw;
	Points3D points_3d;
	Points2D points_2d;

private:
	size_t reserveAmount_pending_;
	bool pending_ = false;
	std::mutex mutex_;
public:

	void resize(size_t reserveAmount) {
		// Reserve the number of points
		if (reserveAmount > need_draw.size()) need_draw.resize(reserveAmount);

		// Reserve the number of 3D points
		if (reserveAmount > points_3d.size()) points_3d.resize(reserveAmount);

		// Reserve the number of 2D points
		if (reserveAmount > points_2d.size()) points_2d.resize(reserveAmount);
	}

	// Sync functions
	void resize_pending(size_t reserveAmount) {
		{
			std::lock_guard lk(mutex_);
			reserveAmount_pending_ = reserveAmount;
			pending_ = true;
		}
	}
	void sync() {
		{
			std::lock_guard lk(mutex_);
			if (pending_) {
				this->resize(reserveAmount_pending_);
				pending_ = false;
			}
		}
	}
};

struct DrawBufferArr {
	struct DrawBuffers{
		// Resizeable buffers
		DrawBuffer mesh_buffer;
		DrawBuffer normals_buffer;

		// Non-resizeable buffers
		DrawBuffer bbox_buffer;
		DrawBuffer axes_buffer;

		// Update NUM_DRAW_BUFFERS if adding more DrawBuffers (either resizeable or not)
	} s;

private:
	std::mutex mutex_resize_;
public:

	inline const DrawBuffer &operator [](size_t i) const { return (&this->s.mesh_buffer)[i]; }
	inline DrawBuffer &operator [](size_t i) { return (&this->s.mesh_buffer)[i]; }

	void init(size_t reserveAmount, size_t bbox_reserveAmount, size_t axes_reserveAmount) {
		for (size_t i = 0; i < NUM_RESIZEABLE_BUFFERS; ++i) {
			(*this).resize(reserveAmount);
		}
		s.bbox_buffer.resize(bbox_reserveAmount);
		s.axes_buffer.resize(axes_reserveAmount);
	}

	void resize(size_t reserveAmount) {
		{
			std::lock_guard lk(mutex_resize_);
			for (size_t i = 0; i < NUM_RESIZEABLE_BUFFERS; ++i) {
				(*this)[i].resize(reserveAmount);
			}
		}
	}

	// Sync functions
	void resize_pending(size_t reserveAmount) {
		{
			std::lock_guard lk(mutex_resize_);
			for (size_t i = 0; i < NUM_RESIZEABLE_BUFFERS; ++i) {
				(*this)[i].resize_pending(reserveAmount);
			}
		}
	}
	void sync() {
		{
			std::lock_guard lk(mutex_resize_);
			for (size_t i = 0; i < NUM_RESIZEABLE_BUFFERS; ++i) {
				(*this)[i].sync();
			}
		}
	}
};
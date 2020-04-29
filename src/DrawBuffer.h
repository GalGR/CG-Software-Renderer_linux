#pragma once

#include <array>
#include <mutex>
#include "Pack.h"

// Current DrawBuffers mesh_buffer, bbox_buffer, normals_buffer, axes_buffer
#define NUM_DRAW_BUFFERS 4

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
		mutex_.lock();
		{
			reserveAmount_pending_ = reserveAmount;
			pending_ = true;
		}
		mutex_.lock();
	}
	void sync() {
		mutex_.lock();
		{
			if (pending_) {
				this->resize(reserveAmount_pending_);
				pending_ = false;
			}
		}
		mutex_.unlock();
	}
};

struct DrawBufferArr {
	struct DrawBuffers{
		DrawBuffer mesh_buffer;
		DrawBuffer normals_buffer;
		DrawBuffer bbox_buffer;
		DrawBuffer axes_buffer;
		// Update NUM_DRAW_BUFFERS if adding more DrawBuffers
	} s;

private:
	size_t reserveAmount_pending_;
	bool pending_ = false;
	std::mutex mutex_;
public:

	inline const DrawBuffer &operator [](size_t i) const { return (&this->s.mesh_buffer)[i]; }
	inline DrawBuffer &operator [](size_t i) { return (&this->s.mesh_buffer)[i]; }

	void resize(size_t reserveAmount) {
		for (size_t i = 0; i < NUM_DRAW_BUFFERS; ++i) {
			(*this)[i].resize(reserveAmount);
		}
	}

	// Sync functions
	void resize_pending(size_t reserveAmount) {
		mutex_.lock();
		{
			for (size_t i = 0; i < NUM_DRAW_BUFFERS; ++i) {
				(*this)[i].resize_pending(reserveAmount);
			}
			pending_ = true;
		}
		mutex_.unlock();
	}
	void sync() {
		mutex_.lock();
		{
			this->resize(reserveAmount_pending_);
			pending_ = false;
		}
		mutex_.unlock();
	}
};
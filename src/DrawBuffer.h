#pragma once

#include <array>

// Current DrawBuffers mesh_buffer, bbox_buffer, normals_buffer, axes_buffer
#define NUM_DRAW_BUFFERS 4

struct DrawBuffer {
	typedef std::vector<bool> NeedDraw;
	typedef std::vector<Vector3> Points3D;
	typedef std::vector<PointI> Points2D;

	NeedDraw need_draw;
	Points3D points_3d;
	Points2D points_2d;

	void resize(size_t reserveAmount) {
		// Reserve the number of points
		if (reserveAmount > need_draw.size()) need_draw.resize(reserveAmount);

		// Reserve the number of 3D points
		if (reserveAmount > points_3d.size()) points_3d.resize(reserveAmount);

		// Reserve the number of 2D points
		if (reserveAmount > points_2d.size()) points_2d.resize(reserveAmount);
	}
};

struct DrawBufferArr {
	union {
		struct {
			DrawBuffer mesh_buffer;
			DrawBuffer bbox_buffer;
			DrawBuffer normals_buffer;
			DrawBuffer axes_buffer;
			// Update NUM_DRAW_BUFFERS if adding more DrawBuffers
		};
		std::array<DrawBuffer, NUM_DRAW_BUFFERS> buffers;
	};
private:
	size_t reserveAmount_pending_;
	bool pending_;
public:

	inline const DrawBuffer &operator [](size_t i) const { return this->buffers[i]; }
	inline DrawBuffer &operator [](size_t i) { return this->buffers[i]; }

	void resize(size_t reserveAmount) {
		size_t numBuffers = this->buffers.size();
		for (size_t i = 0; i < numBuffers; ++i) {
			this->buffers[i].resize(reserveAmount);
		}
	}

	// Sync functions
	void resize_pending(size_t reserveAmount) {
		reserveAmount_pending_ = reserveAmount;
		pending_ = true;
	}
	void sync() {
		this->resize(reserveAmount_pending_);
	}
};
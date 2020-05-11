#pragma once

#include <mutex>

#include "Defines.h"
#include "Action.h"

#include "Screen.h"
#include "ScreenBuffers.h"
#include "ToggleMap.h"
#include "Camera.h"
#include "Object.h"
#include "MeshModel.h"
#include "Light.h"
#include "DrawBuffer.h"
#include "ShadingEnum.h"
#include "Color.h"

struct VarsShared {
	// Draw buffer array
	DrawBufferArr draw_arr;
	// Screen pixels buffers
	ScreenBuffers screen_buffers;
	// The object's mesh model
	MeshModel meshModel;
	// Material
	Material material;
	// Screen dimensions
	ScreenState screen = { START_WIDTH, START_HEIGHT };

private:
	std::mutex mutex_resize_;
public:

	void init() {
		{
			std::lock_guard<std::mutex> lk(mutex_resize_);
			draw_arr.init(START_WIDTH * START_HEIGHT, BOUNDING_BOX_VERTICES, WORLD_AXES_VERTICES);
			screen_buffers.init(START_WIDTH, START_HEIGHT, START_PIXELS_WIDTH, START_PIXELS_HEIGHT);
			screen.init(START_WIDTH, START_HEIGHT);
		}
	}

	void resize_screen(size_t width, size_t height) {
		{
			std::lock_guard<std::mutex> lk(mutex_resize_);
			screen_buffers.resize(width, height);
			screen.resize(width, height);
		}
	}

	void resize_screen_pending(size_t width, size_t height) {
		{
			std::lock_guard<std::mutex> lk(mutex_resize_);
			screen_buffers.resize_pending(width, height);
			screen.resize_pending(width, height);
		}
	}

	void sync() {
		{
			std::lock_guard<std::mutex> lk(mutex_resize_);
			draw_arr.sync();
			screen_buffers.sync();
			screen.sync();
		}
	}
};

struct VarsUnique {
	// Camera
	Camera camera;
	// Object
	Object object;
	// Lighting
	Lighting lighting;
	// Shading mode
	ShadingEnum shading_mode;
	// Actions state
	ToggleMap<Action> state;
	// Colors
	Color obj_color = WHITE;
	Color bbox_color = BLUE;
	Color normals_color = YELLOW;
	// Line Color
	Color color = WHITE;
};
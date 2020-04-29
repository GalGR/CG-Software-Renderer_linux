#pragma once

#include <vector>
#include <memory>

#include "Defines.h"
#include "Action.h"

#include "Screen.h"
#include "ScreenPixels.h"
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
	// Pixels screen pixels
	ScreenPixels pixels;
	// The object's mesh model
	MeshModel meshModel;
	// Screen dimensions
	ScreenState screen = { START_WIDTH, START_HEIGHT };

	void sync() {
		draw_arr.sync();
		pixels.sync();
		screen.sync();
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
	// Material
	Material material;
};
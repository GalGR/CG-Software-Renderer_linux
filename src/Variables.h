#pragma once

#include <vector>

#include "Defines.h"
#include "Action.h"

#include "Screen.h"
#include "ScreenPixels.h"
#include "ToggleMap.h"
#include "Scene.h"
#include "Camera.h"
#include "Object.h"
#include "Light.h"
#include "DrawBuffer.h"
#include "ShadingEnum.h"
#include "Color.h"

struct Variables {
	// Scene
	Scene scene;
	// Shading mode
	ShadingEnum shading_mode;
	// Draw buffer array
	DrawBufferArr draw_arr;
	// Screen dimensions
	ScreenState screen = { START_WIDTH, START_HEIGHT };
	// Actions state
	ToggleMap<Action> state;
	// Colors
	Color obj_color = WHITE;
	Color bbox_color = BLUE;
	Color normals_color = YELLOW;
	// Line Color
	Color color = WHITE;
	// Camera motion
	Motion cam_motion;
	// Object motion
	Motion obj_motion;
	// Object scale
	Motion obj_scale_motion; // Only use the x coordinate
	// Material
	Material material;
};
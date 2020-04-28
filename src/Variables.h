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
};
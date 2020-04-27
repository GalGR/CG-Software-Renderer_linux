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
#include "Draw.h"
#include "Color.h"

struct Variables {
	// Scene
	Scene scene;
	// Camera
	Camera &camera = scene.camera;
	// Object
	Object &object = scene.object;
	// Lighting
	Lighting &lighting = scene.lighting;
	// Shading mode
	ShadingEnum shading_mode;
	// Mesh draw buffers
	DrawBuffers mesh_buffers;
	// Bounding Box draw buffers
	DrawBuffers bbox_buffers;
	// Normals draw buffers
	DrawBuffers normals_buffers;
	// World/Object axes draw buffers
	DrawBuffers axes_buffers;
	// Screen dimensions
	ScreenState screen = { START_WIDTH, START_HEIGHT };
	// Actions state
	ToggleMap<Action> state;
	// Colors
	Color obj_color = WHITE;
	Color bbox_color = BLUE;
	Color normals_color = YELLOW;
};
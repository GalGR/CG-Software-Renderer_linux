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
	// Camera
	Camera &camera = scene.camera;
	// Object
	Object &object = scene.object;
	// Lighting
	Lighting &lighting = scene.lighting;
	// Shading mode
	ShadingEnum shading_mode;
	// Mesh draw buffers
	DrawBuffer mesh_buffer;
	// Bounding Box draw buffers
	DrawBuffer bbox_buffer;
	// Normals draw buffers
	DrawBuffer normals_buffer;
	// World/Object axes draw buffers
	DrawBuffer axes_buffer;
	// Screen dimensions
	ScreenState screen = { START_WIDTH, START_HEIGHT };
	// Actions state
	ToggleMap<Action> state;
	// Colors
	Color obj_color = WHITE;
	Color bbox_color = BLUE;
	Color normals_color = YELLOW;
};
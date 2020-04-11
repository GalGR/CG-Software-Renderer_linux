#pragma once

#include "Camera.h"
#include "Object.h"
#include "Light.h"

#include <vector>

struct Scene {
	Camera camera;
	Object object;
	Lighting lighting;
};
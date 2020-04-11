#pragma once

#include "Color.h"

struct Material {
	Color k_ambient;
	Color k_diffuse;
	Color k_specular;
	double n_specular;
};
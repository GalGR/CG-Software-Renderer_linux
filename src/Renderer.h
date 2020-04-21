#pragma once

#include <vector>
#include <Glew/include/GL/glew.h>
#include <glfw/include/GLFW/glfw3.h>

#include "Pixel.h"
#include "ScreenPixels.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	void drawPixels(const std::vector<Pixel>& pixels);
	void drawScreenPixels(const ScreenPixels &pixels);

protected:

};

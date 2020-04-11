#pragma once

#include <vector>
#include <freeglut/include/GL/freeglut.h>

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

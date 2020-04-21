#include "Renderer.h"

#include "ScreenPixels.h"


Renderer::Renderer()
{
}


Renderer::~Renderer()
{
}


//this function turns on the specified pixels on screen
void Renderer::drawPixels(const std::vector<Pixel>& pixels)
{
	int numPixels = pixels.size();

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_INT, sizeof(Pixel), &pixels[0].x);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Pixel), &pixels[0].color);

	glDrawArrays(GL_POINTS, 0, numPixels);
}

#if defined(SCREEN_PIXELS_IMPLEMENT_BUFFER)

//this function turns on the specified pixels on screen
void Renderer::drawScreenPixels(const ScreenPixels& pixels)
{
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, &pixels[0]);

	glDrawPixels(pixels.width(), pixels.height(), GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
}

#elif defined(SCREEN_PIXELS_IMPLEMENT_LIST)

//this function turns on the specified pixels on screen
void Renderer::drawScreenPixels(const ScreenPixels &pixels)
{
	int numPixels = pixels.size();

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_INT, sizeof(Pixel), &pixels[0]);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Pixel), &pixels[0].color);

	glDrawArrays(GL_POINTS, 0, numPixels);
}

#endif
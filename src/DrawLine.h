#pragma once

#include "Pixel.h"
#include "ScreenPixels.h"

void calcLine(Point p1, Point p2, Color color, ScreenPixels &pixels, double depth = NAN);
void drawLine(Point p1, Point p2, Color color, ScreenPixels &pixels, std::vector<Pixel> &list_pixels, double depth = NAN);
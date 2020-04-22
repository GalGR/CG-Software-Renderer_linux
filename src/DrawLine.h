#pragma once

#include "Pixel.h"
#include "ScreenPixels.h"

void calcLine(PointI p1, PointI p2, Color color, ScreenPixels &pixels, double depth = NAN);
void drawLine(PointI p1, PointI p2, Color color, ScreenPixels &pixels, std::vector<Pixel> &list_pixels, double depth = NAN);
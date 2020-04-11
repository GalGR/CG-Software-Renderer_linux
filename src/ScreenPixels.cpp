#include "ScreenPixels.h"

#ifdef SCREEN_PIXELS_BUFFER_IMPLEMENT

ScreenPixels::ScreenPixels() {
	width_ = 0;
	height_ = 0;
	reserve_width_ = SCREEN_PIXELS_DEFAULT_RESERVE_WIDTH;
	reserve_height_ = SCREEN_PIXELS_DEFAULT_RESERVE_HEIGHT;
	pixels_.reserve(SCREEN_PIXELS_DEFAULT_RESERVE_WIDTH * SCREEN_PIXELS_DEFAULT_RESERVE_HEIGHT);
	occupied_.reserve(SCREEN_PIXELS_DEFAULT_RESERVE_WIDTH * SCREEN_PIXELS_DEFAULT_RESERVE_HEIGHT);
	depth_.reserve(SCREEN_PIXELS_DEFAULT_RESERVE_WIDTH * SCREEN_PIXELS_DEFAULT_RESERVE_HEIGHT);
	reset();
}

ScreenPixels::ScreenPixels(size_t width, size_t height, size_t reserve_width, size_t reserve_height) {
	width_ = width;
	height_ = height;
	reserve_width_ = reserve_width;
	reserve_height_ = reserve_height;
	pixels_.reserve(reserve_width * reserve_height);
	occupied_.reserve(reserve_width * reserve_height);
	depth_.reserve(reserve_width * reserve_height);
	pixels_.resize(width_ * height_);
	occupied_.resize(width_ * height_);
	depth_.resize(width_ * height_);
	reset();
}

std::vector<Pixel> &ScreenPixels::getListPixels(std::vector<Pixel> &list_pixels) const {
	list_pixels.clear();
	for (size_t y = 0; y < height_; ++y) {
		for (size_t x = 0; x < width_; ++x) {
			if (getIsPixel(x, y)) {
				Pixel p = { (int)x, (int)y, getPixel(x, y) };
				list_pixels.push_back(p);
			}
		}
	}
	return list_pixels;
}

void ScreenPixels::reserve(size_t reserve_width, size_t reserve_height) {
	if (reserve_width * reserve_height >= reserve_width_ * reserve_height_) {
		reserve_width_ = reserve_width;
		reserve_height_ = reserve_height;
		pixels_.reserve(reserve_width * reserve_height);
		occupied_.reserve(reserve_width * reserve_height);
		depth_.reserve(reserve_width * reserve_height);
	}
}
void ScreenPixels::resize(size_t width, size_t height) {
	width_ = width;
	height_ = height;
	pixels_.resize(width_ * height_);
	occupied_.resize(width_ * height_);
	depth_.resize(width_ * height_);
	reserve(width, height);
}

#endif
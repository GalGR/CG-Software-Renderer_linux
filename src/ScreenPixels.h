#pragma once

#include "Color.h"
#include "Pixel.h"
#include <vector>
#include <assert.h>
#include <math.h>

#define SCREEN_PIXELS_DEFAULT_RESERVE_WIDTH 6'000
#define SCREEN_PIXELS_DEFAULT_RESERVE_HEIGHT 6'000

#define SCREEN_PIXELS_DEFAULT_NO_PIXEL ((UINT32)0x00'00'00'00)

class ScreenPixels {
	std::vector<Color> pixels_;
	std::vector<bool> occupied_;
	std::vector<double> depth_;
	size_t width_;
	size_t height_;
	size_t reserve_width_;
	size_t reserve_height_;
	size_t width_pending_;
	size_t height_pending_;
	bool pending_;

public:
	ScreenPixels();
	ScreenPixels(size_t width, size_t height, size_t reserve_width = SCREEN_PIXELS_DEFAULT_RESERVE_WIDTH, size_t reserve_height = SCREEN_PIXELS_DEFAULT_RESERVE_HEIGHT);

public:
	const Color &operator [](size_t i) const {
		assert(i < width_ * height_);
		return pixels_[i];
	}
	Color &operator [](size_t i) {
		assert(i < width_ * height_);
		return pixels_[i];
	}
	const Color &operator ()(size_t x, size_t y) const {
		assert(x < width_);
		assert(y < height_);
		return pixels_[y * width_ + x];
	}
	Color &operator ()(size_t x, size_t y) {
		assert(x < width_);
		assert(y < height_);
		return pixels_[y * width_ + x];
	}

private:
	void setDepth(size_t i, double depth) {
		assert(i < width_ * height_);
		depth_[i] = depth;
	}
	void setDepth(size_t x, size_t y, double depth) {
		assert(x < width_);
		assert(y < height_);
		depth_[y * width_ + x] = depth;
	}
	void setDepth(PointI p, double depth) { setDepth(p.x, p.y, depth); }
	double getDepth(size_t i) const {
		assert(i < width_ * height_);
		return depth_[i];
	}

	void setIsPixel(size_t i, bool val) {
		assert(i < width_ * height_);
		occupied_[i] = val;
	}
	void setIsPixel(size_t x, size_t y, bool val) {
		assert(x < width_);
		assert(y < height_);
		occupied_[y * width_ + x] = val;
	}
	void setIsPixel(PointI p, bool val) { setIsPixel(p.x, p.y, val); }
	bool getIsPixel(size_t i) const {
		assert(i < width_ * height_);
		return occupied_[i];
	}

public:
	bool getIsPixel(size_t x, size_t y) const {
		assert(x < width_);
		assert(y < height_);
		return occupied_[y * width_ + x];
	}
	bool getIsPixel(PointI p) const { return getIsPixel(p.x, p.y); }
	double getDepth(size_t x, size_t y) const {
		assert(x < width_);
		assert(y < height_);
		return depth_[y * width_ + x];
	}
	double getDepth(PointI p) const { return getDepth(p.x, p.y); }

	Color getPixel(size_t x, size_t y) const {
		return (*this)(x, y);
	}
	void setPixel(size_t x, size_t y, Color color, double depth = NAN) {
		if (!islessequal(getDepth(x, y), depth)) {
			(*this)(x, y) = color;
			setIsPixel(x, y, true);
			setDepth(x, y, depth);
		}
	}
	void setPixel(Pixel p, double depth = NAN) { setPixel(p.x, p.y, p.color, depth); }
	void push_back(Pixel p, double depth = NAN) { setPixel(p, depth); }

	void reset() {
		std::fill(pixels_.begin(), pixels_.end(), SCREEN_PIXELS_DEFAULT_NO_PIXEL);
		std::fill(occupied_.begin(), occupied_.end(), false);
		std::fill(depth_.begin(), depth_.end(), INFINITY);
	}
	void clear() {}

	size_t width() const { return width_; }
	size_t height() const { return height_; }
	size_t size() const { return width_ * height_; }

	std::vector<Pixel> &getListPixels(std::vector<Pixel> &list_pixels) const;

	void reserve(size_t reserve_width, size_t reserve_height);
	void resize(size_t width, size_t height);

	// Synchronization functions
	void resize_pending(size_t width, size_t height);
	void sync();
};
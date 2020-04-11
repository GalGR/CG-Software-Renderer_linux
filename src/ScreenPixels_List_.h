#pragma once

#include "Pixel.h"
#include <vector>

#define SCREEN_PIXELS_DEFAULT_RESERVE_WIDTH 8'000
#define SCREEN_PIXELS_DEFAULT_RESERVE_HEIGHT 8'000

struct ScreenPixels {
	std::vector<Pixel> pixels_;

	//ScreenPixels() {
	//	pixels_.reserve(SCREEN_PIXELS_DEFAULT_RESERVE_WIDTH * SCREEN_PIXELS_DEFAULT_RESERVE_HEIGHT);
	//}
	ScreenPixels() = default;
	ScreenPixels(size_t width, size_t height, size_t reserve_width = SCREEN_PIXELS_DEFAULT_RESERVE_WIDTH, size_t reserve_height = SCREEN_PIXELS_DEFAULT_RESERVE_HEIGHT) {
		size_t size = width * height;
		size_t reserve = reserve_width * reserve_height;
		size_t cap = (size > reserve) ? size : reserve;
		pixels_.reserve(cap);
	}

	operator std::vector<Pixel>&() { return pixels_; }
	operator const std::vector<Pixel>&() const { return pixels_; }

	const Pixel &operator [](size_t i) const {
		return pixels_[i];
	}
	Pixel &operator [](size_t i) {
		return pixels_[i];
	}

	void push_back(const Pixel &p) { pixels_.push_back(p); }

	void reset() {
		pixels_.clear();
	}
	void clear() {
		pixels_.clear();
		//size_t cap = SCREEN_PIXELS_DEFAULT_RESERVE_WIDTH * SCREEN_PIXELS_DEFAULT_RESERVE_HEIGHT;
		//if (pixels_.capacity() < cap) {
		//	pixels_.reserve(cap);
		//}
	}

	size_t size() { return pixels_.size(); }
	size_t capacity() { return pixels_.capacity(); }

	void reserve(size_t cap) { pixels_.reserve(cap); }
	void resize(size_t size) { pixels_.reserve(size); }
	void reserve(size_t cap_x, size_t cap_y) { if (cap_x * cap_y > pixels_.capacity()) pixels_.reserve(cap_x * cap_y); }
	void resize(size_t size_x, size_t size_y) { if (size_x * size_y > pixels_.capacity()) pixels_.reserve(size_x * size_y); }

	std::vector<Pixel> &getListPixels(std::vector<Pixel> &list_pixels) {
		pixels_.swap(list_pixels);
		return list_pixels;
	}
};
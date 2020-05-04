#pragma once

#include <array>
#include <mutex>

#include "ScreenPixels.h"

#define SCREEN_BUFFERS_SIZE 3

class ScreenBuffers {
	std::array<ScreenPixels, SCREEN_BUFFERS_SIZE> buffers;
	int curr = 0;
	int draw_curr = 2;

	inline int index(int i) const { return ((i % SCREEN_BUFFERS_SIZE) + SCREEN_BUFFERS_SIZE) % SCREEN_BUFFERS_SIZE; }
	inline int index(unsigned i) const { return (int)i % SCREEN_BUFFERS_SIZE; }

	std::mutex mutex_reset_;
	std::mutex mutex_resize_;
	std::mutex mutex_curr_;
	std::array<std::mutex, SCREEN_BUFFERS_SIZE> mutex_arr_;
public:

	const ScreenPixels &currDraw() const { return buffers[index(curr - 1)]; }
	ScreenPixels &currDraw() { return buffers[index(curr - 1)]; }

	const ScreenPixels &currCalc() const { return buffers[curr]; }
	ScreenPixels &currCalc() { return buffers[curr]; }

	void init(size_t width, size_t height, size_t reserve_width, size_t reserve_height) {
		size_t numBuffers = buffers.size();
		for (size_t i = 0; i < numBuffers; ++i) {
			this->buffers[i].reserve(reserve_width, reserve_height);
			this->buffers[i].resize(width, height);
			this->buffers[i].reset();
		}
	}

	void reset() {
		mutex_reset_.lock();
		{
			size_t numBuffers = buffers.size();
			for (size_t i = 0; i < numBuffers; ++i) {
				this->buffers[i].reset();
			}
		}
		mutex_reset_.unlock();
	}

	void next() {
		mutex_curr_.lock();
		{
			int next_ind = index((unsigned)curr + 1);
			mutex_arr_[next_ind].lock();
			{
				curr = next_ind;
			}
			mutex_arr_[next_ind].unlock();
		}
		mutex_curr_.unlock();
	}

	void drawLock() {
		mutex_curr_.lock();
		{
			draw_curr = index(curr - 1);
			mutex_arr_[draw_curr].lock();
		}
		mutex_curr_.unlock();
	}

	void drawUnlock() {
		mutex_arr_[draw_curr].unlock();
	}

	void resize(size_t width, size_t height) {
		mutex_resize_.lock();
		{
			for (size_t i = 0; i < SCREEN_BUFFERS_SIZE; ++i) {
				this->buffers[i].resize(width, height);
			}
		}
		mutex_resize_.unlock();
	}

	void resize_pending(size_t width, size_t height) {
		mutex_resize_.lock();
		{
			for (size_t i = 0; i < SCREEN_BUFFERS_SIZE; ++i) {
				this->buffers[i].resize_pending(width, height);
			}
		}
		mutex_resize_.unlock();
	}

	void sync() {
		mutex_resize_.lock();
		{
			for (size_t i = 0; i < SCREEN_BUFFERS_SIZE; ++i) {
				this->buffers[i].sync();
			}
		}
		mutex_resize_.unlock();
	}
};
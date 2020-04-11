#pragma once

#include "Pixel.h"

struct Mouse {
	struct Sensitivity {
		double x;
		double y;
	};

	Point prev;
	Point curr;
	Point move;
	Point rest;
	Sensitivity sensitivity = { 1.0 / 800.0, 1.0 / 800.0 };

	Mouse() = default;
	Mouse(Point rest) : curr(rest), move(0, 0), rest(rest) {}

	void update_rest(Point rest) {
		this->curr = rest;
		this->move = { 0, 0 };
		this->rest = rest;
	}
	void update_pos(int x, int y) {
		prev = curr;
		curr = { x, y };
		move = { curr.x - prev.x, curr.y - prev.y };
	}
	void reset() {
		curr = rest;
		move = { 0, 0 };
	}
};
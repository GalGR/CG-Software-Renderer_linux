#pragma once

#include "Point.h"
#include <Glew/include/GL/glew.h>
#include <glfw/include/GLFW/glfw3.h>

template <typename Pos_T>
struct Mouse_t {
	struct Sensitivity {
		double x;
		double y;
	};
	enum class State {
		NORMAL,
		HIDDEN,
		DISABLED,
		UNAVAILABLE,
	};

	typedef Point_t<Pos_T> Point_T;

	Point_T prev;
	Point_T curr;
	Point_T move;
	Point_T rest;
	Sensitivity sensitivity = { 1.0 / 800.0, 1.0 / 800.0 };
	GLFWwindow *window = NULL;
	State state;
	

	Mouse_t() = default;
	Mouse_t(Point_T rest) : curr(rest), move(0, 0), rest(rest) {}
	Mouse_t(Pos_T rest_x, Pos_T rest_y) : curr(Point_T(rest_x, rest_y)), move(0, 0), rest(Point_T(rest_x, rest_y)) {}
	Mouse_t(GLFWwindow *window) : window(window) {}
	Mouse_t(GLFWwindow *window, Point_T rest) : curr(rest), move(0, 0), rest(rest), window(window) {}
	Mouse_t(GLFWwindow *window, Pos_T rest_x, Pos_T rest_y) : curr(Point_T(rest_x, rest_y)), move(0, 0), rest(Point_T(rest_x, rest_y)), window(window) {}

	void update_rest(Point_T rest) {
		this->curr = rest;
		this->move = { 0, 0 };
		this->rest = rest;
	}
	void update_pos(Pos_T x, Pos_T y) {
		prev = curr;
		curr = { x, y };
		move = { curr.x - prev.x, curr.y - prev.y };
	}
	void reset() {
		curr = rest;
		move = { 0, 0 };
	}
	/**
	 * Makes the cursor invisible with locking to the confines of the window.
	 */
	void disable() {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		this->state = State::DISABLED;
	}
	/**
	 * Makes the cursor visible and without locking to the confines of the window.
	 */
	void normal() {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		this->state = State::NORMAL;
	}

private:
	void hidden() {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		this->state = State::HIDDEN;
	}
	void unavailable() {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_UNAVAILABLE);
		this->state = State::UNAVAILABLE;
	}
};

typedef Mouse_t<double> Mouse;
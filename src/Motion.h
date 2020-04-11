#pragma once

#include "Vector4.h"
#include <assert.h>

#define MAX_ACCEL 1.0
#define MAX_VELOC 1.0
#define GO_ACCEL 0.01
#define STOP_ACCEL -0.1

struct Motion {
	double g_a; // Go acceleration
	double s_a; // Stop acceleration

	double l_a = 0.0; // Left acceleration
	double r_a = 0.0; // Right acceleration
	double f_a = 0.0; // Forward acceleration
	double b_a = 0.0; // Backward acceleration
	double u_a = 0.0; // Up acceleration
	double d_a = 0.0; // Down acceleration

	double max_a; // Max acceleration

	double l_v = 0.0; // Left velocity
	double r_v = 0.0; // Right velocity
	double f_v = 0.0; // Forward velocity
	double b_v = 0.0; // Backward velocity
	double u_v = 0.0; // Up velocity
	double d_v = 0.0; // Down velocity

	double max_v; // Max velocity

	Motion(
		double go_accel = GO_ACCEL,
		double stop_accel = STOP_ACCEL,
		double max_accel = MAX_ACCEL,
		double max_veloc = MAX_VELOC
	) :
		g_a(go_accel),
		s_a(stop_accel),
		max_a(max_accel),
		max_v(max_veloc)
	{
		assert(go_accel > 0 && stop_accel < 0 && max_accel > 0 && max_veloc > 0);
	}

	void go_left() {
		l_a += g_a;
		l_a = (l_a > max_a) ? (max_a) : (l_a);
		l_v += l_a;
		l_v = (l_v > max_v) ? (max_v) : (l_v);
	}
	void go_right() {
		r_a += g_a;
		r_a = (r_a > max_a) ? (max_a) : (r_a);
		r_v += r_a;
		r_v = (r_v > max_v) ? (max_v) : (r_v);
	}
	void go_forward() {
		f_a += g_a;
		f_a = (f_a > max_a) ? (max_a) : (f_a);
		f_v += f_a;
		f_v = (f_v > max_v) ? (max_v) : (f_v);
	}
	void go_backward() {
		b_a += g_a;
		b_a = (b_a > max_a) ? (max_a) : (b_a);
		b_v += b_a;
		b_v = (b_v > max_v) ? (max_v) : (b_v);
	}
	void go_up() {
		u_a += g_a;
		u_a = (u_a > max_a) ? (max_a) : (u_a);
		u_v += u_a;
		u_v = (u_v > max_v) ? (max_v) : (u_v);
	}
	void go_down() {
		d_a += g_a;
		d_a = (d_a > max_a) ? (max_a) : (d_a);
		d_v += d_a;
		d_v = (d_v > max_v) ? (max_v) : (d_v);
	}

	void stop_left() {
		l_a = 0.0;
		l_v += s_a;
		l_v = (l_v < 0) ? (0) : (l_v);
	}
	void stop_right() {
		r_a = 0.0;
		r_v += s_a;
		r_v = (r_v < 0) ? (0) : (r_v);
	}
	void stop_forward() {
		f_a = 0.0;
		f_v += s_a;
		f_v = (f_v < 0) ? (0) : (f_v);
	}
	void stop_backward() {
		b_a = 0.0;
		b_v += s_a;
		b_v = (b_v < 0) ? (0) : (b_v);
	}
	void stop_up() {
		u_a = 0.0;
		u_v += s_a;
		u_v = (u_v < 0) ? (0) : (u_v);
	}
	void stop_down() {
		d_a = 0.0;
		d_v += s_a;
		d_v = (d_v < 0) ? (0) : (d_v);
	}

	Vector4 calcMove() const {
		return Vector4(r_v - l_v, u_v - d_v, b_v - f_v);
	}
};

#undef MAX_ACCEL
#undef MAX_VELOC
#undef GO_ACCEL
#undef STOP_ACCEL
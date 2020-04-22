#pragma once

#include "Vector4.h"
#include "Point.h"
#include <array>

class Bary {
private:
	std::array<double, 3> bary_;

	template <typename T>
	static inline double ROUND(T x) { return floor(x); }
	template <typename T>
	static inline int INT_ROUND(T x) { return (int)ROUND(x); }

	static double triangle_area_2(const std::array<PointI, 3> &poly) {
		Vector4 v01 = { (double)poly[1].x - poly[0].x, (double)poly[1].y - poly[0].y };
		Vector4 v02 = { (double)poly[2].x - poly[0].x, (double)poly[2].y - poly[0].y };
		return (v01 ^ v02).euclid_length();
	}

	bool isPositive() const {
		return bary_[0] >= 0 && bary_[0] <= 1 && bary_[1] >= 0 && bary_[1] <= 1 && bary_[2] >= 0 && bary_[2] <= 1;
	}
	bool isNegative() const {
		return bary_[0] <= 0 && bary_[0] >= -1 && bary_[1] <= 0 && bary_[1] >= -1 && bary_[2] <= 0 && bary_[2] >= -1;
	}

public:
	Bary() = default;
	Bary(const PointI &p, const std::array<PointI, 3> &poly) {
		double b0 = (poly[1].y - poly[2].y) * p.x + (poly[2].x - poly[1].x) * p.y + (poly[1].x * poly[2].y - poly[1].y * poly[2].x);
		double b1 = (poly[2].y - poly[0].y) * p.x + (poly[0].x - poly[2].x) * p.y + (poly[2].x * poly[0].y - poly[2].y * poly[0].x);
		double b2 = (poly[0].y - poly[1].y) * p.x + (poly[1].x - poly[0].x) * p.y + (poly[0].x * poly[1].y - poly[0].y * poly[1].x);

		double area2 = triangle_area_2(poly);

		bary_[0] = b0 / area2;
		bary_[1] = b1 / area2;
		bary_[2] = b2 / area2;

		if (isNegative()) {
			bary_[0] = -bary_[0];
			bary_[1] = -bary_[1];
			bary_[2] = -bary_[2];
		}
	}

	bool isInside() {
		return isPositive()/* || isNegative()*/;
	}

	Vector4 operator ()(const std::array<Vector4, 3> &vecs) {
		Vector4 mean;
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				mean[j] += vecs[i](j) * bary_[i];
			}
		}
		return mean;
	}
	PointI operator ()(const std::array<PointI, 3> &points) {
		PointI mean = { 0, 0 };
		for (int i = 0; i < 3; ++i) {
			mean.x += INT_ROUND(points[i].x * bary_[i]);
			mean.y += INT_ROUND(points[i].y * bary_[i]);
		}
		return mean;
	}
	double operator ()(const std::array<double, 3> &nums) {
		double mean = 0;
		for (int i = 0; i < 3; ++i) {
			mean += nums[i] * bary_[i];
		}
		return mean;
	}
	Color operator ()(const std::array<Color, 3> &colors) {
		Color mean = 0xff'00'00'00;
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				mean[j] += (UINT8)/*std::floor*/(/*std::max*/(/*255.0, */(double)colors[i][j] * bary_[i]));
			}
		}
		return mean;
	}

	Vector4 operator ()(const Vector4 &vec) const {
		Vector4 vec_euclid = Vector4::euclid(vec);
		double x = vec_euclid[0];
		double y = vec_euclid[1];
		double z = vec_euclid[2];
		return Vector4{
			x * bary_[0] + x * bary_[1] + x * bary_[2],
			y * bary_[0] + y * bary_[1] + y * bary_[2],
			z * bary_[0] + z * bary_[1] + z * bary_[2]
		};
	}
	PointI operator ()(const PointI &point) const {
		int x = point.x;
		int y = point.y;
		return PointI{
			INT_ROUND(x * bary_[0] + x * bary_[1] + x * bary_[2]),
			INT_ROUND(y * bary_[0] + y * bary_[1] + y * bary_[2])
		};
	}
};
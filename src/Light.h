#pragma once

#include <vector>
#include <memory>
#include <cmath>
#include <assert.h>

#include "Vector3.h"
#include "Material.h"

struct Light {
	double intensity;
	typedef Color (*calc_t)(const Light&, const Material&, const Vector3&, const Vector3&, const Vector3&);
	calc_t calc_f;

	Light(calc_t calc_f = NULL) : calc_f(calc_f) {}
	Light(double intensity, calc_t calc_f = NULL) : calc_f(calc_f), intensity(intensity) {}

	Color calc(const Material &material, const Vector3 &normal, const Vector3 &pos, const Vector3 &cam_pos) const {
		return this->calc_f(*this, material, normal, pos, cam_pos);
	}

	inline void setCalc(calc_t calc_f) { this->calc_f = calc_f; }
	template <typename Light_T> inline void setCalc() { this->calc_f = Light_T::calc; }

	inline void setIntensity(double intensity) { this->intensity = intensity; }

	inline const double &getIntensity() const { return this->intensity; }
	inline double &getIntensity() { return this->intensity; }

	template <typename LightsArr>
	static Color calc(const LightsArr &lights_arr, const Material &material, const Vector3 &normal, const Vector3 &pos, const Vector3 &cam_pos) {
		size_t numLights = lights_arr.size();
		Color mean_color = 0;
		for (size_t i = 0; i < numLights; ++i) {
			Color light_color = lights_arr[i].calc(material, normal, pos, cam_pos);
			light_color.a = 0xff;
			for (short j = 0; j < 3; ++j) {
				//mean_color[j] += (UINT8)((size_t)light_color[j] / numLights);
				mean_color[j] = (UINT8)(std::round(std::min(255.0, (double)mean_color[j] + (double)light_color[j])));
			}
		}
		return mean_color;
	}
};

struct VectorLight : Light {
	Vector3 vec;

	VectorLight(calc_t calc_f = NULL) : Light(calc_f) {}
	VectorLight(double intensity, calc_t calc_f = NULL) : Light(intensity, calc_f) {}
	VectorLight(const Vector3 &vec, calc_t calc_f = NULL) : vec(vec), Light(calc_f) {}
	VectorLight(const Vector3 &vec, double intensity, calc_t calc_f = NULL) : vec(vec), Light(intensity, calc_f) {}

	inline void setVector(const Vector3 &vec) { this->vec = vec; }

	inline const Vector3 &getVector() const { return this->vec; }
	inline Vector3 &getVector() { return this->vec; }
};

struct AmbientLight : Light {
	AmbientLight() : Light(this->calc) {}
	AmbientLight(double intensity) : Light(intensity, this->calc) {}

	static Color calc(const Light &light, const Material &material, const Vector3 &normal, const Vector3 &pos, const Vector3 &cam_pos) {
		Color color;
		for (short i = 0; i < 3; ++i) {
			color[i] = (UINT8)std::round(std::min(255.0, light.intensity * (double)material.k_ambient[i]));
		}
		return color;
	}
};

struct DirectionalLight : VectorLight {
	DirectionalLight() : VectorLight(this->calc) {}
	DirectionalLight(double intensity) : VectorLight(intensity, this->calc) {}
	DirectionalLight(const Vector3 &vec) : VectorLight(vec, this->calc) {}
	DirectionalLight(const Vector3 &vec, double intensity) : VectorLight(vec, intensity, this->calc) {}

	static Color calc(const Light &light, const Material &material, const Vector3 &normal, const Vector3 &pos, const Vector3 &cam_pos) {
		const VectorLight &vec_light = (const VectorLight&)light;
		Vector3 dir = Vector3::normal(vec_light.vec);
		Vector3 cam_normal = Vector3::normal(pos.to(cam_pos));
		//double cos_theta = std::abs(dir & normal);
		double cos_theta = std::max(0.0, -(dir * normal));
		Vector3 r_dir = /*Vector3::normal*/(Vector3::sub(dir, 2 * (/*std::abs*/(dir * normal) * normal)));
		const Vector3 &v_cam = cam_normal;
		//double cos_alpha = std::abs(r_dir & v_cam);
		double cos_alpha = std::max(0.0, r_dir * v_cam);
		Color color;
		for (short i = 0; i < 3; ++i) {
			color[i] = (UINT8)std::round(std::min(255.0, vec_light.intensity * cos_theta * (double)material.k_diffuse[i] + vec_light.intensity * std::pow(cos_alpha, material.n_specular) * (double)material.k_specular[i]));
		}
		return color;
	}
};

struct PointLight : VectorLight {
	PointLight() : VectorLight(this->calc) {}
	PointLight(double intensity) : VectorLight(intensity, this->calc) {}
	PointLight(const Vector3 &vec) : VectorLight(vec, this->calc) {}
	PointLight(const Vector3 &vec, double intensity) : VectorLight(vec, intensity, this->calc) {}

	static Color calc(const Light &light, const Material &material, const Vector3 &normal, const Vector3 &pos, const Vector3 &cam_pos) {
		const VectorLight &vec_light = (const VectorLight&)light;
		Vector3 cam_normal = Vector3::normal(pos.to(cam_pos));
		Vector3 dir = Vector3::normal(vec_light.vec.to(pos));
		//double cos_theta = std::abs(dir & normal);
		double cos_theta = std::max(0.0, -(dir & normal));
		Vector3 r_dir = /*Vector3::normal*/(Vector3::sub(dir, 2 * (/*std::abs*/(dir * normal) * normal)));
		const Vector3 &v_cam = cam_normal;
		//double cos_alpha = std::abs(r_dir & v_cam);
		double cos_alpha = std::max(0.0, r_dir * v_cam);
		Color color;
		for (short i = 0; i < 3; ++i) {
			color[i] = (UINT8)std::round(std::min(255.0, vec_light.intensity * cos_theta * (double)material.k_diffuse[i] + vec_light.intensity * std::pow(cos_alpha, material.n_specular) * (double)material.k_specular[i]));
		}
		return color;
	}
};

struct Lighting {
	std::vector<std::unique_ptr<Light>> lights;

	Lighting() = default;
	Lighting(const Lighting &lighting) {
		size_t numLights = lighting.lights.size();
		for (size_t i = 0; i < numLights; ++i) {
			this->lights.push_back(std::make_unique<Light>(lighting.lights[i]));
		}
	}

	void add(Light &&light) { lights.push_back(std::make_unique<Light>(light)); }
	void add(const Light &light) { lights.push_back(std::make_unique<Light>(light)); }

	void push_back(Light &&light) { this->add(std::move(light)); }
	void push_back(const Light &light) { this->add(light); }

	const Light &operator [](size_t i) const { return *lights[i]; }
	Light &operator [](size_t i) { return *lights[i]; }

	Lighting &operator =(const Lighting &lighting) {
		size_t numOtherLights = lighting.lights.size();
		size_t numThisLights = this->lights.size();
		{
			size_t i = 0;
			for (; i < numThisLights; ++i) {
				*this->lights[i] = *lighting.lights[i];
			}
			for (; i < numOtherLights; ++i) {
				this->lights.push_back(std::make_unique<Light>(lighting.lights[i]));
			}
		}
		return *this;
	}

	size_t size() const { return lights.size(); }
};
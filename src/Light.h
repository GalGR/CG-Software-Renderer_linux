#pragma once

#include <vector>

#include "Vector3.h"
#include "Material.h"

struct Light {
	double intensity;

	Light() = default;
	Light(double intensity) : intensity(intensity) {}

	virtual Color calc(const Material &material, const Vector3 &normal, const Vector3 &pos, const Vector3 &cam_pos) const = 0;

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

struct AmbientLight : public Light {
	AmbientLight() = default;
	AmbientLight(double intensity) : Light(intensity) {}
	virtual Color calc(const Material &material, const Vector3 &normal, const Vector3 &pos, const Vector3 &cam_pos) const override {
		Color color;
		for (short i = 0; i < 3; ++i) {
			color[i] = (UINT8)std::round(std::min(255.0, intensity * (double)material.k_ambient[i]));
		}
		return color;
	}
};

struct DirectionalLight : public Light {
	Vector3 dir;

	DirectionalLight() = default;
	DirectionalLight(double intensity) : Light(intensity) {}
	virtual Color calc(const Material &material, const Vector3 &normal, const Vector3 &pos, const Vector3 &cam_pos) const override {
		Vector3 dir = Vector3::normal(this->dir);
		Vector3 cam_normal = Vector3::normal(pos.to(cam_pos));
		//double cos_theta = std::abs(dir & normal);
		double cos_theta = std::max(0.0, -(dir * normal));
		Vector3 r_dir = /*Vector3::normal*/(Vector3::sub(dir, 2 * (/*std::abs*/(dir * normal) * normal)));
		const Vector3 &v_cam = cam_normal;
		//double cos_alpha = std::abs(r_dir & v_cam);
		double cos_alpha = std::max(0.0, r_dir * v_cam);
		Color color;
		for (short i = 0; i < 3; ++i) {
			color[i] = (UINT8)std::round(std::min(255.0, intensity * cos_theta * (double)material.k_diffuse[i] + intensity * std::pow(cos_alpha, material.n_specular) * (double)material.k_specular[i]));
		}
		return color;
	}
};

struct PointLight : public Light {
	Vector3 pos;

	PointLight() = default;
	PointLight(double intensity) : Light(intensity) {}
	virtual Color calc(const Material &material, const Vector3 &normal, const Vector3 &pos, const Vector3 &cam_pos) const override {
		Vector3 cam_normal = Vector3::normal(pos.to(cam_pos));
		Vector3 dir = Vector3::normal(this->pos.to(pos));
		//double cos_theta = std::abs(dir & normal);
		double cos_theta = std::max(0.0, -(dir & normal));
		Vector3 r_dir = /*Vector3::normal*/(Vector3::sub(dir, 2 * (/*std::abs*/(dir * normal) * normal)));
		const Vector3 &v_cam = cam_normal;
		//double cos_alpha = std::abs(r_dir & v_cam);
		double cos_alpha = std::max(0.0, r_dir * v_cam);
		Color color;
		for (short i = 0; i < 3; ++i) {
			color[i] = (UINT8)std::round(std::min(255.0, intensity * cos_theta * (double)material.k_diffuse[i] + intensity * std::pow(cos_alpha, material.n_specular) * (double)material.k_specular[i]));
		}
		return color;
	}
};

struct Lighting {
	AmbientLight *ambient_light;
	Light *light1;
	Light *light2;

	const Light &operator [](size_t i) const { return *((Light**)&ambient_light)[i]; }
	Light &operator [](size_t i) { return *((Light**)&ambient_light)[i]; }

	size_t size() const { return 3; }
};
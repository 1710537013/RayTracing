#pragma once
#include<memory>
#include <vector>
#include"glm/glm.hpp"

class Sphere {
public:
	glm::vec3 pos;
	glm::vec3 albedo;
	float radius;
	bool Hit(float& min_t, const Ray ray) const {
		glm::vec3 dir = glm::normalize(ray.dir);
		float a = glm::dot(dir, dir);
		glm::vec3 po = ray.pos - pos;
		float b = 2 * glm::dot(dir, po);
		float c = glm::dot(po, po) - radius * radius;

		float discriminant = b * b - 4 * a * c;
		if (discriminant < 0) {
			return false;
		}
		float t = (-b - sqrt(discriminant)) / (2.0f * a);

		if (t < -0.01f) {
			t = (-b + sqrt(discriminant)) / (2.0f * a);
			if (t < -0.01f)
				return false;
		}
		if (t > min_t) {
			return false;
		}
		min_t = t;
		return true;
	}
};
class Scene {
public:
	std::vector<Sphere> scenes;
	void Hit(float& min_t, const Ray ray, const Sphere*& closeSphere)const {
		for (const Sphere& sphere : scenes)
		{
			if (sphere.Hit(min_t, ray)) {
				closeSphere = &sphere;
			}
		}
	}
};
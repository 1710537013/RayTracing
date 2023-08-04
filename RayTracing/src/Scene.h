#pragma once
#include<memory>
#include <vector>
#include"glm/glm.hpp"
#include"m_Material.h"
class Material;
class Sphere {
public:
	glm::vec3 pos;
	float radius;
	int MatIndex;

public:
	bool Hit(float& min_t, const Ray ray) const {
		glm::vec3 dir = glm::normalize(ray.dir);
		float a = glm::dot(dir, dir);
		glm::vec3 po = ray.pos - pos;
		float b = glm::dot(dir, po);
		float c = glm::dot(po, po) - radius * radius;

		float discriminant = b * b -  a * c;
		if (discriminant < 0) {
			return false;
		}
		float t = (-b - sqrt(discriminant)) / a;

		if (t < 0.0001f) {
			t = (-b + sqrt(discriminant)) / a;
			if (t < 0.0001f)
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
	void Hit(float& min_t, const Ray ray, int& closeSphereIndex)const { 
		for (int i = 0; i < scenes.size(); i++)
		{
			if (scenes[i].Hit(min_t, ray)) {
				closeSphereIndex = i;
			}
		}
	}
public:
	std::vector<Sphere> scenes;
	std::vector<std::shared_ptr<Material>> Mat;
};
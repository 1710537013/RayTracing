#pragma once
#include"glm/glm.hpp"
#include "Ray.h"

class aabb {
public:
	aabb() {}
	aabb(const glm::vec3& _min, const glm::vec3& _max) :
		min(_min), max(_max) {}
	glm::vec3 get_min()const { return min; }
	glm::vec3 get_max()const { return max; }

	bool hit(const Ray& ray, float t_min, float t_max)const {
		for (int i = 0; i < 3; i++)
		{
			float div = 1.0 / ray.dir[i];
			float t0 = fmin((min[i] - ray.pos[i]) * div,
							(max[i] - ray.pos[i]) * div);
			float t1 = fmax((min[i] - ray.pos[i]) * div, 
							(max[i] - ray.pos[i]) * div);

			t_min = fmax(t0, t_min);
			t_max = fmin(t1, t_max);
			if (t_max <= t_min)		 //"=" :if wiped the edge, it is defined as missed  nan 与任何值比较都是false
			{
				
				return false;
			}
		}
		return true;
	}

public:
	glm::vec3 min;
	glm::vec3 max;
};

inline aabb surounding_box(aabb box0, aabb box1) {
	glm::vec3 small(fmin(box0.get_min().x, box1.get_min().x),
		fmin(box0.get_min().y, box1.get_min().y),
		fmin(box0.get_min().z, box1.get_min().z));
	glm::vec3 large(fmax(box0.get_max().x, box1.get_max().x),
		fmax(box0.get_max().y, box1.get_max().y),
		fmax(box0.get_max().z, box1.get_max().z));
	return aabb( small, large );
}

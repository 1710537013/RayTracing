#pragma once
#include"Scene.h"
#include<memory>
#include <iostream>

class BVH:public Hittable
{
public:
	BVH(){}
	BVH(const std::vector<std::shared_ptr<Hittable>>& objects):BVH(objects,0, objects.size()){}
	BVH(const std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end);

	virtual bool Hit(float t_min, float t_max, Payload& payload, const Ray& ray)const override;
	virtual bool bounding_box(aabb& output_box) const override;
public:
	std::shared_ptr<Hittable> left, right;
	aabb box;
};

inline bool box_compare(const std::shared_ptr<Hittable>a, const std::shared_ptr<Hittable> b,int axis) {
	aabb box_a, box_b;

	if (!a->bounding_box( box_a) || !b->bounding_box(box_b))
		std::cerr << "No bounding box in bvh_node constructor.\n";
	return box_a.get_min()[axis] < box_b.get_min()[axis];
}

inline bool box_x_compare(const std::shared_ptr<Hittable>a, const std::shared_ptr<Hittable> b) {
	return box_compare(a, b, 0);
}
inline bool box_y_compare(const std::shared_ptr<Hittable>a, const std::shared_ptr<Hittable> b) {
	return box_compare(a, b, 1);
}
inline bool box_z_compare(const std::shared_ptr<Hittable>a, const std::shared_ptr<Hittable> b) {
	return box_compare(a, b, 2);
}
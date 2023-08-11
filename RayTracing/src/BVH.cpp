#include "BVH.h"

BVH::BVH(const std::vector<std::shared_ptr<Hittable>>& _objects, size_t start, size_t end)
{
	// 构建二叉树
	auto tmp_obj = _objects;
	uint32_t axis = Walnut::Random::UInt(0, 2);
	
	auto comparator = (axis == 0) ? box_x_compare :
					  (axis == 1) ? box_y_compare : box_z_compare;

	size_t object_span = end - start;

	if (object_span == 1) {
		left = right = tmp_obj[start];
	}
	else if (object_span == 2) {
		if (comparator(tmp_obj[start], tmp_obj[start + 1]))
		{
			left = tmp_obj[start];
			right = tmp_obj[start+1];
		}
		else {
			left = tmp_obj[start+1];
			right = tmp_obj[start];
		}
	}
	else {
		// 左闭右开
		std::sort(tmp_obj.begin() + start, tmp_obj.begin() + end, comparator);
		size_t mid = start + object_span / 2;
		left = std::make_shared<BVH>(tmp_obj, start, mid);
		right= std::make_shared<BVH>(tmp_obj, mid, end);
	}
	aabb box_left, box_right;

	if (!left->bounding_box(box_left)
		|| !right->bounding_box(box_right)
		)
		std::cerr << "no bounding box in bvh_node constructor.\n";

	box = surounding_box(box_left, box_right);
}

bool BVH::Hit(float t_min, float t_max, Payload& payload, const Ray& ray) const
{
	//dont collider
	if (!box.hit(ray, t_min, t_max))
	{

		return false;
	}

	bool hit_left = left->Hit(t_min, t_max, payload, ray);
	bool hit_right = right->Hit(t_min, hit_left? payload.HitT: t_max, payload, ray);

	return hit_left || hit_right;
}

bool BVH::bounding_box(aabb& output_box) const
{
	output_box = box;
	return true;
}

#include"Scene.h"

bool Sphere::Hit(float t_min, float t_max, Payload& payload, const Ray& ray) const {
	glm::vec3 dir = glm::normalize(ray.dir);
	float a = glm::dot(dir, dir);
	glm::vec3 po = ray.pos - pos;
	float b = glm::dot(dir, po);
	float c = glm::dot(po, po) - radius * radius;

	float discriminant = b * b - a * c;
	if (discriminant < 0) {
		return false;
	}
	float t = (-b - sqrt(discriminant)) / a;

	if (t < t_min || t> t_max) {
		t = (-b + sqrt(discriminant)) / a;
		if (t < t_min || t> t_max)
			return false;
	}


	payload.HitT = t;
	payload.worldPos = ray.pos + ray.dir * t;
	payload.worldNoraml = (payload.worldPos - pos) / radius;
	payload.mat = mat;
	// 注意是以球心为原点 所以传 法线
	GetUV(payload.worldNoraml, payload.u, payload.v);
	return true;
}

void Sphere::GetUV(const glm::vec3& p, float& u, float& v)const
{
	float PI = 3.1415926f;
	// [-π，π]， 一般不用 atan（）只返回一四象限即[-π/2, π/2]
	float phi = atan2(p.z, p.x);
	// [-π/2. π/2]
	float theta = asin(p.y);
	// [0,1] -> [1,0] 
	u = 1.0F - (phi + PI) / (2 * PI);
	v = (theta + PI / 2) / PI;
}


bool Scene::Hit(float t_min, float t_max, Payload& payload, const Ray& ray) const
{
	if (scenes.size() == 0)
	{
		return false;
	}
	float tmp = t_max;
	Payload rec_tmp;
	bool isHit = false;
	for (int i = 0; i < scenes.size(); i++)
	{
		bool flag = scenes[i]->Hit(t_min, tmp, rec_tmp, ray);
		if (flag) {
			payload = rec_tmp;
			tmp = payload.HitT;
			isHit = true;
		}
	}
	return isHit;
}

bool Sphere::bounding_box(aabb& output_box) const {
	output_box = aabb(
		pos - glm::vec3(radius, radius, radius),
		pos + glm::vec3(radius, radius, radius));
	return true;
}


// 构造层次包围盒
bool Scene::bounding_box(aabb& output_box) const
{
	if (scenes.empty())	return false;

	aabb temp_box;
	bool first_box = true;
	for (const auto& object : scenes)
	{
		if (!object->bounding_box(temp_box)) return false;

		output_box = first_box ? temp_box : surounding_box(output_box, temp_box);
		first_box = false;
	}
	return true;
}

bool xy_rect::Hit(float t_min, float t_max, Payload& payload, const Ray& ray) const
{
	float t = (z - ray.pos.z) / ray.dir.z;
	if (t > t_max || t < t_min)
		return false;
	float x = ray.pos.x + t * ray.dir.x;
	float y = ray.pos.y + t * ray.dir.y;
	if (x > x1 || x< x0 || y>y1 || y < y0)
		return false;

	payload.HitT = t;
	payload.mat = mat;
	payload.u = (x - x0) / (x1 - x0);
	payload.v = (y - y0) / (y1 - y0);
	payload.worldPos = glm::vec3(x, y, z);

	payload.worldNoraml = (glm::dot(ray.dir, glm::vec3(0, 0, 1)) < 0) ? glm::vec3(0, 0, 1) : glm::vec3(0, 0, -1);

	return true;
}

bool xy_rect::bounding_box(aabb& output_box) const
{
	output_box = aabb(glm::vec3(x0, y0, z - 0.001f), glm::vec3(x1, y1, z + 0.001f));
	return true;
}

bool yz_rect::Hit(float t_min, float t_max, Payload& payload, const Ray& ray) const {
	float t = (x - ray.pos.x) / ray.dir.x;
	if (t > t_max || t < t_min)
		return false;
	float y = ray.pos.y + t * ray.dir.y;
	float z = ray.pos.z + t * ray.dir.z;
	if (y > y1 || y< y0 || z>z1 || z < z0)
		return false;

	payload.HitT = t;
	payload.mat = mat;
	payload.u = (z - z0) / (z1 - z0);
	payload.v = (y - y0) / (y1 - y0);
	payload.worldPos = glm::vec3(x, y, z);

	payload.worldNoraml = (glm::dot(ray.dir, glm::vec3(1, 0, 0)) < 0) ? glm::vec3(1, 0, 0) : glm::vec3(-1, 0, 0);

	return true;
}
bool yz_rect::bounding_box(aabb& output_box) const {
	output_box = aabb(glm::vec3(x - 0.001f, y0, z0), glm::vec3(x + 0.001f, y1, z1));
	return true;
}
bool xz_rect::Hit(float t_min, float t_max, Payload& payload, const Ray& ray) const {
	float t = (y - ray.pos.y) / ray.dir.y;
	if (t > t_max || t < t_min)
		return false;
	float x = ray.pos.x + t * ray.dir.x;
	float z = ray.pos.z + t * ray.dir.z;
	if (x > x1 || x< x0 || z>z1 || z < z0)
		return false;

	payload.HitT = t;
	payload.mat = mat;
	payload.u = (x - x0) / (x1 - x0);
	payload.v = (z - z0) / (z1 - z0);
	payload.worldPos = glm::vec3(x, y, z);

	payload.worldNoraml = (glm::dot(ray.dir, glm::vec3(0, 1, 0)) < 0) ? glm::vec3(0, 1, 0) : glm::vec3(0, -1, 0);

	return true;
}
bool xz_rect::bounding_box(aabb& output_box) const {
	output_box = aabb(glm::vec3(x0, y - 0.001f, z0), glm::vec3(x1 , y + 0.001f, z1));
	return true;
}


bool translate::Hit(float t_min, float t_max, Payload& payload, const Ray& r) const {
	Ray moved_r;
	moved_r.pos = r.pos - offset;
	moved_r.dir = r.dir;
	if (!ptr->Hit(t_min, t_max, payload, moved_r))
		return false;

	payload.worldNoraml += offset;

	return true;
}

bool translate::bounding_box(aabb& output_box) const {
	if (!ptr->bounding_box( output_box))
		return false;

	output_box = aabb(
		output_box.min + offset,
		output_box.max + offset);

	return true;
}

box::box(const glm::vec3& min, const glm::vec3& max, std::shared_ptr<Material> _mat) {
	box_min = min;
	box_max = max;
	objs.scenes.push_back(std::make_shared<xy_rect>(min.x, max.x, min.y, max.y, min.z,_mat));
	objs.scenes.push_back(std::make_shared<xy_rect>(min.x, max.x, min.y, max.y, max.z, _mat));

	objs.scenes.push_back(std::make_shared<yz_rect>( min.y, max.y, min.z,max.z, min.x, _mat));
	objs.scenes.push_back(std::make_shared<yz_rect>(min.y, max.y, min.z, max.z, max.x, _mat));

	objs.scenes.push_back(std::make_shared<xz_rect>(min.x, max.x, min.z, max.z, max.y, _mat));
	objs.scenes.push_back(std::make_shared<xz_rect>(min.x, max.x, min.z, max.z, max.y, _mat));
	
}

bool box::Hit(float t_min, float t_max, Payload& payload, const Ray& ray) const
{
	return objs.Hit(t_min, t_max, payload, ray);
}

bool box::bounding_box(aabb& output_box) const
{
	output_box = aabb(box_min, box_max);
	return true;
}



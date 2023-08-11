#pragma once
#include<memory>
#include <vector>
#include"glm/glm.hpp"
#include"m_Material.h"
#include "aabb.h"
//
class Material;
struct Payload;
class Hittable {
public:
	virtual bool Hit(float t_min, float t_max,Payload& payload, const Ray& ray) const = 0;
	virtual bool bounding_box(aabb& output_box) const = 0;
};

class Sphere:public Hittable {
public:
	Sphere(glm::vec3 _pos, float _radius, std::shared_ptr< Material> _mat) :pos(_pos), radius(_radius), mat(_mat){}
	glm::vec3 pos;
	float radius;
	std::shared_ptr< Material> mat;
public:
	virtual bool Hit(float t_min, float t_max, Payload& payload, const Ray& ray) const override;
	virtual bool bounding_box(aabb& output_box) const override;

private:
	void GetUV(const glm::vec3& p, float& u, float& v)const;
};
class xy_rect :public Hittable {
public:
	xy_rect(float _x0, float _x1, float _y0, float _y1, float _z, std::shared_ptr<Material> _mat):
	x0(_x0),x1(_x1),y0(_y0),y1(_y1),z(_z),mat(_mat)
	{}
	virtual bool Hit(float t_min, float t_max, Payload& payload, const Ray& ray) const override;
	virtual bool bounding_box(aabb& output_box) const override;

private:
	std::shared_ptr <Material> mat;
	float x0, x1, y0, y1, z;
};
class yz_rect :public Hittable {
public:
	yz_rect(float _y0, float _y1, float _z0, float _z1, float _x, std::shared_ptr<Material> _mat) :
		x(_x), y0(_y0), y1(_y1), z0(_z0), z1(_z1), mat(_mat)
	{}
	virtual bool Hit(float t_min, float t_max, Payload& payload, const Ray& ray) const override;
	virtual bool bounding_box(aabb& output_box) const override;

private:
	std::shared_ptr <Material> mat;
	float x, y0, y1, z0, z1;
};
class xz_rect :public Hittable {
public:
	xz_rect(float _x0, float _x1,  float _z0, float _z1, float _y, std::shared_ptr<Material> _mat) :
		x0(_x0), x1(_x1), y(_y), z0(_z0), z1(_z1), mat(_mat)
	{}
	virtual bool Hit(float t_min, float t_max, Payload& payload, const Ray& ray) const override;
	virtual bool bounding_box(aabb& output_box) const override;

private:
	std::shared_ptr <Material> mat;
	float x0, x1, y, z0, z1;
};

class translate : public Hittable {
public:
	translate(std::shared_ptr<Hittable> p, const glm::vec3& displacement)
		: ptr(p), offset(displacement) {}

	virtual bool Hit(float t_min, float t_max, Payload& payload, const Ray& ray) const override;
	virtual bool bounding_box( aabb& output_box) const override;

public:
	std::shared_ptr<Hittable> ptr;
	glm::vec3 offset;
};


/************************************** Scene  ****************************************************/
class Scene {
public:
	Scene() {}
	Scene(std::shared_ptr<Hittable> obj) { scenes.push_back(obj); }
	bool Hit(float t_min, float t_max, Payload& payload, const Ray& ray)const;
	bool bounding_box(aabb& output_box) const;
public:
	std::vector<std::shared_ptr<Hittable>> scenes;
};


class box:public Hittable {
public:
	box(const glm::vec3& min, const glm::vec3& max,std::shared_ptr<Material> _mat); 

	virtual bool Hit(float t_min, float t_max, Payload& payload, const Ray& ray) const ;
	virtual bool bounding_box(aabb& output_box) const ;
private:
	glm::vec3 box_min, box_max;
	Scene objs;
};

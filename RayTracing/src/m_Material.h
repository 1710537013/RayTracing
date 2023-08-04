#pragma once
#include"glm/glm.hpp"
#include "Ray.h"
#include "Render.h"

struct Payload;
class Material {
public:
	glm::vec3 albedo{ 1.0f };
	float Metalic = 0.0f;
public:
	Material(glm::vec3 al) :albedo(al){}
	Material() {}
	virtual bool Shading(Ray& ray, Payload& payload, glm::vec3& attenuation)const = 0;
};

class Lambert :public Material {
public:
	Lambert(glm::vec3 al): Material(al){}
	virtual bool Shading(Ray& ray, Payload& payload, glm::vec3& attenuation) const override ;
};
class Metal : public Material {
public:
	float Roughness = 1.0f;
public:
	Metal(glm::vec3 al, float fuzz) : Material(al), Roughness(fuzz) {}
	Metal() {}
	virtual bool Shading(Ray& ray, Payload& payload, glm::vec3& attenuation) const override;
};

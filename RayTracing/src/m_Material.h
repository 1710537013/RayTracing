#pragma once
#include"glm/glm.hpp"
#include "Ray.h"
#include "Render.h"
#include"texture.h"
//
struct Payload;

class Material {
public:
	float Metalic = 0.0f;
public:
	virtual bool Shading(Ray& ray, Payload& payload, glm::vec3& attenuation)const = 0;
	virtual glm::vec3 emitted(float u, float v, const glm::vec3& p)const {
		return glm::vec3(0, 0, 0);
	}
};

class Lambert :public Material {
public:
	Lambert(const glm::vec3& a) : albedo(std::make_shared<solid_color>(a)) {}
	Lambert(std::shared_ptr<texture> a) : albedo(a) {}
	virtual bool Shading(Ray& ray, Payload& payload, glm::vec3& attenuation) const override ;

private:
	std::shared_ptr<texture> albedo;
};
class Metal : public Material {
public:
	glm::vec3 albedo;
	float Roughness = 1.0f;
public:
	Metal(const glm::vec3& al, float fuzz) : albedo(al), Roughness(fuzz) {}
	Metal() {}
	virtual bool Shading(Ray& ray, Payload& payload, glm::vec3& attenuation) const override;
};

class Light :public Material {
public:
	Light(const glm::vec3& a):albedo(std::make_shared<solid_color>(a)){}
	virtual bool Shading(Ray& ray, Payload& payload, glm::vec3& attenuation)const override;
	virtual glm::vec3 emitted(float u, float v, const glm::vec3& p)const {
		return albedo->value(u,v,p);
	}
private:
	std::shared_ptr<texture> albedo;
};

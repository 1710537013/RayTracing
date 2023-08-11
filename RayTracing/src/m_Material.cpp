#include "m_Material.h"

bool Metal::Shading(Ray& ray, Payload& payload, glm::vec3& attenuation)const
{
	glm::vec3 reflect = glm::normalize(glm::reflect(ray.dir, payload.worldNoraml));
	ray.dir = glm::normalize(reflect + Walnut::Random::InUnitSphere() * Roughness);
	ray.pos = payload.worldPos;
	attenuation = albedo;
	return (glm::dot(ray.dir, payload.worldNoraml) > 0);
}

bool Lambert::Shading(Ray& ray, Payload& payload, glm::vec3& attenuation)const
{
	ray.dir = glm::normalize(payload.worldNoraml+ Walnut::Random::InUnitSphere());
	ray.pos = payload.worldPos;
	attenuation = albedo->value(payload.u, payload.v, payload.worldPos);
	return true;
}

bool Light::Shading(Ray& ray, Payload& payload, glm::vec3& attenuation)const {
	return false;
}

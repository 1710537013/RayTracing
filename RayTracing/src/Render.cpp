#include "Render.h"

namespace Utility {
	uint32_t RGBA2Uint32(const glm::vec4& color) {
		uint8_t R = (color.r * 255);
		uint8_t G = (color.g * 255);
		uint8_t B = (color.b * 255);
		uint8_t A = (color.a * 255);
		uint32_t rgba = R | (G << 8) | (B << 16) | (A << 24);
		return rgba;
	}
}

void Renderer::Resize(uint32_t width, uint32_t height) {

	if (m_ImageData)
	{
		if (width == m_Image->GetWidth() && height == m_Image->GetHeight())
			return;
		m_Image->Resize(width, height);
	}
	else {
		m_Image = std::make_shared<Image>(width, height, ImageFormat::RGBA);
	}
	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];
}
void Renderer::Render(const Scene& m_scene, const Camera& cam) {

	Ray ray;
	for (int y = 0; y < m_Image->GetHeight(); y++)
	{
		for (int x = 0; x < m_Image->GetWidth(); x++)
		{
			ray.dir = cam.GetRayDirections()[y * m_Image->GetWidth() + x];
			ray.pos = cam.GetPosition();

			glm::vec4 color = glm::clamp(RayTrace(m_scene, ray), glm::vec4(0.0f), glm::vec4(1.0f));;

			m_ImageData[y * m_Image->GetWidth() + x] = Utility::RGBA2Uint32(color);
		}
	}
	m_Image->SetData(m_ImageData);
}

void Renderer::SetLight(const glm::vec3& lightColor, const glm::vec3& lightPos)
{
	this->lightColor = lightColor;
	this->lightPos = lightPos;
}

glm::vec4 Renderer::RayTrace(const Scene& m_scene, const Ray& ray)
{
	/**
	* D^2*t^2 + 2*D*(P-O)*t + (P-O)^2 - R^2 = 0
	**/
	if (m_scene.scenes.size() == 0)
		return glm::vec4(0,0,0,1); 

	glm::vec3 dir = glm::normalize(ray.dir);
	glm::vec3 hitPostion;
	float min_t= FLT_MAX;		
	const Sphere* closeSphere = nullptr;
	m_scene.Hit(min_t, ray, closeSphere);
	if(!closeSphere) 
		return glm::vec4(0, 0, 0, 1);

	hitPostion = ray.pos + dir * min_t;

	//Sphere Shading
	glm::vec3 Difcolor(1.0f, 1.0f, 1.0f);
	glm::vec3 normal = glm::normalize(hitPostion - closeSphere->pos);

	glm::vec3 lightDir = glm::normalize(hitPostion - lightPos);
	float diffuse = glm::max(0.0f, glm::dot(-lightDir, normal));
	glm::vec3 lambert = diffuse * lightColor*closeSphere->albedo;


	return glm::vec4(lambert, 1.0f);
}


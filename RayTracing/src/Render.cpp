#include "Render.h"
#include <execution>

namespace Utility {
	uint32_t RGBA2Uint32(glm::vec4 color) {
		color.r = sqrt(color.r);
		color.g = sqrt(color.g);
		color.b = sqrt(color.b);
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

	delete[] m_ImageAccumulateData;
	m_ImageAccumulateData = new glm::vec4[width * height];

	m_ImageHorizontalIter.resize(width);
	m_ImageVerticalIter.resize(height);
	for (uint32_t i = 0; i < width; i++)
		m_ImageHorizontalIter[i] = i;
	for (uint32_t i = 0; i < height; i++)
		m_ImageVerticalIter[i] = i;
}
void Renderer::Render(const Scene& m_scene, const Camera& cam) {
	scene = &m_scene;
	camera = &cam;

	if (m_FrameIndex == 1)
		memset(m_ImageAccumulateData, 0, m_Image->GetWidth() * m_Image->GetHeight() * sizeof(glm::vec4));

#define MT 1
#if MT
	std::for_each(std::execution::par, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(), [this](uint32_t y) {
		std::for_each(std::execution::par, m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(), [this, y](uint32_t x) {
			Ray ray;
			ray.dir = glm::normalize(camera->GetRayDirections()[y * m_Image->GetWidth() + x]);
			ray.pos = camera->GetPosition();

			m_ImageAccumulateData[y * m_Image->GetWidth() + x] += glm::vec4(PerPixel(ray, bounces), 1.0f);

			glm::vec4 m_ImageAccumulateColor = m_ImageAccumulateData[y * m_Image->GetWidth() + x];
			m_ImageAccumulateColor /= (float)m_FrameIndex;

			glm::vec4 color = glm::clamp(m_ImageAccumulateColor, glm::vec4(0.0f), glm::vec4(1.0f));;
			m_ImageData[y * m_Image->GetWidth() + x] = Utility::RGBA2Uint32(color);

			});
		});
#else
	for (int y = 0; y < m_Image->GetHeight(); y++)
	{
		for (int x = 0; x < m_Image->GetWidth(); x++)
		{
			Ray ray;
			ray.dir = glm::normalize(camera->GetRayDirections()[y * m_Image->GetWidth() + x]);
			ray.pos = camera->GetPosition();

			int bouncesTmp = bounces;
			m_ImageAccumulateData[y * m_Image->GetWidth() + x] += glm::vec4(PerPixel(ray, bouncesTmp), 1.0f);

			glm::vec4 m_ImageAccumulateColor = m_ImageAccumulateData[y * m_Image->GetWidth() + x];
			m_ImageAccumulateColor /= (float)m_FrameIndex;

			glm::vec4 color = glm::clamp(m_ImageAccumulateColor, glm::vec4(0.0f), glm::vec4(1.0f));;
			m_ImageData[y * m_Image->GetWidth() + x] = Utility::RGBA2Uint32(color);
		}
	}
#endif // MT
	m_Image->SetData(m_ImageData);

	if (m_setting.Accumulate)
		m_FrameIndex++;
	else
		m_FrameIndex = 1;
}



void Renderer::SetLight(const glm::vec3& lightColor, const glm::vec3& lightPos)
{
	this->lightColor = lightColor;
	this->lightPos = lightPos;
}
glm::vec3 Renderer::PerPixel(Ray ray, int bounces)
{
	if (bounces <= 0)
		return glm::vec3(0, 0, 0);
	Payload payload;
	payload = RayTrace(ray);
	if (payload.HitT < 0.0f)
	{
		glm::vec3 unit_direction = glm::normalize(ray.dir);
		float t = 0.5 * (unit_direction.y + 1.0);
		return (1.0f - t) * glm::vec4(1.0, 1.0, 1.0, 1.0f) + t * glm::vec4(lightColor, 1.0f);
	}

	const Sphere& obj = scene->scenes[payload.objIndex];
	const std::shared_ptr<Material> mat = scene->Mat[obj.MatIndex];

	glm::vec3 attenuation;
	if (mat->Shading(ray, payload, attenuation))
		return attenuation * mat->albedo * PerPixel(ray, bounces - 1);
	return glm::vec3(0, 0, 0);
}

Payload Renderer::RayTrace(const Ray& ray)
{
	/**
	* D^2*t^2 + 2*D*(P-O)*t + (P-O)^2 - R^2 = 0
	**/
	if (scene->scenes.size() == 0)
		return Miss();

	float min_t = FLT_MAX;
	int closeSphereIndex = -1;
	scene->Hit(min_t, ray, closeSphereIndex);

	if (closeSphereIndex < 0)
		return Miss();
	return CloseHit(ray, min_t, closeSphereIndex);
}


Payload Renderer::Miss()
{
	Payload payload;
	payload.HitT = -1.0f;
	return payload;
}

Payload Renderer::CloseHit(const Ray& ray, float min_t, int closeSphereIndex)
{
	Payload payload;
	payload.worldPos = ray.pos + ray.dir * min_t;
	payload.objIndex = closeSphereIndex;
	payload.HitT = min_t;
	payload.worldNoraml = glm::normalize(payload.worldPos - scene->scenes[closeSphereIndex].pos);

	return payload;
}


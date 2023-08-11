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
			glm::vec3 camDir = glm::normalize(camera->GetRayDirections()[y * m_Image->GetWidth() + x]);
#define aperture 0
#if aperture 
			glm::vec3 u = glm::normalize(glm::cross(glm::vec3(0, 1.0f, 0), camDir));
			glm::vec3 v = glm::normalize(glm::cross(camDir, u));
			glm::vec3 rd = Walnut::Random::InUnitSphere() * 0.005f;
			glm::vec3 offset = rd.x * u + rd.y * v;

			//ray.pos = camera->GetPosition();// +offset.x + offset.y;
			ray.pos = camera->GetPosition() + offset.x + offset.y;
			//ray.dir = glm::normalize(camDir);// -offset.x - offset.y);
			ray.dir = glm::normalize(camDir - offset.x - offset.y);
#else 
			ray.pos = camera->GetPosition();
			ray.dir = glm::normalize(camDir);
#endif

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
		return glm::vec3(0.0, 0.0, 0.0);

	glm::vec3 attenuation;
	glm::vec3 emitted = payload.mat->emitted(payload.u, payload.v, payload.worldPos);
	if (!payload.mat->Shading(ray, payload, attenuation))
		return  emitted;
	return emitted+ attenuation* PerPixel(ray, bounces - 1);
}

Payload Renderer::RayTrace(const Ray& ray)
{
	Payload rec_payload;
	if (scene->Hit(0.001f, FLT_MAX, rec_payload, ray))
		return rec_payload;
	else {
		rec_payload.HitT = -1.0f;
		return rec_payload;
	}
}
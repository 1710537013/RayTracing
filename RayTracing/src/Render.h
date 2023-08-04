#pragma once
#include<memory>
#include <Walnut/Image.h>
#include<Walnut/Random.h>
#include <imgui.h>
#include"glm/glm.hpp"
#include "Camera.h"
#include "Ray.h"
#include"Scene.h"
using namespace Walnut;

struct Payload{
	glm::vec3 worldPos;
	glm::vec3 worldNoraml;
	float HitT;

	int objIndex;
};

class Renderer
{
public:
	struct Settings
	{
		bool Accumulate = true;
	};
public:
	Renderer() = default;

	void Resize(uint32_t width, uint32_t height);
	void Render(const Scene& m_scene, const Camera& cam);
	void SetLight(const glm::vec3& lightColor, const glm::vec3& lightPos);
	std::shared_ptr<Image> GetImage()const { return m_Image; }

	glm::vec3 PerPixel(Ray ray, int bounces);
	Payload RayTrace(const Ray& ray);
	Payload Miss();
	Payload CloseHit(const Ray& ray, float min_t, int closeSphereInde);
	void ResetFrameIndex() { m_FrameIndex = 1; }
	Settings& GetSettings() { return m_setting; }
private:


private:
	glm::vec3 lightColor = { 1.0f,1.0f,1.0f };
	glm::vec3 lightPos = { 1.0f,1.0f,1.0f };
	const Camera* camera;
	const Scene* scene;
	std::shared_ptr<Image> m_Image;
	uint32_t* m_ImageData = nullptr;

	std::vector<uint32_t> m_ImageHorizontalIter, m_ImageVerticalIter;
	glm::vec4* m_ImageAccumulateData = nullptr;
	uint32_t m_FrameIndex = 1;
	Settings m_setting;
	int bounces= 10;
};


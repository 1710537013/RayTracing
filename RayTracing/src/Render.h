#pragma once
#include<memory>
#include <Walnut/Image.h>
#include <imgui.h>
#include"glm/glm.hpp"
#include "Camera.h"
#include "Ray.h"
#include"Scene.h"
using namespace Walnut;

class Renderer
{
public:
	Renderer() = default;

	void Resize(uint32_t width, uint32_t height);
	void Render(const Scene& m_scene, const Camera& cam);
	glm::vec4 RayTrace(const Scene& m_scene, const Ray& coord);
	void SetLight(const glm::vec3& lightColor, const glm::vec3& lightPos);

	std::shared_ptr<Image> GetImage()const { return m_Image; }
private:
	glm::vec3 lightColor = { 1.0f,1.0f,1.0f };
	glm::vec3 lightPos = { 1.0f,1.0f,1.0f };
	std::shared_ptr<Image> m_Image;
	uint32_t* m_ImageData = nullptr;
};


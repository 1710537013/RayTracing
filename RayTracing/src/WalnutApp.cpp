#include<memory>
#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include"Render.h"

#include"glm/gtc/type_ptr.hpp"
#include"Camera.h"
#include"Scene.h"
#include "Walnut/Image.h"
#include <Walnut/Random.h>
#include <Walnut/Timer.h>
#include "BVH.h"
using namespace Walnut;
using namespace std;
using namespace glm;
typedef glm::vec3 color;
typedef glm::vec3 point3;

class ExampleLayer : public Walnut::Layer
{
private:

	std::vector<std::shared_ptr<Hittable>> random_scene() {
		std::vector<std::shared_ptr<Hittable>> world;

		auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

		world.push_back(make_shared<Sphere>(point3(0, -1000, 0), 1000, make_shared<Lambert>(checker)));

		for (int a = -11; a < 11; a++) {
			for (int b = -11; b < 11; b++) {
				auto choose_mat = Walnut::Random::Float();
				point3 center(a + 0.9 * Walnut::Random::Float(), 0.2, b + 0.9 * Walnut::Random::Float());

				if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
					shared_ptr<Material> sphere_material;

					if (choose_mat < 0.8) {
						// diffuse
						auto albedo =vec3( Walnut::Random::Float() * Walnut::Random::Float());
						sphere_material = make_shared<Lambert>(albedo);
						//auto center2 = center + vec3(0, Walnut::Random::random_float(0, .5), 0);
						world.push_back(make_shared<Sphere>(
							center, 0.2, sphere_material));
					}
					else if (choose_mat < 0.95) {
						// metal
						auto albedo = vec3(Walnut::Random::random_float(0.5, 1));
						auto fuzz = Walnut::Random::random_float(0, 0.5);
						sphere_material = make_shared<Metal>(albedo, fuzz);
						world.push_back(make_shared<Sphere>(center, 0.2, sphere_material));
					}
				}
			}
		}


		auto material2 = make_shared<Lambert>(color(0.4, 0.2, 0.1));
		world.push_back(make_shared<Sphere>(point3(-4, 1, 0), 1.0, material2));

		auto material3 = make_shared<Metal>(color(0.7, 0.6, 0.5), 0.0);
		world.push_back(make_shared<Sphere>(point3(4, 1, 0), 1.0, material3));

		auto bvh = make_shared<BVH>(world);
		return { bvh };
	}
	std::vector<std::shared_ptr<Hittable>> cornell_box() {
		std::vector<std::shared_ptr<Hittable>> list;

		auto red = make_shared<Lambert>(glm::vec3(0.65, 0.05, 0.05));
		auto white = make_shared<Lambert>(glm::vec3(0.73, 0.73, 0.73));
		auto green = make_shared<Lambert>(glm::vec3(0.12, 0.45, 0.15));
		auto light = make_shared<Light>(glm::vec3(10));


		list.push_back(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
		list.push_back(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
		list.push_back(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
		list.push_back(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
		list.push_back(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
		list.push_back(make_shared<xy_rect>(0, 555, 0, 555, 555, white));
		list.push_back(make_shared<box>(glm::vec3(130, 0, 65), glm::vec3(295, 165, 230), white));
		list.push_back(make_shared<box>(glm::vec3(265, 0, 295), glm::vec3(430, 330, 460), white));
		//objects.add(make_shared<box>(vec3(130, 0, 65), vec3(295, 165, 230), white));
		//objects.add(make_shared<box>(vec3(265, 0, 295), vec3(430, 330, 460), white));

		auto bvh = make_shared<BVH>(list);
		return { bvh };
		//return list;
	}
	std::vector<std::shared_ptr<Hittable>> three_sphere() {
		auto material_center = std::make_shared<Lambert>(glm::vec3(0.8, 0.8, 0.0));
		auto material_ground = std::make_shared<Lambert>(std::make_shared<checker_texture>
			(glm::vec3(0.2, 0.3, 0.1), glm::vec3(0.8, 0.8, 0.8)));
		auto material_left = std::make_shared<Metal>(glm::vec3(0.8, 0.8, 0.8), 0.3);
		auto material_right = std::make_shared<Metal>(glm::vec3(0.8, 0.6, 0.2), 1.0);

		auto perlin_mat = std::make_shared<Lambert>(std::make_shared<noise_texture>(10));
		auto imageMat = std::make_shared<Lambert>(std::make_shared<image_texture>("D:/MyFile/RayTracing/InOneWeekend/RayTracing/RayTracing/src/texture.jpg"));
		auto lightMat = std::make_shared<Light>(glm::vec3(1.0f, 1.0f, 1.0f));

		std::vector<std::shared_ptr<Hittable>> list;
		list.push_back(std::make_shared<Sphere>(glm::vec3(0.0, -100.5, -1.0), 100.0, material_ground));
		list.push_back(std::make_shared<Sphere>(glm::vec3(0.0, 0.0f, -1.0), 0.5, imageMat));
		list.push_back(std::make_shared<Sphere>(glm::vec3(-1.0, 0.0, -1.0), 0.5, material_left));
		list.push_back(std::make_shared<Sphere>(glm::vec3(1.0, 0.0, -1.0), 0.5, lightMat));
		auto bvh = make_shared<BVH>(list);
		return { bvh };
		/*return list;*/
	}
public:

	ExampleLayer() :cam(40.0f, 1.0f, 1000.0f) {

		m_scenes.scenes= cornell_box();
	}
	virtual void OnUpdate(float ts) {
		cam.OnUpdate(ts);
		if (!MotionBlur)
			m_render.ResetFrameIndex();
	}
	virtual void OnUIRender() override
	{
		ImGui::Begin("Setting");
		ImGui::Text("last render: %.3fms", lastTime);
		ImGui::Text("cam Pos:( %.1f, %.1f, %.1f)", cam.GetPosition().x, cam.GetPosition().y, cam.GetPosition().z);
		ImGui::ColorEdit3("LightColor: ", glm::value_ptr(lightColor));
		ImGui::DragFloat3("LightPos: ", glm::value_ptr(lightPos), .1f);
		m_render.SetLight(lightColor, lightPos);

		ImGui::Checkbox("Accumulate", &m_render.GetSettings().Accumulate);
		ImGui::Checkbox("MotionBlur", &MotionBlur);
		if (ImGui::Button("Render")) {
			Render();
		}
		if (ImGui::Button("ResetFrame")) {
			m_render.ResetFrameIndex();
		}
		ImGui::End();

		//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(.0f, .0f));
		ImGui::Begin("Begin");

		m_ViewportWidth = (uint32_t)ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = (uint32_t)ImGui::GetContentRegionAvail().y;
		auto image = m_render.GetImage();
		if (image)
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() }, ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();
		//		ImGui::PopStyleVar();

		Render();
	}
	void Render() {
		Timer time;
		cam.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_render.Resize(m_ViewportWidth, m_ViewportHeight);
		m_render.Render(m_scenes, cam);

		lastTime = time.ElapsedMillis();
	}
private:
	Scene m_scenes;
	Camera cam;
	Renderer m_render;
	float lastTime = .0;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	bool MotionBlur = false;
	glm::vec3 lightColor = { 0.6f, 0.7f, 0.9f };
	glm::vec3 lightPos = { 6.0f,6.0f,-1.0f };
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{
					app->Close();
				}
				ImGui::EndMenu();
			}
		});
	return app;
}
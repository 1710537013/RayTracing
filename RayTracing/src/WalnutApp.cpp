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
using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer():cam(45.0f,0.1f, 1000.0f){
		m_scenes.scenes.push_back({ {1,0,0},{1,0,1},.5f });
		m_scenes.scenes.push_back({ {0,0,0},{1,1,0},.5f });
	}
	virtual void OnUpdate(float ts) {
		cam.OnUpdate(ts);
	}
	virtual void OnUIRender() override
	{
		ImGui::Begin("Setting");
		ImGui::Text("last render: %.3fms", lastTime);
		ImGui::ColorEdit3("LightColor: ", glm::value_ptr(lightColor));
		ImGui::DragFloat3("LightPos: ",glm::value_ptr(lightPos),.1f);
		m_render.SetLight(lightColor, lightPos);
		/*if (ImGui::Button("Render")) {
			Render();
		}*/

		ImGui::End();

		//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(.0f, .0f));
		ImGui::Begin("Begin");

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;
		auto image = m_render.GetImage();
		if (image)
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() },ImVec2(0,1), ImVec2(1,0));

		ImGui::End();
//		ImGui::PopStyleVar();

		Render();
	}
	void Render() {
		Timer time;
		cam.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_render.Resize(m_ViewportWidth, m_ViewportHeight);
		m_render.Render(m_scenes,cam);

		lastTime = time.ElapsedMillis();
	}
private:
	Scene m_scenes;
	Camera cam;
	Renderer m_render;
	float lastTime = .0;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	glm::vec3 lightColor = {1.0f,1.0f,1.0f};
	glm::vec3 lightPos = { 6.0f,6.0f,-1.0f };
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Walnut Example";

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
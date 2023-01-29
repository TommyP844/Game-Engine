#include "ImGuiLayer.h"

#include "Mule/Mule.h"

#include "imgui.h"

void ImGuiLayer::OnAttach()
{
	Mule::RenderAPI::InitImGui();
}

void ImGuiLayer::OnUpdate()
{
	Mule::RenderAPI::BeginImGuiFrame();

	// draw stuff

	ImGui::ShowDemoWindow();


	Mule::RenderAPI::EndImGuiFrame();

}

void ImGuiLayer::OnDetach()
{
	Mule::RenderAPI::ShutdownImGui();
}

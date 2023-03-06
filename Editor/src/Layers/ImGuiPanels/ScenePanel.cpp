#include "ScenePanel.h"

#include "imgui.h"

ScenePanel::ScenePanel()
	:
	IPanel("Scene")
{
}

void ScenePanel::OnAttach()
{
}

void ScenePanel::OnUpdate()
{
	if (ImGui::Begin("Scene Panel"))
	{
		ImGui::Text("Scene");
	}
	ImGui::End();
}

void ScenePanel::OnDetach()
{
}

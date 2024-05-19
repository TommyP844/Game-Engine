#include "RenderSettingsPanel.h"

void RenderSettingsPanel::OnAttach()
{
}

void RenderSettingsPanel::OnUpdate(float dt)
{
	if (ImGui::Begin(Name().c_str()))
	{
		if (auto scene = mEditorState.GetActiveScene())
		{
			Mule::SceneRenderSettings& settings = mEditorState.GetActiveScene()->RenderSettings();

			//auto renderSystem = scene->GetRenderSystem();
			//
			//ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
			//if (ImGui::TreeNode("Stats"))
			//{
			//	ImGui::Text("CPU Frame Time: %.3fms", renderSystem->Stats.CPUTime);
			//	ImGui::Checkbox("Enable Frustum Culling", &renderSystem->Settings.EnableFrustumCulling);
			//	ImGui::Text("Frustum culled Objects: %i", renderSystem->Stats.FrustumCulledObjects);
			//	ImGui::Text("Draw Calls: %i", renderSystem->Stats.DrawCalls);
			//	
			//	ImGui::TreePop();
			//}
		}
	}
	ImGui::End();
}

void RenderSettingsPanel::OnDetach()
{
}

void RenderSettingsPanel::OnEvent(Mule::Ref<Mule::Event> event)
{
}

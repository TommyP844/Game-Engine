#include "SceneHierarchyPanel.h"

#include "imgui.h"

ScenePanel::ScenePanel(AppData& appData)
	:
	IPanel("Scene", appData)
{
}

void ScenePanel::OnAttach()
{
}

static void DisplayChildren(Mule::Entity parent, std::vector<Mule::Entity> children, int& ptr, AppData& appData)
{
	bool removeChild = false;
	for (auto& child : children)
	{
		int flags = ImGuiTreeNodeFlags_OpenOnDoubleClick;
		if (std::find(appData.SelectedEntities.begin(), appData.SelectedEntities.end(), child) != appData.SelectedEntities.end())
			flags |= ImGuiTreeNodeFlags_Selected;
		if (!child.HasChildren())
			flags |= ImGuiTreeNodeFlags_Bullet;
		bool open = ImGui::TreeNodeEx((void*)ptr++, flags, child.GetTag().c_str());
		if (ImGui::IsItemClicked())
		{
			appData.SelectedEntities.clear();
			appData.SelectedEntities.push_back(child);
		}
		if (ImGui::BeginPopupContextItem(0))
		{
			if (ImGui::MenuItem("Add Child"))
			{
				Mule::Entity entity = appData.ActiveScene->CreateEntity();
				child.AddChild(entity);
			}
			if (ImGui::MenuItem("Delete"))
			{
				removeChild = true;
			}
			
			ImGui::EndPopup();
		}
		if(open)
		{
			if (child.HasChildren())
			{
				DisplayChildren(child, child.GetChildren(), ptr, appData);
			}
			ImGui::TreePop();
		}

		if (removeChild)
		{
			appData.ActiveScene->RemoveEntity(child);
			if (std::find(appData.SelectedEntities.begin(), appData.SelectedEntities.end(), child) != appData.SelectedEntities.end())
			{
				appData.SelectedEntities.clear();
			}
			removeChild = false;
		}
		
	}
}

void ScenePanel::OnUpdate()
{
	if (ImGui::Begin("Scene Panel"))
	{
		if (!mAppData.ActiveScene)
		{
			ImGui::Text("No Scene");
			ImGui::End();
			return;
		}
		int ptr = 0;
		std::vector<Mule::Entity> parents = mAppData.ActiveScene->GetParentEntities();
		bool open = ImGui::TreeNode("Scene");
		if (ImGui::BeginPopupContextItem(0))
		{
			if (ImGui::MenuItem("Add Child"))
			{
				Mule::Entity entity = mAppData.ActiveScene->CreateEntity();
			}
			ImGui::EndPopup();
		}
		if(open)
		{
			DisplayChildren(Mule::Entity(), parents, ptr, mAppData);
			ImGui::TreePop();
		}
	}
	ImGui::End();
}

void ScenePanel::OnDetach()
{
}

#include "SceneHierarchyPanel.h"

static void DisplayEntityHierarchy(Mule::Entity entity, std::set<Mule::Entity>& selected)
{
	std::string label = entity.Tag() + "##" + std::to_string(entity.ID());

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	flags |= entity.Children().empty() ? ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet : 0;

	if(selected.find(entity) != selected.end())
		flags |= ImGuiTreeNodeFlags_Selected;

	bool open = ImGui::TreeNodeEx(label.c_str(), flags);

	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
	{
		selected.clear();
		selected.insert(entity);
	}

	if (ImGui::BeginPopupContextItem(label.c_str(), ImGuiPopupFlags_MouseButtonRight))
	{
		ImGui::Text(entity.Tag().c_str());
		ImGui::Separator();
		if (ImGui::MenuItem("Add Child"))
		{
			auto e = entity.GetScene()->CreateEntity();
			entity.AddChild(e);
		}
		if (ImGui::MenuItem("Delete"))
		{
			entity.Destroy();
			ImGui::EndPopup();
			if (open) ImGui::TreePop();
			return;
		}
		ImGui::EndPopup();
	}

	if (open)
	{
		for (auto& child : entity.Children())
		{
			DisplayEntityHierarchy(child, selected);
		}
		ImGui::TreePop();
	}
}

void SceneHierarchyPanel::OnAttach()
{
}

void SceneHierarchyPanel::OnUpdate(float dt)
{
}

void SceneHierarchyPanel::OnDetach()
{
}

void SceneHierarchyPanel::OnEvent(Mule::Ref<Mule::Event> event)
{
	switch (event->Type())
	{
	case Mule::EventType::WindowEvent:
	{
		Mule::WeakRef<Mule::WindowEvent> windowEvent = event;

	}
		break;
	}
}

void SceneHierarchyPanel::OnImGuiRender()
{
	int flags = 0;

	if (ImGui::Begin(Name().c_str(), nullptr, flags))
	{
		//ImGui::SetNextItemOpen(true, ImGuiCond_Always);
		//if (ImGui::TreeNode(mEditorState.GetActiveScene()->Name().c_str()))
		//{
		//	if (ImGui::BeginPopupContextWindow("Scene Popup"))
		//	{
		//		ImGui::Text(mEditorState.GetActiveScene()->Name().c_str());
		//		ImGui::Separator();
		//		if(ImGui::MenuItem("Add Entity"))
		//		{
		//			mEditorState.GetActiveScene()->CreateEntity();
		//		}
		//		ImGui::EndPopup();
		//	}
		//
		//	mEditorState.GetActiveScene()->IterateParentEntities([&](Mule::Entity e) {
		//		DisplayEntityHierarchy(e, mEditorState.SelectedEntities);
		//		});
		//	ImGui::TreePop();
		//}
	}
	ImGui::End();
}

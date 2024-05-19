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

	int id = entity.ID();
	DRAG_DROP_SOURCE(DRAG_DROP_ENTITY, &id, sizeof(int));
	DRAG_DROP_TARGET(DRAG_DROP_ENTITY, [&](const ImGuiPayload* payload) {
		int id = *(int*)payload->Data;
		Mule::Entity e((entt::entity)id, entity.GetScene());
		e.Orphan();
		entity.AddChild(e);
		});

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
	int flags = 0;
	if (mEditorState.GetActiveScene() && mEditorState.GetActiveScene()->Modified())
	{
		flags |= ImGuiWindowFlags_UnsavedDocument;
	}

	if (ImGui::Begin(Name().c_str(), nullptr, flags) && mEditorState.GetActiveScene())
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Always);
		if (ImGui::TreeNode(mEditorState.GetActiveScene()->Name().c_str()))
		{
			if (ImGui::BeginPopupContextWindow("Scene Popup"))
			{
				ImGui::Text(mEditorState.GetActiveScene()->Name().c_str());
				ImGui::Separator();
				if(ImGui::MenuItem("Add Entity"))
				{
					mEditorState.GetActiveScene()->CreateEntity();
				}
				ImGui::EndPopup();
			}

			DRAG_DROP_TARGET(DRAG_DROP_ENTITY, [&](const ImGuiPayload* payload) {
				int id = *(int*)payload->Data;
				Mule::Entity e((entt::entity)id, mEditorState.GetActiveScene());
				e.Orphan();
				});
			mEditorState.GetActiveScene()->IterateParentEntities([&](Mule::Entity e) {
				DisplayEntityHierarchy(e, mEditorState.SelectedEntities);
				});
			ImGui::TreePop();
		}
	}
	ImGui::End();
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

		if (windowEvent->KeyPressed[(int)Mule::Key::Key_S]
			&& (windowEvent->KeyPressed[(int)Mule::Key::Key_LeftControl]
				|| windowEvent->KeyPressed[(int)Mule::Key::Key_RightControl]))
		{
			if (mEditorState.GetActiveScene() && mEditorState.GetActiveScene()->Modified())
			{
				mEditorState.GetActiveScene()->Serialize(Mule::SerializationMode::Text);
			}
		}
	}
		break;
	}
}

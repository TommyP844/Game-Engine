#include "SceneHierarchyPanel.h"

#include "ImGuiUtil.h"

static void DisplayEntityHierarchy(Mule::Entity entity, std::set<Mule::Entity>& selected)
{
	std::string label = entity.Tag() + "##" + std::to_string(entity.ID());

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	flags |= entity.Children().empty() ? ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet : 0;

	if(selected.find(entity) != selected.end())
		flags |= ImGuiTreeNodeFlags_Selected;

	bool open = ImGui::TreeNodeEx(label.c_str(), flags);
	DragDropModelToEntity(entity);

	int id = entity.ID();
	DragDropSource(ENTITY_ID_PAYLOAD, &id, sizeof(uint32_t));
	DragDropTarget(ENTITY_ID_PAYLOAD, [&](const ImGuiPayload* payload) {
		uint32_t ddid = TypeFromPayload<uint32_t>(payload);
		if (ddid == id)
			return;
		entity.AddChild(Mule::Entity((entt::entity)ddid, entity.GetScene()));
		});

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
			selected.clear();
			selected.insert(e);
			entity.AddChild(e);
		}
		if (ImGui::MenuItem("Delete"))
		{
			selected.erase(entity);
			entity.Destroy();
			ImGui::EndPopup();
			if (open) ImGui::TreePop();
			return;
		}
		if (ImGui::MenuItem("Orphan"))
		{
			entity.Orphan();
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
	Mule::Ref<Mule::Scene> scene = mEditorState->GetActiveScene();

	int flags = 0;
	if (scene)
	{
		flags |= scene->Modified() ? ImGuiWindowFlags_UnsavedDocument : 0;
	}

	if (mIsOpen)
	{
		if (ImGui::Begin(Name().c_str(), &mIsOpen, flags))
		{
			if (scene == nullptr)
			{
				ImGui::Text("No scene loaded");
			}
			else
			{
				if (ImGui::BeginPopupContextWindow("Scene Popup"))
				{
					if (ImGui::MenuItem("Add Entity"))
					{
						auto e = scene->CreateEntity();
						mEditorState->SelectedEntities.clear();
						mEditorState->SelectedEntities.insert(e);
					}
					ImGui::EndPopup();
				}

				ImGui::Text("Scene: %s", scene->Name().c_str());
				scene->IterateParentEntities([&](Mule::Entity e) {
					DisplayEntityHierarchy(e, mEditorState->SelectedEntities);
					});
			}
		}
		ImGui::End();
	}
}

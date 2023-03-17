#include "SceneViewPanel.h"

#include "../../ImGuiGlobals.h"

SceneViewPanel::SceneViewPanel(AppData& data)
	:
	IPanel("Scene View", data)
{
}

void SceneViewPanel::OnAttach()
{
}

void SceneViewPanel::OnUpdate()
{
	Mule::AssetManager& manager = Mule::AssetManager::Get();
	Mule::Ref<Mule::Scene> scene = mAppData.ActiveScene;
	std::string windowName = scene ? scene->GetName() : "Scene";
	static ImVec2 oldSize = ImVec2(1, 1);

	int flags = 0;
	if (scene && scene->IsModified())
		flags |= ImGuiWindowFlags_UnsavedDocument;

	if (ImGui::Begin(windowName.c_str(), 0, flags))
	{
		ImVec2 size = ImGui::GetContentRegionAvail();
		ImTextureID id = 0;
		ImGui::Text("Scene");
		if (scene)
		{
			id = scene->OnEditorRender(size.x, size.y);

			if (size.x != oldSize.x || size.y != oldSize.y)
			{
				scene->ResizeFramebuffer(size.x, size.y);
				LOG_MSG(L"Framebuffer resized for scene: {0}", scene->GetName());
			}
			oldSize = size;

			if (scene->IsModified())
			{
				// problably need to read from imgui IO, these functions have to be set by the user
				if (
					(ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
						&& ImGui::IsKeyDown(ImGuiKey_C))
				{
					Mule::Scene::Serialize(scene->GetPath(), scene);
				}
			}

		}
		ImGui::Image(id, size);
		// begin drag drop for mesh and materials
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(PAYLOAD_SCENE))
			{
				std::string sceneName = std::string((char*)payload->Data, payload->DataSize);
				mAppData.ActiveScene = manager.GetAsset<Mule::Scene>(sceneName);
				auto e = mAppData.ActiveScene->CreateEntity();
				mAppData.SelectedEntities.push_back(e);
				Mule::AssetManager& manager = Mule::AssetManager::Get();
				auto& m = e.AddComponent<Mule::MeshCollectionComponent>();
				m.Meshes = manager.GetAsset<Mule::MeshCollection>("spider");

			}
			ImGui::EndDragDropTarget();
		}
	}
	ImGui::End();
}

void SceneViewPanel::OnDetach()
{
}

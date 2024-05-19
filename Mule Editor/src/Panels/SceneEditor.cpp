#include "SceneEditor.h"

#include "Mule.h"

#include "imgui.h"
#include "ImGuizmo.h"
#include "imgui_internal.h"

void SceneEditor::OnAttach()
{
	char data[] = { 0x00, 0x00, 0x00, 0xFF };
	mNoSceneTexture = Mule::Texture::Create2D("No Scene Texture", data, 1, 1, Mule::TextureFormat::RGBA8);
	mEditorCamera = Mule::Camera({ 0.f, 1.f, 0.f }, 1.f, 1.f, 1000.f, 90.f, 0.f, 0.f);
	mCameraSpeed = 8.f;
}

void SceneEditor::OnUpdate(float dt)
{
	static ImVec2 windowSize = { 0, 0 };
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
	if (ImGui::Begin(Name().c_str()))
	{
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		Mule::AssetManager& manager = Mule::AssetManager::Get();
		auto size = ImGui::GetContentRegionAvail();
		ImTextureID texId = mNoSceneTexture->ImGuiID();

		if (mEditorState.GetActiveScene())
		{
			Mule::Ref<Mule::Scene> scene = mEditorState.GetActiveScene();
			scene->OnUpdate(dt);

			if (size.x != windowSize.x || size.y != windowSize.y)
			{
				windowSize = size;
				scene->Resize(windowSize.x, windowSize.y);
				scene->IterateEntities([](Mule::Entity e) {
					if (e.HasComponent<Mule::CameraComponent>()) {
						Mule::CameraComponent& camera = e.GetComponent<Mule::CameraComponent>();
						camera.Camera.SetAspectRatio(windowSize.x / windowSize.y);
					}
					});
			}

			mEditorCamera.UpdateMatrices(mEditorCameraTransform.TR());
			scene->OnEditorRender(dt, mEditorState.SelectedEntities, mEditorCamera);
			texId = scene->OutputImage();

			bool openEntityPopup = false;
			bool leftClick = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
			bool rightClick = ImGui::IsMouseClicked(ImGuiMouseButton_Right);
			if ((leftClick || rightClick) && ImGui::IsWindowHovered() && !ImGuizmo::IsUsing())
			{
				//auto attachment = scene->GetRenderSystem()->GetFramebuffer()->GetAttachment(1);
				auto [x, y] = ImGui::GetMousePos();
				x -= cursorPos.x;
				y -= cursorPos.y;
				y = size.y - y;
				int data[1];
				//attachment->ReadPixels(data, x, y, 1, 1);
				if (data[0] != INT32_MAX)
				{
					if (scene->IsEntityValid((entt::entity)data[0]))
					{
						mEditorState.SelectedEntities.clear();
						mEditorState.SelectedEntities.insert(Mule::Entity((entt::entity)data[0], scene));
						if (rightClick)
						{
							openEntityPopup = true;
						}
					}
				}
			}

			if (openEntityPopup)
			{
				ImGui::OpenPopup("Scene Entity Popup");
			}

			if (ImGui::BeginPopupContextItem("Scene Entity Popup"))
			{
				Mule::Entity e = *(mEditorState.SelectedEntities.begin());
				ImGui::Text("%s", e.Tag().c_str());
				ImGui::Separator();
				if (ImGui::MenuItem("Delete"))
				{
					mEditorState.SelectedEntities.clear();
					scene->RemoveEntity(e);
				}
				ImGui::EndPopup();
			}
		}
			
		ImGui::Image(texId, size, {0, 1}, {1, 0});
		DRAG_DROP_TARGET(DRAG_DROP_SCENE_FILE, [&](const ImGuiPayload* payload) {
			std::string path = PAYLOAD_TO_STR(payload);
			if (auto scene = manager.GetAssetByFilepath<Mule::Scene>(path))
			{
				mEditorState.SetActiveSceneHandle(scene->Handle());
			}
			else
			{
				auto LoadedScene = Mule::Scene::Deserialize(path, Mule::SerializationMode::Text);
				manager.InsertAsset(LoadedScene);
				mEditorState.SetActiveSceneHandle(LoadedScene->Handle());
			}
			});


		// Handle camera
		if (ImGui::IsWindowHovered() && ImGui::IsWindowFocused())
		{
			auto& io = ImGui::GetIO();

			const glm::vec3& forward = mEditorCamera.Forward();
			const glm::vec3& right = mEditorCamera.Right();
			const glm::vec3 localUp = glm::normalize(glm::cross(forward, right));

			glm::vec3 translation = glm::vec3(0.f);
			if (io.KeysDown[ImGuiKey_W]) 
			{
				translation += forward * mCameraSpeed * dt;
			}
			if (io.KeysDown[ImGuiKey_S])
			{
				translation += -forward * mCameraSpeed * dt;
			}
			if (io.KeysDown[ImGuiKey_A])
			{
				translation += -right * mCameraSpeed * dt;
			}
			if (io.KeysDown[ImGuiKey_D])
			{
				translation += right * mCameraSpeed * dt;
			}
			if (io.KeysDown[ImGuiKey_R])
			{
				translation += glm::vec3(0, 1, 0) * mCameraSpeed * dt;
			}
			if (io.KeysDown[ImGuiKey_F])
			{
				translation += glm::vec3(0, -1, 0) * mCameraSpeed * dt;
			}
			mEditorCameraTransform.Translation += translation;

			if (io.MouseDown[ImGuiMouseButton_Middle])
			{
				glm::vec2 mouseDelta = { io.MouseDelta.x, io.MouseDelta.y };
				mEditorCamera.SetYaw(mEditorCamera.YawDegrees() + mouseDelta.x);
				mEditorCamera.SetPitch(mEditorCamera.PitchDegrees() + mouseDelta.y);
			}
		}

		// Handle Gizmos
		if (mEditorState.SelectedEntities.size() == 1)
		{
			if (ImGui::IsKeyPressed(ImGuiKey_T, false)) mShowTranslationGizmo = !mShowTranslationGizmo;
			if (ImGui::IsKeyPressed(ImGuiKey_R, false)) mShowRotationGizmo = !mShowRotationGizmo;
			if (ImGui::IsKeyPressed(ImGuiKey_S, false)) mShowScaleGizmo = !mShowScaleGizmo;

			int operation = 0;
			operation |= (mShowTranslationGizmo ? ImGuizmo::OPERATION::TRANSLATE : 0);
			operation |= (mShowRotationGizmo ? ImGuizmo::OPERATION::ROTATE : 0);
			operation |= (mShowScaleGizmo ? ImGuizmo::OPERATION::SCALE : 0);

			Mule::Entity entity = *(mEditorState.SelectedEntities.begin());
			Mule::Transform& transform = entity.Transform();
			glm::mat4 trs = transform.TRS();

			glm::mat4 projection = mEditorCamera.ProjectionMatrix();
			glm::mat4 view = mEditorCamera.ViewMatrix();

			if (operation != 0)
			{
				ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
				ImGuizmo::SetRect(cursorPos.x, cursorPos.y, size.x, size.y);
				if (ImGuizmo::Manipulate(
					&view[0][0],
					&projection[0][0],
					(ImGuizmo::OPERATION)operation,
					ImGuizmo::MODE::WORLD,
					&trs[0][0]
				))
				{
					ImGuizmo::DecomposeMatrixToComponents(&trs[0][0], &transform.Translation[0], &transform.Rotation[0], &transform.Scale[0]);
				}
			}
		}
	}
	ImGui::End();
	ImGui::PopStyleVar();

}

void SceneEditor::OnDetach()
{
}

void SceneEditor::OnEvent(Mule::Ref<Mule::Event> event)
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

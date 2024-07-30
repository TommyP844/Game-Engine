#include "SceneView.h"
#include "ImGuiUtil.h"

void SceneView::OnAttach()
{
	mRenderer = Mule::SceneRenderer::Create(
		mEditorState->Device,
		mEditorState->Device->GetMainThreadContext()
	);
}

void SceneView::OnUpdate(float dt)
{
	auto scene = mEditorState->GetActiveScene();
	if (scene)
	{
		if (mResizeRequired)
		{
			mResizeRequired = false;
			mRenderer->Resize(mWidth, mHeight);
		}
		mRenderer->Render(scene);
	}
}

void SceneView::OnDetach()
{
}

void SceneView::OnEvent(Mule::Ref<Mule::Event> event)
{
}

void SceneView::OnImGuiRender()
{
	if (mIsOpen)
	{
		int flags = 0;

		auto scene = mEditorState->GetActiveScene();
		if (scene)
		{
			flags |= scene->Modified() ? ImGuiWindowFlags_UnsavedDocument : 0;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
		if (ImGui::Begin(Name().c_str(), &mIsOpen))
		{
			auto [width, height] = ImGui::GetWindowSize();
			if (width != mWidth || height != mHeight)
			{
				mWidth = width;
				mHeight = height;
				mResizeRequired = true;
			}
			ImVec2 fbSize = ImGui::GetContentRegionAvail();
			ImGui::Image(mRenderer->GetFBImage(0), fbSize);
			DragDropTarget(SCENE_PATH_PAYLOAD, [&](const ImGuiPayload* payload) {
				fs::path path = PathFromPayload(payload);
				Mule::AssetManager& manager = Mule::AssetManager::Get();
				if (Mule::Ref<Mule::Scene> scene = manager.GetAssetByFilepath<Mule::Scene>(path))
				{
					mEditorState->ActiveSceneHandle = scene->Handle();
				}
				else
				{
					Mule::Ref<Mule::Scene> loadedScene = Mule::Scene::Deserialize(path, Mule::SerializationMode::Text);
					manager.InsertAsset(loadedScene);
					mEditorState->ActiveSceneHandle = loadedScene->Handle();
				}
				});
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

	if (!mIsOpen)
	{
		MULE_LOG_VERBOSE("Closing Panel: {0}", Name());
	}
}

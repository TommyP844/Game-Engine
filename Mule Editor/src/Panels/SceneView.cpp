#include "SceneView.h"

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
			ImGui::Image(mRenderer->GetFBImage(0), ImGui::GetContentRegionAvail());
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

	if (!mIsOpen)
	{
		MULE_LOG_VERBOSE("Closing Panel: {0}", Name());
	}
}

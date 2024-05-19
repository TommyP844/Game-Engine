#include "TextEditorPanel.h"

#include <fstream>

void TextEditorPanel::OnAttach()
{
	std::fstream file(mFilePath, std::ios::in | std::ios::ate);
	if (!file.is_open())
	{
		MULE_LOG_ERROR("Failed to open file: {0}", mFilePath);
	}

	size_t size = file.tellg();
	file.seekg(0);
	file.read(mBuffer, size);
	file.close();

	mTextEditor.SetText(mBuffer);
	mTextEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
}

void TextEditorPanel::OnUpdate(float dt)
{
	int flags = mModified ? ImGuiWindowFlags_UnsavedDocument : 0;
	if (ImGui::Begin(Name().c_str(), nullptr, flags))
	{
		mFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);
		auto size = ImGui::GetContentRegionAvail();
		mTextEditor.Render("##fileEdit", size);
		mFocused |= ImGui::IsItemFocused();
		mModified |= mTextEditor.IsTextChanged();
	}
	ImGui::End();
}

void TextEditorPanel::OnDetach()
{
}

void TextEditorPanel::OnEvent(Mule::Ref<Mule::Event> event)
{
	switch (event->Type())
	{
	case Mule::EventType::WindowEvent:
	{
		Mule::WeakRef<Mule::WindowEvent> winEvent = event;
		if (mFocused && mModified && winEvent->KeyPressed[(int)Mule::Key::Key_S]
			&& (winEvent->KeyPressed[(int)Mule::Key::Key_LeftControl]
				|| winEvent->KeyPressed[(int)Mule::Key::Key_RightControl]))
		{
			event->SetHandled();
			mModified = false;
			std::ofstream file(mFilePath);
			if (!file.is_open())
			{
				MULE_LOG_ERROR("Failed to open file: {0}", mFilePath);
				break;
			}
			std::string data = mTextEditor.GetText();
			file << data;
			file.close();
		}
	}
		break;
	}
}

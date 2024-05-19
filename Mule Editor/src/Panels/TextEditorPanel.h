#pragma once

#include "Panel.h"
#include "TextEditor.h"

class TextEditorPanel : public Panel
{
public:
	TextEditorPanel(const fs::path& file, EditorLayer* layer)
		:
		mFilePath(file),
		Panel(file.filename().string(), layer)
	{}

	virtual void OnAttach() override;
	virtual void OnUpdate(float dt) override;
	virtual void OnDetach() override;
	virtual void OnEvent(Mule::Ref<Mule::Event> event) override;

private:
	fs::path mFilePath;
	char mBuffer[4096] = { 0 };
	bool mModified = false;
	bool mFocused = false;
	TextEditor mTextEditor;
};

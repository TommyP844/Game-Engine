#pragma once

#include "Mule.h"
#include "EditorState.h"

class EditorLayer;

class Panel
{
public:
	Panel(const std::string& name, Mule::WeakRef<EditorState> editorState)
		:
		mName(name),
		mIsOpen(true),
		mEditorState(editorState)
	{}
	~Panel(){}

	virtual void OnAttach() = 0;
	virtual void OnUpdate(float dt) = 0;
	virtual void OnDetach() = 0;
	virtual void OnImGuiRender() = 0;
	virtual void OnEvent(Mule::Ref<Mule::Event> event) = 0;

	const std::string& Name() const { return mName; }
	bool IsOpen() const { return mIsOpen; }
	void SetOpen(bool open) { mIsOpen = true; }
	void ToggleOpen() { mIsOpen = !mIsOpen; }

protected:
	bool mIsOpen;
	Mule::WeakRef<EditorState> mEditorState;
private:
	std::string mName;
};


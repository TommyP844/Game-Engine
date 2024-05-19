#pragma once

#include "Mule.h"
#include "EditorState.h"

class EditorLayer;

class Panel
{
public:
	Panel(const std::string& name, EditorLayer* layer)
		:
		mName(name),
		mEditorState(GetEditorState()),
		mLayer(layer),
		mIsOpen(true)
	{}
	~Panel(){}

	virtual void OnAttach() = 0;
	virtual void OnUpdate(float dt) = 0;
	virtual void OnDetach() = 0;
	virtual void OnEvent(Mule::Ref<Mule::Event> event) = 0;

	const std::string& Name() const { return mName; }

protected:
	EditorState& mEditorState;
	EditorLayer* mLayer;
	bool mIsOpen;
private:
	std::string mName;
};


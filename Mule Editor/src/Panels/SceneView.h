#pragma once

#include "Panel.h"

#include "Mule.h"

class SceneView : public Panel
{
public:
	SceneView(Mule::Ref<EditorState> editorState)
		:
		Panel("Scene View", editorState)
	{}

	virtual void OnAttach() override;
	virtual void OnUpdate(float dt) override;
	virtual void OnDetach() override;
	virtual void OnEvent(Mule::Ref<Mule::Event> event) override;
	virtual void OnImGuiRender() override;

private:
	Mule::Ref<Mule::SceneRenderer> mRenderer;
	float mWidth, mHeight;
	bool mResizeRequired = false;
};


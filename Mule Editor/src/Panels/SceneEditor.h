#pragma once

#include "Panel.h"
#include "Mule.h"

class SceneEditor : public Panel
{
public:
	SceneEditor(EditorLayer* layer)
		:
		Panel("Scene View", layer)
	{}

	virtual void OnAttach() override;
	virtual void OnUpdate(float dt) override;
	virtual void OnDetach() override;
	virtual void OnEvent(Mule::Ref<Mule::Event> event) override;

private:
	Mule::Ref<Mule::Texture> mNoSceneTexture;
	Mule::Camera mEditorCamera;
	Mule::Transform mEditorCameraTransform;
	float mCameraSpeed;

	bool mShowScaleGizmo = false;
	bool mShowRotationGizmo = false;
	bool mShowTranslationGizmo = false;

};

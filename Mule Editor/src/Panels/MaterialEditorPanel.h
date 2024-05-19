#pragma once

#include "Panel.h"

#include "Mule.h"

class MaterialEditorPanel : public Panel
{
public:
	MaterialEditorPanel(EditorLayer* editor, Mule::Ref<Mule::Material> material = nullptr)
		:
		Panel("Material Editor", editor),
		mMaterial(material)
	{}

	virtual void OnAttach() override;
	virtual void OnUpdate(float dt) override;
	virtual void OnDetach() override;
	virtual void OnEvent(Mule::Ref<Mule::Event> event) override;

private:
	Mule::Ref<Mule::Material> mMaterial;
	bool mModified = false;

	void DisplayTexture(const char* name, Mule::AssetHandle& handle);
};
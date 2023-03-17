#pragma once

#include "IPanel.h"

#include "Mule/Mule.h"

class MaterialPanel : public IPanel
{
public:
	MaterialPanel(AppData& appData);

	// This constructor should be used when we want to open a material with the panel
	MaterialPanel(AppData& appData, Mule::Ref<Mule::Material> openMaterial);

	// This is for creating a new material
	MaterialPanel(AppData& appData, const fs::path& materialName);
	~MaterialPanel();

	virtual void OnAttach() override;
	virtual void OnUpdate() override;
	virtual void OnDetach() override;

private:
	Mule::Ref<Mule::Material> mEditMaterial;
	int mWindowID;
	static int sWindowID;
	char mBuffer[64] = { 0 };

	enum MaterialTextureSlot
	{
		MaterialTextureSlot_Albedo,
		MaterialTextureSlot_Normal,
		MaterialTextureSlot_Roughness,
		MaterialTextureSlot_Metalness,
		MaterialTextureSlot_AO
	};

	bool DisplayMaterialTexture(std::string& texture, Mule::Ref<Mule::Texture> tex, const char* label, MaterialTextureSlot slot);
};
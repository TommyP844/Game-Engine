#pragma once

#include "IPanel.h"

#include "Mule/Mule.h"


class AssetManagerPanel : public IPanel
{
public:
	AssetManagerPanel(AppData& appData, const std::string& baseDir);
	~AssetManagerPanel();

	virtual void OnAttach() override;
	virtual void OnUpdate() override;
	virtual void OnDetach() override;

private:
	std::string mBaseDir;

	void ListAssets(Mule::AssetType type);
};

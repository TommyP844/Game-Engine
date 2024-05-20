#pragma once

#include "Mule.h"
#include "Panels/Panel.h"
#include "ILayer.h"

#include <map>
#include <set>

enum class PanelType : int
{
	AssetBrowserPanel,
	ComponentPanel,
	ContentBrowserPanel,
	SceneEditorPanel,
	SceneHierarchyPanel,
	TextEditorPanel,
	MaterialEditorPanel,
	RenderSettingsPanel
};

class EditorLayer : public ILayer
{
public:
	EditorLayer(const fs::path& projectPath)
		:
		mProjectPath(projectPath),
		ILayer(nullptr, "Editor Layer")
	{}

	virtual void OnAttach() override;
	virtual void OnUpdate(float dt) override;
	virtual void OnDetach() override;
	virtual void OnEvent(Mule::Ref<Mule::Event> event) override;
	virtual void OnImGuiRender(float dt = 0.f) override;

	template<typename T, typename ...Args>
	void PushPanel(PanelType type, Args&&... args)
	{
		mPanels[type] = Mule::Ref<T>(new T(std::forward<Args>(args)...));
		mPanels[type]->OnAttach();
	}

	void PopPanel(PanelType type);
	void PopPanelAfterUpdate(PanelType type);

private:
	std::map<PanelType, Mule::Ref<Panel>> mPanels;
	std::map<PanelType, bool> mOpenPanels;
	std::set<PanelType> mPanelsToPop;

	void DisplayMenuBar();
	void DisplayPopups();

	char mInputBuffer[256] = { 0 };

	fs::path mProjectPath;

};
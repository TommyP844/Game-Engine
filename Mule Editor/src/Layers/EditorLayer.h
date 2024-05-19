#pragma once

#include "Mule.h"
#include "Panels/Panel.h"
#include "ProjectSettings.h"

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

class EditorLayer : public Mule::Layer
{
public:
	EditorLayer(const fs::path& projectPath)
		:
		mProjectPath(projectPath),
		Layer("Editor Layer")
	{}

	virtual void OnAttach() override;
	virtual void OnUpdate(float dt) override;
	virtual void OnDestroy() override;
	virtual void OnEvent(Mule::Ref<Mule::Event> event) override;

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

	void DisplayMenuBar();
	void DisplayPopups();

	char mInputBuffer[256] = { 0 };

	ProjectSettings mProjectSettings;
	fs::path mProjectPath;
	std::set<PanelType> mPanelsToPop;

};
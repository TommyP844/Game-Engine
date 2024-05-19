#include "EditorLayer.h"
#include "ThumbnailManager.h"

// panels
#include "Panels/ComponentPanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowser.h"
#include "Panels/AssetBrowser.h"
#include "Panels/SceneEditor.h"
#include "Panels/MaterialEditorPanel.h"
#include "Panels/RenderSettingsPanel.h"

#include "Util.h"

#define PUSH_POP_PANEL(name, type, panel, ...) \
	if (ImGui::MenuItem(name, "", &mOpenPanels[panel])) \
	{ \
		if (mOpenPanels[panel]) \
			PushPanel<type>(panel, this, __VA_ARGS__); \
		else \
			PopPanel(panel); \
	}\

#define PUSH_POP_PANEL(name, type, panel) \
	if (ImGui::MenuItem(name, "", &mOpenPanels[panel])) \
	{ \
		if (mOpenPanels[panel]) \
			PushPanel<type>(panel, this); \
		else \
			PopPanel(panel); \
	}

void EditorLayer::OnAttach()
{
	ThumbnailManager::Initialize();
	ThumbnailManager::Get().Init();

	mProjectSettings.Load(mProjectPath);

	if (fs::exists(mProjectSettings.Path()))
	{
		EditorState& state = GetEditorState();
		state.ProjectPath = mProjectPath;
		
	}

	mOpenPanels = {
		{ PanelType::AssetBrowserPanel, false },
		{ PanelType::ComponentPanel, true },
		{ PanelType::ContentBrowserPanel, true },
		{ PanelType::SceneEditorPanel, true },
		{ PanelType::SceneHierarchyPanel, true },
		{ PanelType::RenderSettingsPanel, true }
	};

	if (mOpenPanels[PanelType::AssetBrowserPanel]) PushPanel<AssetBrowser>(PanelType::AssetBrowserPanel, this);
	if (mOpenPanels[PanelType::ComponentPanel]) PushPanel<ComponentPanel>(PanelType::ComponentPanel, this);
	if (mOpenPanels[PanelType::ContentBrowserPanel]) PushPanel<ContentBrowser>(PanelType::ContentBrowserPanel, this);
	if (mOpenPanels[PanelType::SceneEditorPanel]) PushPanel<SceneEditor>(PanelType::SceneEditorPanel, this);
	if (mOpenPanels[PanelType::SceneHierarchyPanel]) PushPanel<SceneHierarchyPanel>(PanelType::SceneHierarchyPanel, this);
	if (mOpenPanels[PanelType::RenderSettingsPanel]) PushPanel<RenderSettingsPanel>(PanelType::RenderSettingsPanel, this);
}

void EditorLayer::OnUpdate(float dt)
{
	EditorState& state = GetEditorState();

	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockspace_flags);
	ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size, ImGuiCond_Always);
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;

	DisplayMenuBar();

	for (auto& [type, panel] : mPanels)
	{
		panel->OnUpdate(dt);
	}

	for (auto panelType : mPanelsToPop)
	{
		PopPanel(panelType);
		mOpenPanels[panelType] = false;
	}
	mPanelsToPop.clear();
}

void EditorLayer::OnDestroy()
{
	mProjectSettings.Save();
	ThumbnailManager::Shutdown();
}

void EditorLayer::OnEvent(Mule::Ref<Mule::Event> event)
{
	for (auto& [type, panel] : mPanels)
	{
		panel->OnEvent(event);
	}
}

void EditorLayer::PopPanel(PanelType type)
{
	mPanels[type]->OnDetach();
	mPanels.erase(type);
}

void EditorLayer::PopPanelAfterUpdate(PanelType type)
{
	mPanelsToPop.insert(type);
}

void EditorLayer::DisplayMenuBar()
{
	EditorState& state = GetEditorState();

	bool openNewSceneModal = false;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Project...", "Crtl + Shift + N")) {}
			if (ImGui::MenuItem("Open Project", "Crtl + Shift + O")) {}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Scene")) { openNewSceneModal = true; memset(mInputBuffer, 0, 256); }
			if (ImGui::MenuItem("Entity", "", nullptr, state.GetActiveScene())) { state.GetActiveScene()->CreateEntity(); }
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Panels"))
		{
			PUSH_POP_PANEL("Asset Browser", AssetBrowser, PanelType::AssetBrowserPanel)
			PUSH_POP_PANEL("Component Panel", ComponentPanel, PanelType::ComponentPanel)
			PUSH_POP_PANEL("Content Browser", ContentBrowser, PanelType::ContentBrowserPanel)
			PUSH_POP_PANEL("Scene Editor", SceneEditor, PanelType::SceneEditorPanel)
			PUSH_POP_PANEL("Scene Hierarchy", SceneHierarchyPanel, PanelType::SceneHierarchyPanel)
			PUSH_POP_PANEL("Material Editor", MaterialEditorPanel, PanelType::MaterialEditorPanel)
			PUSH_POP_PANEL("Render Settings", RenderSettingsPanel, PanelType::RenderSettingsPanel)
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (openNewSceneModal) ImGui::OpenPopup("New Scene Modal");

	DisplayPopups();
}

void EditorLayer::DisplayPopups()
{
	EditorState& state = GetEditorState();
	Mule::AssetManager& manager = Mule::AssetManager::Get();

	if (ImGui::BeginPopupModal("New Scene Modal"))
	{
		ImGui::InputText("Name", mInputBuffer, 256);
		// TODO: Add filesystem explorer
		ImGui::Text("Path: %s", (state.ProjectPath / "Assets" / "Scenes").string().c_str());
		if (ImGui::Button("Create"))
		{
			// validate: name, directory, etc

			// create
			std::string name = mInputBuffer;
			fs::path filepath = state.ProjectPath / "Assets" / "Scenes" / name;
			if (!name.ends_with(".scene"))
				filepath += ".scene";

			auto scene = Mule::Scene::Create(filepath);
			manager.InsertAsset(scene);
			scene->Serialize(Mule::SerializationMode::Text); // So the file appears in the editor

			// make scene current
			state.SetActiveSceneHandle(scene->Handle());

			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

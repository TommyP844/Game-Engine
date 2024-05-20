#include "EditorLayer.h"

// panels
#include "Panels/ComponentPanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowser.h"

#include "Util.h"

void EditorLayer::OnAttach()
{
	mOpenPanels = {
		{ PanelType::AssetBrowserPanel, false },
		{ PanelType::ComponentPanel, true },
		{ PanelType::ContentBrowserPanel, true },
		{ PanelType::SceneEditorPanel, true },
		{ PanelType::SceneHierarchyPanel, true },
		{ PanelType::RenderSettingsPanel, true }
	};

	if (mOpenPanels[PanelType::ComponentPanel]) PushPanel<ComponentPanel>(PanelType::ComponentPanel, mApplicationData);
	if (mOpenPanels[PanelType::ContentBrowserPanel]) PushPanel<ContentBrowser>(PanelType::ContentBrowserPanel, mApplicationData);
	if (mOpenPanels[PanelType::SceneHierarchyPanel]) PushPanel<SceneHierarchyPanel>(PanelType::SceneHierarchyPanel, mApplicationData);
}

void EditorLayer::OnUpdate(float dt)
{
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnEvent(Mule::Ref<Mule::Event> event)
{
	for (auto& [type, panel] : mPanels)
	{
		panel->OnEvent(event);
	}
}

void EditorLayer::OnImGuiRender(float dt)
{
	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockspace_flags);
	ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size, ImGuiCond_Always);
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;

	DisplayMenuBar();

	for (auto& [type, panel] : mPanels)
	{
		panel->OnImGuiRender();
	}

	for (auto panelType : mPanelsToPop)
	{
		PopPanel(panelType);
		mOpenPanels[panelType] = false;
	}
	mPanelsToPop.clear();
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
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Panels"))
		{
			//PUSH_POP_PANEL("Component Panel", ComponentPanel, PanelType::ComponentPanel)
			//PUSH_POP_PANEL("Content Browser", ContentBrowser, PanelType::ContentBrowserPanel)
			//PUSH_POP_PANEL("Scene Hierarchy", SceneHierarchyPanel, PanelType::SceneHierarchyPanel)
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (openNewSceneModal) ImGui::OpenPopup("New Scene Modal");

	DisplayPopups();
}

void EditorLayer::DisplayPopups()
{
	Mule::AssetManager& manager = Mule::AssetManager::Get();

	if (ImGui::BeginPopupModal("New Scene Modal"))
	{
		ImGui::InputText("Name", mInputBuffer, 256);
		// TODO: Add filesystem explorer
		if (ImGui::Button("Create"))
		{
			// validate: name, directory, etc


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

#include "ImGuiLayer.h"

#include "Mule/Mule.h"

#include "imgui.h"

#include "ImGuiPanels/ScenePanel.h"


void ImGuiLayer::OnAttach()
{
	// default panels
	PushPanel(Mule::MakeRef<ScenePanel>());
}

void ImGuiLayer::OnUpdate()
{
	for (Mule::Ref<IPanel> panel : mPanels)
	{
		panel->OnUpdate();
	}
}

void ImGuiLayer::OnDetach()
{
	for (Mule::Ref<IPanel> panel : mPanels)
	{
		panel->OnDetach();
	}
}

void ImGuiLayer::PushPanel(Mule::Ref<IPanel> panel)
{
	panel->OnAttach();
	mPanels.push_back(panel);
}

void ImGuiLayer::RemovePanel(Mule::Ref<IPanel> panel)
{
	auto iter = std::find(mPanels.begin(), mPanels.end(), panel);
	if (iter != mPanels.end())
	{
		panel->OnDetach();
		mPanels.erase(iter);
		return;
	}
	LOG_ERR(L"Could not find panel: {0}", panel->GetName());
}

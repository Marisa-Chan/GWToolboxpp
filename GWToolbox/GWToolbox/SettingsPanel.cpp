#include "SettingsPanel.h"

#include <Windows.h>

#include <GWCA\GWCA.h>

#include "Defines.h"

#include "GuiUtils.h"

#include "GWToolbox.h"
#include "MainWindow.h"

SettingsPanel::SettingsPanel(IDirect3DDevice9* device) {
	D3DXCreateTextureFromFile(device, GuiUtils::getSubPath("settings.png", "img").c_str(), &texture);
}

void SettingsPanel::Draw(IDirect3DDevice9* pDevice) {
	ImGui::SetNextWindowPosCenter(ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiSetCond_FirstUseEver);
	ImGui::Begin(Name(), &visible);
	ImGui::Text("GWToolbox++ version %s by Has and KAOS", GWTOOLBOX_VERSION);
	float w = ImGui::GetWindowContentRegionWidth() / 2;
	if (ImGui::Button("Open Settings Folder", ImVec2(w, 0))) {
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		ShellExecute(NULL, "open", GuiUtils::getSettingsFolder().c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
	ImGui::SameLine();
	if (ImGui::Button("Open GWToolbox++ Website", ImVec2(w, 0))) {
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		ShellExecute(NULL, "open", GWTOOLBOX_WEBSITE, NULL, NULL, SW_SHOWNORMAL);
	}

	for (ToolboxModule* module : GWToolbox::instance().modules) {
		module->DrawSettings();
	}

	if (ImGui::CollapsingHeader("Theme")) {
	}
		
	ImGui::End();
}

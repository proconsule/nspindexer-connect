#include "imgui.h"
#include "popups.h"
#include <vector>
#include <string>

#include <glad/glad.h>
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

namespace Popups {
	
	
	void TitleOptionsPopup(MatchedTitle title){
		Popups::SetupPopup(FormatApplicationId(title.app_id).c_str());
		if (ImGui::BeginPopupModal(FormatApplicationId(title.app_id).c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		
		
		}
		Popups::ExitPopup();
	}
	
	void Separator(void) {
		ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Spacing
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Spacing
	}
	
	int MainOptionsPopup(int current){
		Popups::SetupPopup("Options");
		int retval = current;
		if (ImGui::BeginPopupModal("Options", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			if (ImGui::Button("Show all", ImVec2(200, 50))) {
				retval = 0;
			}
			ImGui::SameLine();
			if (ImGui::Button("On Both", ImVec2(200, 50))) {
				retval = 1;
			}
			Separator();
			if (ImGui::Button("On Switch", ImVec2(200, 50))) {
				retval = 2;
			}
			ImGui::SameLine();
			if (ImGui::Button("On Switch need update", ImVec2(200, 50))) {
				retval = 5;
			}
			Separator();
			if (ImGui::Button("On Server", ImVec2(200, 50))) {
				retval = 3;
			}
			ImGui::SameLine();
			if (ImGui::Button("On Server need update", ImVec2(200, 50))) {
				retval = 4;
			}
		
		
		}
		Popups::ExitPopup();
		return retval;
	}
	
}
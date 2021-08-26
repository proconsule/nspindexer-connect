#ifndef POPUPS_H
#define POPUPS_H

#include "imgui.h"
#include "ServerTitles.h"

namespace Popups {
    inline void SetupPopup(const char *id) {
        ImGui::OpenPopup(id);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
        ImGui::SetNextWindowPos(ImVec2(640.0f, 360.0f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    };
    
    inline void ExitPopup(void) {
        ImGui::EndPopup();
        ImGui::PopStyleVar();
    };

    void TitleOptionsPopup(MatchedTitle title);
	int MainOptionsPopup(int current);
    
}

#endif

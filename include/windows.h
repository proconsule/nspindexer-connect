#ifndef WINDOWS_H
#define WINDOWS_H

#include "imgui.h"
#include <vector>
#include <string>
#include "TitleManager.h"
#include "ServerTitles.h"


namespace Windows {
    inline void SetupWindow(void) {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(880.0f, 720.0f), ImGuiCond_Once);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    };

    inline void ExitWindow(void) {
        ImGui::End();
        ImGui::PopStyleVar();
    };

    void MainWindow(void);
    int LocaltoServerWindow(std::vector<MatchedTitle>,bool focus);
}

namespace DetailWindows {
    inline void SetupWindow(void) {
        ImGui::SetNextWindowPos(ImVec2(880.0f, 0.0f), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(400.0f, 720.0f), ImGuiCond_Once);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    };

    inline void ExitWindow(void) {
        ImGui::End();
        ImGui::PopStyleVar();
    };

    vector<MatchedTitle> DetailServerWindow(std::vector<MatchedTitle>,int idx);
}

#endif

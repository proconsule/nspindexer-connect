#include "imgui.h"
#include "windows.h"
#include <vector>
#include <string>

#include <glad/glad.h>
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

using namespace std;

bool LoadTextureFromMemory(unsigned char* image_data, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    if (image_data == NULL){
        return false;
	}
		
	int jpegSubsamp;
	int _jpegSize = 0x20000;
	
	tjhandle _jpegDecompressor = tjInitDecompress();

	tjDecompressHeader2(_jpegDecompressor, image_data, _jpegSize, &image_width, &image_height, &jpegSubsamp);

	unsigned char buffer[image_width*image_height*3];

	tjDecompress2(_jpegDecompressor, image_data, _jpegSize, buffer, image_width, 0/*pitch*/, image_height, TJPF_RGB,  TJFLAG_FASTDCT);

	tjDestroy(_jpegDecompressor);

	GLuint id = 0;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    

    *out_texture = id;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

namespace Windows {
	static std::string tag_name = std::string();
    
	void Separator(void) {
		ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Spacing
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Spacing
	}
	
	int LocaltoServerWindow(vector<MatchedTitle> mytitles) {
		Windows::SetupWindow();
		static int item_current_idx = 0;
		if (ImGui::Begin("Check Installed VS Server", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
            
			if (ImGui::BeginListBox("Installed Titles",ImVec2(1280.0f, 720.0f)))
            {
            for (int n = 0; n < (int)mytitles.size(); n++)
            {
				const bool is_selected = (item_current_idx == n);
                if (ImGui::Selectable(FormatApplicationId(mytitles[n].app_id).c_str(), is_selected))
                    item_current_idx = n;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
				ImGui::SameLine(200.0f);
				
				if(mytitles[n].isServer && mytitles[n].isSwitch){
					if(mytitles[n].switch_version < mytitles[n].server_version){
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),u8"%s",mytitles[n].titleText.c_str());
					}else if(mytitles[n].switch_version >= mytitles[n].server_version){
						if(mytitles[n].switch_version == mytitles[n].lastlive_version){
							ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f),u8"%s",mytitles[n].titleText.c_str());
						}else{
							ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f),u8"%s",mytitles[n].titleText.c_str());
						}
					}else if(mytitles[n].switch_version >= mytitles[n].server_version){
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f),mytitles[n].titleText.c_str());
					}else{
						ImGui::Text(mytitles[n].titleText.c_str());
					}
					
				}
				else if(mytitles[n].isServer){
					ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f),mytitles[n].titleText.c_str());
					
				}
				else if(mytitles[n].isSwitch){
					ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 0.7f),mytitles[n].titleText.c_str());
					
				}
                
            }
            ImGui::EndListBox();
        }
                }
		
		Windows::ExitWindow();
		return item_current_idx;
	}
}

namespace DetailWindows {
	static std::string tag_name = std::string();

	void Separator(void) {
		ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Spacing
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Spacing
	}

	vector<MatchedTitle> DetailServerWindow(vector<MatchedTitle> mytitles,int idx) {
		DetailWindows::SetupWindow();
		
		if (ImGui::Begin("Details", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse) && idx<(int)mytitles.size()) {
			if(mytitles[idx].icon !=  nullptr){
				if(mytitles[idx].gltexture.id == 0){
					LoadTextureFromMemory(mytitles[idx].icon,&mytitles[idx].gltexture.id,&mytitles[idx].gltexture.width,&mytitles[idx].gltexture.height);
				}else{
					ImGui::Image((void*)(intptr_t)mytitles[idx].gltexture.id, ImVec2(mytitles[idx].gltexture.width,mytitles[idx].gltexture.height));
				}
			}
			ImGui::Text(mytitles[idx].titleText.c_str());
			ImGui::Text(mytitles[idx].authorText.c_str());
			Separator();
			ImGui::Text("Is on Switch:");
			ImGui::SameLine();
			ImGui::TextColored(mytitles[idx].isSwitch ? ImVec4(0.0f,1.0f,0.0f,1.0f) : ImVec4(1.0f,0.0f,0.0f,1.0f) ,mytitles[idx].isSwitch ? "true" : "false");
			ImGui::Text("Is on Server");
			ImGui::SameLine();
			ImGui::TextColored(mytitles[idx].isServer ? ImVec4(0.0f,1.0f,0.0f,1.0f) : ImVec4(1.0f,0.0f,0.0f,1.0f) ,mytitles[idx].isServer ? "true" : "false");
			if(mytitles[idx].isServer && mytitles[idx].isSwitch){
				ImGui::Text("Switch Version: ");
				ImGui::SameLine();
				if(mytitles[idx].switch_version<mytitles[idx].server_version){
					ImGui::TextColored(ImVec4(1.0f,0.0f,0.0f,1.0f),std::to_string(mytitles[idx].switch_version).c_str());
				}else if(mytitles[idx].switch_version>=mytitles[idx].server_version){
					if(mytitles[idx].switch_version<mytitles[idx].lastlive_version){
						ImGui::TextColored(ImVec4(1.0f,1.0f,0.0f,1.0f),std::to_string(mytitles[idx].switch_version).c_str());	
					}else{
						ImGui::TextColored(ImVec4(0.0f,1.0f,0.0f,1.0f),std::to_string(mytitles[idx].switch_version).c_str());
					}
				}
				ImGui::Text("Server Version: ");
				ImGui::SameLine();
				if(mytitles[idx].server_version<mytitles[idx].lastlive_version){
					ImGui::TextColored(ImVec4(1.0f,1.0f,0.0f,1.0f),std::to_string(mytitles[idx].server_version).c_str());	
				}else{
					ImGui::TextColored(ImVec4(0.0f,1.0f,0.0f,1.0f),std::to_string(mytitles[idx].server_version).c_str());	
					
				}
				ImGui::Text("Live Version: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.0f,1.0f,0.0f,1.0f),std::to_string(mytitles[idx].lastlive_version).c_str());
			}
			else if(mytitles[idx].isSwitch){
				ImGui::Text("Switch Version: ");
				ImGui::SameLine();
				ImGui::Text(std::to_string(mytitles[idx].switch_version).c_str());	
			}
		
		}
		Windows::ExitWindow();
		return mytitles;
	}
	
	
}


#include "imgui.h"
#include "windows.h"
#include <vector>
#include <string>



#include "b64.h"

#include <glad/glad.h>
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

using namespace std;





bool LoadTextureFromMemory(unsigned char* image_data,unsigned long _jpegSize, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    if (image_data == NULL){
        return false;
	}
		
	int jpegSubsamp;
	
	
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


ImVec4 GetTitleColor(MatchedTitle thistitle){
	if(thistitle.isServer && thistitle.isSwitch){
		if(thistitle.switch_version < thistitle.server_versions.back()){
				 return ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		}else if(thistitle.switch_version >= thistitle.server_versions.back()){
			if(thistitle.switch_version == thistitle.lastlive_version){
				return ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
			}else{
				return ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
			}
		}else if(thistitle.switch_version >= thistitle.server_versions.back()){
				return ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
		}else{
				return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		}
					
	}else if(thistitle.isServer){
		if(thistitle.server_versions.back()<thistitle.lastlive_version){
			return ImVec4(1.0f, 0.0f, 1.0f, 1.0f);
		}else{
			return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		}
					
	}else if(thistitle.isSwitch){
		return ImVec4(0.5f, 0.5f, 0.5f, 0.7f);
	}
	
	return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
}



namespace Windows {
	static std::string tag_name = std::string();
    
	void Separator(void) {
		ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Spacing
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Spacing
	}
	
	int LocaltoServerWindow(vector<MatchedTitle> mytitles,bool focus) {
		Windows::SetupWindow();
		static int item_current_idx = 0;
		if (ImGui::Begin("Check Installed VS Server", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
            if(focus){
				ImGui::SetNextWindowFocus();
			}
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
				
				ImGui::TextColored(GetTitleColor(mytitles[n]),u8"%s",mytitles[n].titleText.c_str());
				
				if(mytitles[n].isServer && mytitles[n].isSwitch){
					if(mytitles[n].nand_size != 0 || mytitles[n].sd_size != 0){
						ImGui::SameLine(750.0f);
						ImGui::TextColored(GetTitleColor(mytitles[n]),"%s",humanSize(mytitles[n].nand_size+mytitles[n].sd_size).c_str());
					}
                }else if(mytitles[n].isServer){
					ImGui::SameLine(750.0f);
					ImGui::TextColored(GetTitleColor(mytitles[n]),"%s",humanSize(mytitles[n].server_basegame_size).c_str());
				}else if(mytitles[n].isSwitch){
					if(mytitles[n].nand_size != 0 || mytitles[n].sd_size != 0){
						ImGui::SameLine(750.0f);
						ImGui::TextColored(GetTitleColor(mytitles[n]),"%s",humanSize(mytitles[n].nand_size+mytitles[n].sd_size).c_str());
					}
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

	vector<MatchedTitle> DetailServerWindow(vector<MatchedTitle> mytitles,int idx,ServerConfig myserverconfig) {
		DetailWindows::SetupWindow();
		
		if (ImGui::Begin("Details", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse) && idx<(int)mytitles.size()) {
			auto windowWidth = ImGui::GetWindowSize().x;
			if(mytitles[idx].icon !=  nullptr && mytitles[idx].isSwitch){
				if(mytitles[idx].gltexture.id == 0){
					LoadTextureFromMemory(mytitles[idx].icon,mytitles[idx].iconsize,&mytitles[idx].gltexture.id,&mytitles[idx].gltexture.width,&mytitles[idx].gltexture.height);
				}else {
					ImGui::SetCursorPosX((windowWidth - 256) * 0.5f);
					ImGui::Image((void*)(intptr_t)mytitles[idx].gltexture.id, ImVec2(mytitles[idx].gltexture.width,mytitles[idx].gltexture.height));
				}
			} 
			if (!mytitles[idx].server_rominfo && mytitles[idx].isServer){
				if(myserverconfig.enableRomInfo && mytitles[idx].server_rominfo == false){ 
					curlDownloader * tmpcurl = new curlDownloader();
					std::string tmpfilepath = string(serverUrl)+"?rominfo=" + tmpcurl->urlencode(mytitles[idx].server_filePaths.back());
					
					bool okdownload = tmpcurl->download(tmpfilepath);
					Document d;
					d.Parse(tmpcurl->chunk.memory);
					if(d.IsObject()){
						if(!d.HasMember("int")){
							if(!mytitles[idx].isSwitch){
								string gameiconString = d["gameIcon"].GetString();
								mytitles[idx].icon = b64_decode_ex(gameiconString.c_str(),gameiconString.size(),&mytitles[idx].iconsize);
							}
							mytitles[idx].extended_serverinfo.valid = true;
							mytitles[idx].authorText = d["publisher"].GetString();
							mytitles[idx].extended_serverinfo.sdk = d["sdk"].GetString();
							mytitles[idx].extended_serverinfo.humanversion = d["humanVersion"].GetString();
						}
						
					}
					
					mytitles[idx].server_rominfo = true;
					delete tmpcurl;
					
				}else{
					ImGui::SetCursorPosX((windowWidth - 256) * 0.5f);
					if(mytitles[idx].serverFileType == "NSP"){
						ImGui::Image((void*)(intptr_t)dummyNSP.id, ImVec2(dummyNSP.width,dummyNSP.height));
					}
					if(mytitles[idx].serverFileType == "NSZ"){
						ImGui::Image((void*)(intptr_t)dummyNSZ.id, ImVec2(dummyNSZ.width,dummyNSZ.height));
					}
					if(mytitles[idx].serverFileType == "XCI"){
						ImGui::Image((void*)(intptr_t)dummyXCI.id, ImVec2(dummyXCI.width,dummyXCI.height));
					}
					if(mytitles[idx].serverFileType == "XCZ"){
						ImGui::Image((void*)(intptr_t)dummyXCZ.id, ImVec2(dummyXCZ.width,dummyXCZ.height));
					}
				}
				
			}else if(mytitles[idx].icon !=  nullptr && mytitles[idx].isServer && !mytitles[idx].isSwitch){
				if(mytitles[idx].gltexture.id == 0){
					LoadTextureFromMemory(mytitles[idx].icon,mytitles[idx].iconsize,&mytitles[idx].gltexture.id,&mytitles[idx].gltexture.width,&mytitles[idx].gltexture.height);
				}else {
					ImGui::SetCursorPosX((windowWidth - 256) * 0.5f);
					ImGui::Image((void*)(intptr_t)mytitles[idx].gltexture.id, ImVec2(mytitles[idx].gltexture.width,mytitles[idx].gltexture.height));
				}
			}
			auto titleTextwidth   = ImGui::CalcTextSize(mytitles[idx].titleText.c_str()).x;
			ImGui::SetCursorPosX((windowWidth - titleTextwidth) * 0.5f);
			ImGui::Text(mytitles[idx].titleText.c_str());
			auto authorTextwidth   = ImGui::CalcTextSize(mytitles[idx].authorText.c_str()).x;
			ImGui::SetCursorPosX((windowWidth - authorTextwidth) * 0.5f);
			ImGui::Text(mytitles[idx].authorText.c_str());
			Separator();
			ImGui::Text("Is on Switch:");
			ImGui::SameLine();
			ImGui::TextColored(mytitles[idx].isSwitch ? ImVec4(0.0f,1.0f,0.0f,1.0f) : ImVec4(1.0f,0.0f,0.0f,1.0f) ,u8"%s",mytitles[idx].isSwitch ? "\u2714" : "\u2716");
			ImGui::Text("Is on Server");
			ImGui::SameLine();
			ImGui::TextColored(mytitles[idx].isServer ? ImVec4(0.0f,1.0f,0.0f,1.0f) : ImVec4(1.0f,0.0f,0.0f,1.0f) ,u8"%s",mytitles[idx].isServer ? "\u2714" : "\u2716");
			if(mytitles[idx].isServer && mytitles[idx].isSwitch){
				ImGui::Text("Switch Version: ");
				ImGui::SameLine();
				if(mytitles[idx].switch_version<mytitles[idx].server_versions.back()){
					ImGui::TextColored(ImVec4(1.0f,0.0f,0.0f,1.0f),"%d (%s)",mytitles[idx].switch_version,mytitles[idx].switch_humanversion.c_str());
				}else if(mytitles[idx].switch_version>=mytitles[idx].server_versions.back()){
					if(mytitles[idx].switch_version<mytitles[idx].lastlive_version){
						ImGui::TextColored(ImVec4(1.0f,1.0f,0.0f,1.0f),"%d (%s)",mytitles[idx].switch_version,mytitles[idx].switch_humanversion.c_str());	
					}else{
						ImGui::TextColored(ImVec4(0.0f,1.0f,0.0f,1.0f),"%d (%s)",mytitles[idx].switch_version,mytitles[idx].switch_humanversion.c_str());
					}
				}
				ImGui::Text("Server Version: ");
				ImGui::SameLine();
				if(mytitles[idx].server_versions.back()<mytitles[idx].lastlive_version){
					if(mytitles[idx].extended_serverinfo.valid){
						ImGui::TextColored(ImVec4(1.0f,1.0f,0.0f,1.0f),"%d (%s)",mytitles[idx].server_versions.back(),mytitles[idx].extended_serverinfo.humanversion.c_str());	
					}else{
						ImGui::TextColored(ImVec4(1.0f,1.0f,0.0f,1.0f),std::to_string(mytitles[idx].server_versions.back()).c_str());		
					}
				}else{
					if(mytitles[idx].extended_serverinfo.valid){
						ImGui::TextColored(ImVec4(0.0f,1.0f,0.0f,1.0f),"%d (%s)",mytitles[idx].server_versions.back(),mytitles[idx].extended_serverinfo.humanversion.c_str());
					}else{
						ImGui::TextColored(ImVec4(0.0f,1.0f,0.0f,1.0f),std::to_string(mytitles[idx].server_versions.back()).c_str());	
					}
					
					
				}
				ImGui::Text("Live Version: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.0f,1.0f,0.0f,1.0f),std::to_string(mytitles[idx].lastlive_version).c_str());
				ImGui::Text("Server File Type (Basegame): %s",mytitles[idx].serverFileType.c_str());
			}
			else if(mytitles[idx].isSwitch){
				ImGui::Text("Switch Version: ");
				ImGui::SameLine();
				ImGui::Text("%d (%s)",mytitles[idx].switch_version,mytitles[idx].switch_humanversion.c_str());	
			}
			else if(mytitles[idx].isServer){
				ImGui::Text("Server Version: ");
				ImGui::SameLine();
				if(mytitles[idx].server_versions.back()<mytitles[idx].lastlive_version){
					if(mytitles[idx].extended_serverinfo.valid){
						ImGui::TextColored(ImVec4(1.0f,0.0f,0.0f,1.0f),"%d (%s)",mytitles[idx].server_versions.back(),mytitles[idx].extended_serverinfo.humanversion.c_str());	
					}else{
						ImGui::TextColored(ImVec4(1.0f,0.0f,0.0f,1.0f),std::to_string(mytitles[idx].server_versions.back()).c_str());	
					}
				}else{
					if(mytitles[idx].extended_serverinfo.valid){
						ImGui::TextColored(ImVec4(1.0f,1.0f,1.0f,1.0f),"%d (%s)",mytitles[idx].server_versions.back(),mytitles[idx].extended_serverinfo.humanversion.c_str());	
					}else{
						ImGui::TextColored(ImVec4(1.0f,1.0f,1.0f,1.0f),std::to_string(mytitles[idx].server_versions.back()).c_str());	
					}
				}
				ImGui::Text("Live Version: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.0f,1.0f,0.0f,1.0f),std::to_string(mytitles[idx].lastlive_version).c_str());
				ImGui::Text("Server File Type (Basegame): %s",mytitles[idx].serverFileType.c_str());
			}
			if(mytitles[idx].extended_serverinfo.valid){
					Separator();
					ImGui::Text("Extended info (Server Decryption)");
					ImGui::Text("SDK: %s",mytitles[idx].extended_serverinfo.sdk.c_str());
			}
		
		}
		Windows::ExitWindow();
		return mytitles;
	}
	
	
}

namespace FooterWindows {
	static std::string tag_name = std::string();

	void Separator(void) {
		ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Spacing
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Spacing
	}
	void FooterWindow(){
		FooterWindows::SetupWindow();
		if (ImGui::Begin("Device Space", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
			ImGui::Text("SD Card : %s/%s",sdcard_free_space_human.c_str(),sdcard_total_space_human.c_str());
			ImGui::Text("NAND : %s/%s",nand_free_space_human.c_str(),nand_total_space_human.c_str());
		}
		FooterWindows::ExitWindow();
	}
	
	
	
}

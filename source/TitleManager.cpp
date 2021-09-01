#include "TitleManager.h"

string FormatApplicationId(u64 app_id) {
        std::stringstream strm;
        strm << std::uppercase << std::setfill('0') << std::setw(16) << std::hex << app_id;
        return strm.str();
}


TitleManager::TitleManager(){
	
	
}

bool TitleManager::isUpdate(Title *mytitle){
	return mytitle->type == NcmContentMetaType_Patch;
}

bool TitleManager::isAddon(Title *mytitle){
	return mytitle->type == NcmContentMetaType_AddOnContent;
}

u8 *TitleManager::TryGetIcon(Title *mytitle) {
        u8 *icon_data = nullptr;
        NsApplicationControlData control_data = {};
        size_t tmp = 0;
        if(R_SUCCEEDED(nsGetApplicationControlData(NsApplicationControlSource_Storage, mytitle->app_id, &control_data, sizeof(control_data), &tmp))) {
            icon_data = new u8[0x20000]();
            memcpy(icon_data, control_data.icon, 0x20000);
        }
        else if(R_SUCCEEDED(nsGetApplicationControlData(NsApplicationControlSource_Storage, GetBaseApplicationId(mytitle->app_id, mytitle->type), &control_data, sizeof(control_data), &tmp))) {
            icon_data = new u8[0x20000]();
            memcpy(icon_data, control_data.icon, 0x20000);
        }
        return icon_data;
}

NacpStruct TitleManager::TryGetNACP(Title *mytitle){
NsApplicationControlData control_data = {};
        size_t tmp = 0;
        if(R_SUCCEEDED(nsGetApplicationControlData(NsApplicationControlSource_Storage, mytitle->app_id, &control_data, sizeof(control_data), &tmp))) {
            return control_data.nacp;
        }
        else if(R_SUCCEEDED(nsGetApplicationControlData(NsApplicationControlSource_Storage, GetBaseApplicationId(mytitle->app_id, mytitle->type), &control_data, sizeof(control_data), &tmp))) {
            return control_data.nacp;
        }
        return {};
}

string TitleManager::FindNacpName(NacpStruct nacp) {
        NacpLanguageEntry *lang_entry;
        nacpGetLanguageEntry(const_cast<NacpStruct*>(std::addressof(nacp)), &lang_entry);
        if(lang_entry != nullptr) {
            return string(lang_entry->name);
        }
        return string("");
}

string TitleManager::FindHumanVersion(NacpStruct nacp) {
		return string(nacp.display_version);
}


string TitleManager::FindNacpPublisher(NacpStruct nacp) {
        NacpLanguageEntry *lang_entry;
        nacpGetLanguageEntry(const_cast<NacpStruct*>(std::addressof(nacp)), &lang_entry);
        if(lang_entry != nullptr) {
            return string(lang_entry->author);
        }
        return string("");
}



u64 TitleManager::GetBaseApplicationId(u64 app_id, NcmContentMetaType type) {
        switch(type) {
            case NcmContentMetaType_Patch: {
                return app_id ^ 0x800;
            }
            case NcmContentMetaType_AddOnContent: {
                return (app_id ^ 0x1000) & ~0xFFF;
            }
            default: {
                return app_id;
            }
        }
}


void TitleManager::SearchTitles(NcmContentMetaType type, NcmStorageId storage_id) {
        std::vector<Title> titles;
        NcmContentMetaDatabase cnt_meta_db = {};
        if(R_SUCCEEDED(ncmOpenContentMetaDatabase(&cnt_meta_db, storage_id))) {
            auto meta_keys = new NcmContentMetaKey[MaxTitleCount]();
            s32 written = 0;
            s32 total = 0;
            if(R_SUCCEEDED(ncmContentMetaDatabaseList(&cnt_meta_db, &total, &written, meta_keys, MaxTitleCount, type, 0, 0, UINT64_MAX, NcmContentInstallType_Full)) && (written > 0)) {
                for(s32 i = 0; i < written; i++) {
                    auto cur_meta_key = meta_keys[i];
                    Title title = {
                        .app_id = cur_meta_key.id,
                        .type = static_cast<NcmContentMetaType>(cur_meta_key.type),
                        .lastversion = cur_meta_key.version,
                        //.meta_key = cur_meta_key,
                        .storage_id = storage_id,
                    };
                    titles.push_back(title);
                }
            }
            delete[] meta_keys;
            ncmContentMetaDatabaseClose(&cnt_meta_db);
        }
		
		for(auto &cnt: titles) {
            bool ok = true;
            for(int i=0;i<(int)mytitles.size();i++) {
                auto cnt_base_app_id = GetBaseApplicationId(cnt.app_id, cnt.type);
                auto cur_cnt_base_app_id = GetBaseApplicationId(mytitles[i].app_id, mytitles[i].type);
				
                if(cnt_base_app_id == cur_cnt_base_app_id) {
					if(isUpdate(&cnt)){
						mytitles[i].lastversion = cnt.lastversion;
						mytitles[i].update_humanversion = FindHumanVersion(TryGetNACP(&cnt));
						
					
					
					}
                    ok = false;
                    break;
                }
				if(isUpdate(&cnt) || isAddon(&cnt)){
					ok = false;
				}
            }
            if(ok) {
				Title tmpTitle;
				tmpTitle.app_id = cnt.app_id;
				tmpTitle.titleText = FindNacpName(TryGetNACP(&cnt));
				tmpTitle.authorText = FindNacpPublisher(TryGetNACP(&cnt));
				tmpTitle.lastversion = cnt.lastversion;
				tmpTitle.type = cnt.type;
				tmpTitle.icon = TryGetIcon(&cnt);
				tmpTitle.update_humanversion = FindHumanVersion(TryGetNACP(&cnt));
				
				ApplicationOccupiedSize size{};
						auto result = nsCalculateApplicationOccupiedSize(cnt.app_id, (NsApplicationOccupiedSize*)&size);
						if (R_FAILED(result)) {
							printf("failed to get application occupied size for ID %lX\n", tmpTitle.app_id);
							tmpTitle.nand_size = tmpTitle.sd_size = 0;
						} else {
							auto fill_size = [&](const ApplicationOccupiedSizeEntry& e) {
								switch (e.storageId) {
                                case NcmStorageId_BuiltInUser:
										tmpTitle.nand_size = e.sizeApplication + e.sizeAddOnContent + e.sizePatch;
										break;
                                case NcmStorageId_SdCard:
                                    tmpTitle.sd_size = e.sizeApplication + e.sizeAddOnContent + e.sizePatch;
                                    break;
								default:
                                    assert(0 && "unk ncm storageID when getting size!");
                                    break;
								}
							};
							// unsure if the order of the storageID will always be nand then sd.
							// because of this, i manually check using a switch (for now).
							fill_size(size.entry[0]);
							fill_size(size.entry[1]);
							//entry.size_total = entry.size_nand + entry.size_sd;
						}
				

            mytitles.push_back(tmpTitle);
#ifdef DEBUG_NXLINK
			printf("%s %s %s\n",tmpTitle.titleText.c_str(),humanSize(tmpTitle.nand_size).c_str(),humanSize(tmpTitle.sd_size).c_str());
#endif
            }
        }
		
		
		
       
    }
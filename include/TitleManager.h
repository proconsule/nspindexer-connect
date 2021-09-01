#ifndef TITLEMANAGER_H
#define TITLEMANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cassert>

#include <turbojpeg.h>

#include <switch.h>
#include "globals.h"

#include "utils.h"

constexpr u32 MaxTitleCount = 64000;

using namespace std;

string FormatApplicationId(u64 app_id);

/* Taken from iTotalJustice github */
struct ApplicationOccupiedSizeEntry {
    std::uint8_t storageId;
    std::uint64_t sizeApplication;
    std::uint64_t sizePatch;
    std::uint64_t sizeAddOnContent;
};

struct ApplicationOccupiedSize {
    ApplicationOccupiedSizeEntry entry[4];
};



struct Title{
		u64 app_id;
		std::string titleText;
		std::string authorText;
		NcmContentMetaType type;
		u32 lastversion;
		u64 update_id;
		std::string update_humanversion;
		NcmStorageId storage_id;
		
		u64 nand_size;
		u64 sd_size;
		
		u8* icon = nullptr;
};

class TitleManager{
public:
	TitleManager();
	
	bool isUpdate(Title *mytitle);
	bool isAddon(Title *mytitle);
	u64 GetBaseApplicationId(u64 app_id, NcmContentMetaType type);
	NacpStruct TryGetNACP(Title *mytitle);
	u8 *TryGetIcon(Title *mytitle);
	string FindNacpName(NacpStruct nacp);
	string FindNacpPublisher(NacpStruct nacp);
	void SearchTitles(NcmContentMetaType type, NcmStorageId storage_id);
	string FormatApplicationId(u64 app_id);
	string FindHumanVersion(NacpStruct nacp);
	vector<Title> mytitles;
	


};
#endif
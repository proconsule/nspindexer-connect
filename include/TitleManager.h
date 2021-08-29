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

#include <turbojpeg.h>

#include <switch.h>

constexpr u32 MaxTitleCount = 64000;

using namespace std;

string FormatApplicationId(u64 app_id);

struct Title{
		u64 app_id;
		std::string titleText;
		std::string authorText;
		NcmContentMetaType type;
		u32 lastversion;
		NcmStorageId storage_id;
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
	vector<Title> mytitles;
	


};
#endif
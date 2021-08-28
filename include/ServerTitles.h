#ifndef SERVERTITLES_H
#define SERVERTITLES_H

#include <vector>
#include <string>
#include <algorithm>
#include "document.h"

#include <switch.h>
#include "TitleManager.h"

#include <glad/glad.h>
#include "globals.h"


using namespace std;
using namespace rapidjson;


struct ServerTitle{
	u64 app_id;
	std::string titleText;
	std::string serverFileType;
	std::vector<int> versions;
	int lastliveversion = 0;
	std::vector<string> filePaths;
	
};

struct MatchedTitle{
	u64 app_id;
	std::string titleText;
	std::string authorText;
	std::string serverFileType;
	bool isSwitch = false;
	bool isServer = false;
	int switch_version = 0;
	std::vector<string> server_filePaths;
	std::vector<int> server_versions;
	int lastlive_version = 0;
	u8 * icon;
	Tex gltexture;
	
	
};

class ServerTitles{
public:

	ServerTitles(char* jsondata,char * serverconfigjsondata);
	vector<ServerTitle> mytitles;
	vector<MatchedTitle> matchedtitles;
	void Match(vector<Title> switchtitles,int filter = 0);
	
};
#endif
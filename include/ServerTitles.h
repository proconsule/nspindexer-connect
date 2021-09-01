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


struct ServerConfig{
	float version = 0.0;
	std::string contentUrl;
	bool enableRomInfo;
};

struct ServerTitle{
	u64 app_id;
	std::string titleText;
	std::string serverFileType;
	std::vector<int> versions;
	int lastliveversion = 0;
	std::vector<string> filePaths;
	u64 basegame_size;
	
};

struct struct_extended_Serverinfo{
	bool valid = false;
	std::string sdk;
	std::string humanversion;
	
};

struct MatchedTitle{
	u64 app_id;
	std::string titleText;
	std::string authorText;
	std::string serverFileType;
	bool isSwitch = false;
	bool isServer = false;
	bool server_rominfo = false;
	int switch_version = 0;
	u64 sd_size = 0;
	u64 nand_size = 0;
	string switch_humanversion;
	u64 server_basegame_size;
	std::vector<string> server_filePaths;
	std::vector<int> server_versions;
	struct_extended_Serverinfo extended_serverinfo;
	int lastlive_version = 0;
	u8 * icon = nullptr;
	unsigned long iconsize = 0;
	Tex gltexture;
	
	
};

class ServerTitles{
public:
	
	
	ServerTitles();
	bool GetSeverConfig(char *jsondata);
	bool GetServerTitles(char * jsondata);
	ServerConfig myserverconfig;
	vector<ServerTitle> mytitles;
	vector<MatchedTitle> matchedtitles;
	void Match(vector<Title> switchtitles,int filter = 0);
	
};
#endif
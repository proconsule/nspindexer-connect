#ifndef SERVERTITLES_H
#define SERVERTITLES_H

#include <vector>
#include <string>
#include <algorithm>
#include "document.h"

#include <switch.h>
#include "TitleManager.h"

#include <glad/glad.h>


using namespace std;
using namespace rapidjson;


struct ServerTitle{
	u64 app_id;
	std::string titleText;
	int lastversion = 0;
	int lastliveversion;
	
};

typedef struct {
    GLuint id = 0;
    int width = 0;
    int height = 0;
} Tex;


struct MatchedTitle{
	u64 app_id;
	std::string titleText;
	std::string authorText;
	bool isSwitch = false;
	bool isServer = false;
	int switch_version = 0;
	int server_version = 0;
	int lastlive_version = 0;
	u8 * icon;
	Tex gltexture;
	
	
};

class ServerTitles{
public:

	ServerTitles(char* jsondata);
	vector<ServerTitle> mytitles;
	vector<MatchedTitle> matchedtitles;
	void Match(vector<Title> switchtitles,int filter = 0);
	
};
#endif
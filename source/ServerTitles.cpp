#include "ServerTitles.h"

#define MINSERVERVERSION 0.4

bool compareAlphabet(const MatchedTitle& a, const MatchedTitle& b)
{
    return a.titleText < b.titleText;
}

bool ServerTitles::GetSeverConfig(char *jsondata){
	Document config_document;
	config_document.Parse(jsondata);
	if(config_document.IsObject()){	
		myserverconfig.version = atof(config_document["version"].GetString());
		printf("NSP Indexer Server version: %.1f\n",myserverconfig.version);
		if(myserverconfig.version<MINSERVERVERSION){
			printf("Update to a NSP Indexer Server version > %.1f\n",MINSERVERVERSION);
			return false;
		}
		myserverconfig.contentUrl = config_document["contentUrl"].GetString();
		myserverconfig.enableRomInfo = config_document["enableRomInfo"].GetBool();
		
	}else{
		printf("Unable to get NSP Indexer Server Config\n");
		printf("Check your config\n");
		return false;
	}
	return true;
}

ServerTitles::ServerTitles(){
	
}

void ServerTitles::GetServerTitles(char * jsondata){

    Document d;
    d.Parse(jsondata);
	
	const Value& titlesarray = d["titles"];
	for (Value::ConstMemberIterator iter = titlesarray.MemberBegin(); iter != titlesarray.MemberEnd(); ++iter){
		ServerTitle myservertitle;
		myservertitle.app_id = strtoul(iter->name.GetString(), NULL, 16);
		string mytitlekey =  iter->name.GetString();
		const Value& thistitle = titlesarray[mytitlekey.c_str()];
		const Value& thistitleupdates = thistitle["updates"];
		myservertitle.titleText = thistitle["name"].GetString();
#ifdef DEBUG_NXLINK
		printf("%s\n",myservertitle.titleText.c_str());
#endif
		myservertitle.serverFileType = thistitle["fileType"].GetString();
		const rapidjson::Value& lastlive_Value = thistitle["latest_version"];
		if( lastlive_Value.IsNull() )
		{
			myservertitle.lastliveversion = 0;
		}else if ( lastlive_Value.IsString() ){
		    myservertitle.lastliveversion = atoi(thistitle["latest_version"].GetString());
		}else{
			myservertitle.lastliveversion = thistitle["latest_version"].GetInt();
		}
		
		myservertitle.versions.push_back(0);
		myservertitle.filePaths.push_back(thistitle["path"].GetString());
		if(thistitleupdates.IsObject()){
			for (Value::ConstMemberIterator itr = thistitle["updates"].MemberBegin(); itr != thistitle["updates"].MemberEnd(); ++itr){
				myservertitle.versions.push_back(atoi(itr->name.GetString()));
				myservertitle.filePaths.push_back(thistitleupdates[itr->name.GetString()]["path"].GetString());
			}
		}
		
		mytitles.push_back(myservertitle);
		
	}

}

void ServerTitles::Match(vector<Title> switchtitles,int filter){
	for(int i=0;i<(int)mytitles.size();i++){
		MatchedTitle tmpmatched;
		tmpmatched.app_id = mytitles[i].app_id;
		tmpmatched.titleText = mytitles[i].titleText;
		tmpmatched.isServer = true;
		tmpmatched.serverFileType = mytitles[i].serverFileType;
		tmpmatched.server_versions = mytitles[i].versions;
		tmpmatched.lastlive_version = mytitles[i].lastliveversion;
		matchedtitles.push_back(tmpmatched);
	}
	
	for(int i=0;i<(int)switchtitles.size();i++){
		u64 tmpswitchapp_id = 0;
		for(int n=0;n<(int)matchedtitles.size();n++){
			if(switchtitles[i].app_id == matchedtitles[n].app_id){
				matchedtitles[n].authorText = switchtitles[i].authorText;
				matchedtitles[n].isSwitch = true;
				matchedtitles[n].switch_version = switchtitles[i].lastversion;
				matchedtitles[n].icon = switchtitles[i].icon;
				tmpswitchapp_id = matchedtitles[n].app_id;
				break;
			}
		}
		if(tmpswitchapp_id == 0){
			MatchedTitle tmpmatched;
			tmpmatched.app_id = switchtitles[i].app_id;
			tmpmatched.titleText = switchtitles[i].titleText;
			tmpmatched.authorText = switchtitles[i].authorText;
			tmpmatched.isServer = false;
			tmpmatched.isSwitch = true;
			tmpmatched.switch_version  = switchtitles[i].lastversion;
			tmpmatched.icon = switchtitles[i].icon;
			matchedtitles.push_back(tmpmatched);
		}
		
	
	}
	
	sort(matchedtitles.begin(), matchedtitles.end(), compareAlphabet);
	
	vector<MatchedTitle> filteredTitles;
	
	if(filter >0){
		for(int n=0;n<(int)matchedtitles.size();n++){
			if(filter == 1){
				if(matchedtitles[n].isServer && matchedtitles[n].isSwitch){
					filteredTitles.push_back(matchedtitles[n]);
				}
			}
			if(filter == 2){
				if(!matchedtitles[n].isServer && matchedtitles[n].isSwitch){
					filteredTitles.push_back(matchedtitles[n]);
				}
			}
			if(filter == 3){
				if(matchedtitles[n].isServer && !matchedtitles[n].isSwitch){
					filteredTitles.push_back(matchedtitles[n]);
				}
			}
			if(filter == 4){
				if(matchedtitles[n].isServer && !matchedtitles[n].isSwitch && matchedtitles[n].server_versions.back()<matchedtitles[n].lastlive_version){
					filteredTitles.push_back(matchedtitles[n]);
				}
			}
			if(filter == 5){
				if(matchedtitles[n].isServer && matchedtitles[n].isSwitch && matchedtitles[n].switch_version<matchedtitles[n].server_versions.back()){
					filteredTitles.push_back(matchedtitles[n]);
				}
			}
		
		}
		matchedtitles.swap(filteredTitles);
	}
	/*
	for(int i=0;i<matchedtitles.size();i++){
		printf("Appid: %s\n",FormatApplicationId(matchedtitles[i].app_id).c_str());
		printf("Title: %s\n",matchedtitles[i].titleText.c_str());
		printf("\tisServer: %d\n",matchedtitles[i].isServer);
		printf("\tisSwitch: %d\n",matchedtitles[i].isSwitch);
		printf("\tServer Version: %d\n",matchedtitles[i].server_version);
		printf("\tSwitch Version: %d\n",matchedtitles[i].switch_version);
		printf("\tLive Version: %d\n",matchedtitles[i].lastlive_version);
		
	}
	*/
	
}


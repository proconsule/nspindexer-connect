#include "curlDownloader.h"

 
static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  char *ptr = (char *)realloc(mem->memory, mem->size + realsize + 1);
  if(!ptr) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

curlDownloader::curlDownloader(){
	chunk.memory = (char *)malloc(1);  
    chunk.size = 0;    
}

curlDownloader::~curlDownloader(){
	free(chunk.memory);
}

bool curlDownloader::download(string url){
	curl_global_init(CURL_GLOBAL_DEFAULT);
	
	CURL *curl = curl_easy_init();
	
	 if (curl) {
		 consoleUpdate(NULL);
		 curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
 
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
 
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
 
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
 
		CURLcode res = curl_easy_perform(curl);
		
		if(res != CURLE_OK) {
			return false;
			curl_easy_cleanup(curl);
			curl_global_cleanup();
		}
		else {
			curl_easy_cleanup(curl);
			curl_global_cleanup();
			return true;
		}
 
		
		 
		}
	
	return false;
}
#include "curlDownloader.h"

void hexchar(unsigned char c, unsigned char &hex1, unsigned char &hex2)
{
    hex1 = c / 16;
    hex2 = c % 16;
    hex1 += hex1 <= 9 ? '0' : 'a' - 10;
    hex2 += hex2 <= 9 ? '0' : 'a' - 10;
}

string curlDownloader::urlencode(string s)
{
    const char *str = s.c_str();
    vector<char> v(s.size());
    v.clear();
    for (size_t i = 0, l = s.size(); i < l; i++)
    {
        char c = str[i];
        if ((c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            c == '-' || c == '_' || c == '.' || c == '!' || c == '~' ||
            c == '*' || c == '\'' || c == '(' || c == ')' || c == '/')
        {
            v.push_back(c);
        }
        else if (c == ' ')
        {
            v.push_back('+');
        }
        else
        {
            v.push_back('%');
            unsigned char d1, d2;
            hexchar(c, d1, d2);
            v.push_back(d1);
            v.push_back(d2);
        }
    }

    return string(v.cbegin(), v.cend());
}
 
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
  //printf("Donw Size: %d\n",mem->size);
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
 
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 5.1; rv:31.0) Gecko/20100101 Firefox/31.0");
 
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
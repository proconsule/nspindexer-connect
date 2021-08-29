#ifndef CURLDOWNLOADER_H
#define CURLDOWNLOADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

// Include the main libnx system header, for Switch development
#include <switch.h>

#include <curl/curl.h>

struct MemoryStruct {
  char *memory;
  size_t size;
};

using namespace std;

class curlDownloader{
public:
	curlDownloader();
	~curlDownloader();
	bool download(string url);

	struct MemoryStruct chunk;
	string urlencode(string url);


};

#endif
#include "utils.h"


std::string humanSize(uint64_t bytes)
{
	std::vector<std::string>suffix = {"B", "KB", "MB", "GB", "TB"};
	
	int i = 0;
	double dblBytes = bytes;

	if (bytes > 1024) {
		for (i = 0; (bytes / 1024) > 0 && i<(int)suffix.size()-1; i++, bytes /= 1024)
			dblBytes = bytes / 1024.0;
	}

	char output[200];
	sprintf(output, "%.02lf %s", dblBytes, suffix[i].c_str());
	string ret = output;
	return ret;
}
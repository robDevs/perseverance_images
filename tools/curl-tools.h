#ifndef _CURL_TOOLS
#define _CURL_TOOLS

#include <string>

struct stringcurl {
  char *ptr;
  size_t len;
};

extern std::string progress_string;

extern int progress_func(void* ptr, double TotalToDownload, double NowDownloaded, double TotalToUpload, double NowUploaded);

extern void init_string(struct stringcurl *s);

extern size_t writefunc(void *ptr, size_t size, size_t nmemb, struct stringcurl *s);

extern size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data);

size_t write_data_to_disk(void *ptr, size_t size, size_t nmemb, FILE *stream);

extern std::string curl_get_string(std::string url);

//straight from the samples. Downloads a file from  url to file--->see parameters.
extern void curl_download_file(std::string url , std::string file);

#endif
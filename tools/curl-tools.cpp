#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <curl/curl.h>

#include "curl-tools.h"

std::string progress_string;

//progress func for file download.
int progress_func(void* ptr, double TotalToDownload, double NowDownloaded, double TotalToUpload, double NowUploaded) {
    // ensure that the file to be downloaded is not empty
    // because that would cause a division by zero error later on
    if (TotalToDownload <= 0.0) {
        //draw_alert_message(std::to_string(TotalToDownload));
        //return 0;
    }

		// how wide you want the progress meter to be
    int total=320;

    double fractiondownloaded = 0;
    int progress = 0;
    if(TotalToDownload > 0.0) {
        fractiondownloaded = NowDownloaded / TotalToDownload;
        //progress = round(fractiondownloaded * total);
    }

        //if(TotalToDownload > 0.0)
            //vita2d_draw_rectangle(960 / 2 - 160, 544 / 2 - 2, progress, 4, RGBA8(0,255,0,255));

    // if you don't return 0, the transfer will be aborted - see the documentation
    return 0;
}

void init_string(struct stringcurl *s) {
  s->len = 0;
  s->ptr = (char*)malloc(s->len+1);
  if (s->ptr == NULL) {
    return;
    //exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}


size_t writefunc(void *ptr, size_t size, size_t nmemb, struct stringcurl *s){
  size_t new_len = s->len + size*nmemb;
  s->ptr = (char*)realloc(s->ptr, new_len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    return 0;
    //exit(EXIT_FAILURE);
  }
  memcpy(s->ptr+s->len, ptr, size*nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size*nmemb;
}

size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*) ptr, size * nmemb);
    return size * nmemb;
}

size_t write_data_to_disk(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

std::string curl_get_string(std::string url){

	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	if(curl) {
		struct stringcurl body;
		init_string(&body);
		struct stringcurl header;
		init_string(&header);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		// Set useragant string
		curl_easy_setopt(curl, CURLOPT_USERAGENT, \
      "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 \
      (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36");
		// not sure how to use this when enabled
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		// not sure how to use this when enabled
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		// Set SSL VERSION to TLS 1.2
		curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
		// Set timeout for the connection to build
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
		// Follow redirects (?)
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		// The function that will be used to write the data
		std::string response_string;
        std::string header_string;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
		// Internal CURL progressmeter must be disabled if we provide our own callback
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
		// Install the callback function
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_func);
		// write function of response headers
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, writefunc);
		// the response header data where to write to
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header);
		// Request Header :
		struct curl_slist *headerchunk = NULL;
		headerchunk = curl_slist_append(headerchunk, "Accept: */*");
		headerchunk = curl_slist_append(headerchunk, "Content-Type: application/json");
        headerchunk = curl_slist_append(headerchunk, "User-Agent: Mozilla/5.0 \
        (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) \
        Chrome/58.0.3029.110 Safari/537.36");
		res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerchunk);


		// Perform the request
		res = curl_easy_perform(curl);
		int httpresponsecode = 0;
		curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &httpresponsecode);

		if(res != CURLE_OK){
		}
        curl_easy_cleanup(curl);
		return response_string;
	}
	return "Error";
}

//straight from the samples. Downloads a file from  url to file--->see parameters.
//modified to use std::string
void curl_download_file(std::string url , std::string file){
    FILE *fp;

	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	if(curl) {
        fp = fopen(file.c_str(),"wb");
		struct stringcurl body;
		init_string(&body);
		struct stringcurl header;
		init_string(&header);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		// Set useragant string
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36");
		// not sure how to use this when enabled
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		// not sure how to use this when enabled
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		// Set SSL VERSION to TLS 1.2
		curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
		// Set timeout for the connection to build
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
		// Follow redirects (?)
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		// The function that will be used to write the data
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_to_disk);
		// The data filedescriptor which will be written to
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &fp);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
		// Install the callback function
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_func);
		// write function of response headers
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, writefunc);
		// the response header data where to write to
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header);
		// Request Header :
		struct curl_slist *headerchunk = NULL;
		headerchunk = curl_slist_append(headerchunk, "Accept: */*");
		headerchunk = curl_slist_append(headerchunk, "Content-Type: application/json");
		headerchunk = curl_slist_append(headerchunk, "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36");
		headerchunk = curl_slist_append(headerchunk, "Content-Length: 0");
		res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerchunk);


		// Perform the request
		res = curl_easy_perform(curl);
		int httpresponsecode = 0;
		curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &httpresponsecode);

		if(res != CURLE_OK){
			//fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
        else {
            
        }
	}
    else {
        //do error alert here
    }

	// close filedescriptor
	fclose(fp);

	// cleanup
	curl_easy_cleanup(curl);

}
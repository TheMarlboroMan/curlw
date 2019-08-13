#include "tools.h"

#include <curl/curl.h>

#include "curl_request.h"

using namespace tools;

size_t tools::curl_request_body_callback(void *ptr, size_t size, size_t count, void *stream) {

	*((std::string*)stream)+=std::string((char*)ptr, size* count);
	return size* count;
}

size_t tools::curl_request_header_callback(char * buffer, size_t size , size_t nitems, void * stream) {

	((std::string*)stream)->append((char*)buffer, 0, size*nitems);
	return size*nitems;
}

void tools::curl_request_global_init() {

	curl_global_init(CURL_GLOBAL_ALL);
}

std::string tools::curl_request_escape_string(const curl_request& _request, const std::string& c) {

	char * ptr_c=curl_easy_escape(_request.curl ,c.c_str() , c.size());
	std::string str_res(ptr_c);
	curl_free(ptr_c);
	return str_res;
}
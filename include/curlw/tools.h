#pragma once

#include <string>

namespace curlw {

struct curl_request;

//!Callback function to write the response body contents.
size_t 			curl_request_body_callback(void *, size_t , size_t , void *);

//!Callback function to write the response headers.
size_t 			curl_request_header_callback(char *, size_t , size_t , void *);

//!Calls curl_global_init. A good idea to use this before instantiating any curl_request objects.
void 			curl_request_global_init();

//!Creates a URL encoded string from the parameter.
std::string 	curl_request_escape_string(const curl_request&, const std::string&);
}


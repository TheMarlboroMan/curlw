#ifndef TOOLS_CURL_REQUEST_EXCEPTION_H
#define TOOLS_CURL_REQUEST_EXCEPTION_H

#include <stdexcept>
#include <string>
#include <curl/curl.h>

namespace tools {

class curl_request_exception
	:public std::runtime_error {
	public:
			curl_request_exception(const std::string&);
};

//!Exception thrown when two conflicting post data carriers are used.
class curl_request_post_conflict_exception
	:public curl_request_exception {
	public:
			curl_request_post_conflict_exception();
};

//!Exception thrown upon failure when sending the request.
class curl_request_send_exception
	:public curl_request_exception {
	public:
			curl_request_send_exception(const std::string&, const std::string&, const std::string&, CURLcode _c);
	std::string		error,
					url,
					message;
	CURLcode		code;
};

class curl_request_not_sent_exception
	:public curl_request_exception {
	public:
		curl_request_not_sent_exception();
};

}

#endif
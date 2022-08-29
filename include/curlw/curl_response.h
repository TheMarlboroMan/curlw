#pragma once

#include <vector>
#include <string>
#include <curl/curl.h>

namespace curlw {

//!Represents a list of headers for the response.
struct curl_response_header {
	std::string	 				name,
								value;
};

class curl_response {

	public:

	using						headers=std::vector<curl_response_header>;
								curl_response(int, std::string&&, const std::string&);

	//!Returns the response body.
	const std::string&			get_body() const {return body;}

	const std::string&			get_status_line() const {return status_line;}
	//!Returns the response headers.
	const headers&				get_headers() const {return response_headers;}

	//!Returns the status code returned by the last operation.
	long						get_status_code() const {return status_code;}

	std::string					to_string() const;

	private:


	//!Reads the header response data to fill human-readable fields.
	void						process_response_headers(const std::string&);

	long 						status_code;		//!< HTTP status code of the response.
	std::string					status_line,		//!< HTTP status line, first line in the response.
								body;				//!< Will contain the response body upon success.
	headers						response_headers;	//!< Response headers.
};

}

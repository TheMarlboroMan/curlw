#ifndef TOOLS_CURL_REQUEST_H
#define TOOLS_CURL_REQUEST_H

#include <vector>
#include <string>
#include <curl/curl.h>

#include "exception.h"
#include "structures.h"
#include "curl_response.h"

namespace tools {

//!Wrapper around libcurl, designed to quickly do http requests.
class curl_request {

	public:
		
	//!Default constructor.
									curl_request();
	//!Initializes the class with the url set as the parameter.
									curl_request(const std::string&);
	//!Resets the request and destroys the curl handle.
									~curl_request();

	//!Manually sets the payload. Not compatible with form-urlencoded fields.
	//!Payload can only be set once.
	curl_request&					set_payload(const curl_payload&);

	//!Adds the key-value pair to the www-formurlencoded map. Not compatible 
	//!with set_payload. The value will be urlencoded.
	template<typename T>
	curl_request&					add_field(const curl_pair<T>& _pair) {

		if(payload.size()) {
			throw curl_request_post_conflict_exception();		
		}
		
		post_data.push_back({
			curl_request_scape_string(*this, _pair.field),
			std::to_string(_pair.value)
		});
	
		return *this;
	}
	
	//!Adds a header. 
	template<typename T>
	curl_request&					add_header(const curl_pair<T>& _pair) {

		headers.push_back(_pair.name+":"+std::to_string(_pair.value));
		return *this;
	}

	//!Adds a header.
	curl_request&					add_header(const std::string& _header) {

		headers.push_back(_header);
		return *this;
	}
	
	//!Sets accept-decoding parameters, enabling automatic decompression of responses. Set to true by default.
	curl_request&					set_accept_decoding(bool _v);

	//TODO: Where is the set method thingy??????

	//!Enables curl verbose mode.
	curl_request&					set_verbose(bool _v);
			
	//!Sets the proxy location.
	curl_request&					set_proxy(const std::string& _v);

	//!Sets the proxy type according to the list in https://curl.haxx.se/libcurl/c/CURLOPT_PROXYTYPE.html
	curl_request&					set_proxy_type(int _v);

	//!Sets the url to be used in the request.
	curl_request& 					set_url(const std::string& v);
	
	//!Enables curl to follow redirects until it arrives to the final response. Disabled by default.
	curl_request&					set_follow_location(bool v);
	
	//!Executes the request.
	curl_response 					send();

	//!Returns the internal libcurl result code. Throws if we haven't sent.
	CURLcode						get_code() const;

	//!Resets the request so it can be reused.

	//!Resets the sent and recieved data but does nothing to the proxy or
	//!verbosity values. Follow location data is reset.
	void					reset();

	private:

	//!Represents a key-value pair of post form data. For internal use.
	struct post_field {
		std::string name, value;
	};

	std::vector<std::string>		headers;		//!< Request headers.
	std::vector<post_field>			post_data;		//!< Request form post data.

	CURL 							*curl;			//!< Curl handle.
	CURLcode 						res;			//!< Internal curl state, as https://curl.haxx.se/libcurl/c/libcurl-errors.html

	struct curl_httppost 			*formpost;		//!< Internal curl form list beginning.
	struct curl_httppost 			*lastptr;		//!< Internal curl form list ending.
	struct curl_slist 				*headerlist;	//!< Internal curl request header list.

	std::string 					url,			//!< Url to be requested.
									proxy,			//!< Proxy url.
									payload;		//!< Raw post data of the request.

	int								proxy_type;		//!< curl proxy type as in https://curl.haxx.se/libcurl/c/CURLOPT_PROXYTYPE.html
	bool 							follow_location,	//!< Follow location flag.
									verbose,		//!< Verbosity flag.
									accept_decoding,	//!< Decompress flag.
									sent=false;

	friend std::string 				url_scape_string(const curl_request&, const std::string&);
};

}
#endif

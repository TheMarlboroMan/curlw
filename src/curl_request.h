#ifndef TOOLS_CURL_REQUEST_H
#define TOOLS_CURL_REQUEST_H

#include <vector>
#include <string>
#include <curl/curl.h>
#include <stdexcept>

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

//!Wrapper around libcurl, designed to quickly do http requests.
class curl_request {

	public:

	//!Represents a list of headers for the response.
	struct response_header {
		std::string name, value;
	};

	//!Default constructor.
							curl_request();
	//!Initializes the class with the url set as the parameter.
							curl_request(const std::string&);
	//!Resets the request and destroys the curl handle.
							~curl_request();

	//!Calls curl_global_init. A good idea to use this before instantiating any curl_request objects.
	static void				global_init();

	//!Adds the key-value pair of strings to form_data_data fields. If both this and form_data_data fields have been set, an exception is thrown when calling send.
	curl_request&			add_field(const std::string&, const std::string&);
	//!Ads the key value pair (key must be a string, value must be convertible to string) to post data fields. If both this and form_data_data fields have been set, an exception is thrown when calling send.
	template <typename T>
 	curl_request&	 		add_field(const std::string& p_field, const T& p_value) {
		std::string field=url_scape_string(p_field);
		form_data.push_back({field, std::to_string(p_value)});
		return *this;
	}

	//!Manually sets the payload data to the string set. If both this and form_data_data fields have been set, an exception is thrown when calling send.
	curl_request& 			set_payload(std::string const& c);
	//!Adds the string to the header list.
	curl_request&			add_header(const std::string&);
	//!Adds the pair of key-value to the header list.
	curl_request&			add_header(const std::string&, const std::string&);
	//!Executes the request. May throw if both post methods are used or if the request fails.
	curl_request&			send();
	//!Returns the response body. Throws if we haven't sent.
	const std::string&		get_response_body() const;
	//!Returns the response headers. Throws if we haven't sent.
	const std::vector<response_header>&		get_response_headers() const;
	//!Returns the internal libcurl result code. Throws if we haven't sent.
	CURLcode				get_code() const;
	//!Returns the status code returned by the last operation. Throws if we haven't sent.
	long					get_status_code() const;
	//!Sets accept-decoding parameters, enabling automatic decompression of responses. Set to true by default.
	curl_request&			set_accept_decoding(bool v);
	//TODO: Enable redirecting of this somewhere else.
	//!Enables curl verbose mode.
	curl_request&			set_verbose(bool v);
	//!Sets the proxy location.
	curl_request&			set_proxy(const std::string& v);
	//!Sets the proxy type according to the list in https://curl.haxx.se/libcurl/c/CURLOPT_PROXYTYPE.html
	curl_request&			set_proxy_type(int v);
	//!Shorthand to calling set_proxy and set_proxy_type.
	curl_request&			configure_proxy(const std::string& p, int t);
	//!Sets the url to be used in the request.
	curl_request&			set_url(const std::string& v);
	//!Enables curl to follow redirects until it arrives to the final response. Disabled by default.
	curl_request&			set_follow_location(bool v);
	//!Creates a URL encoded string from the parameter.
	std::string				url_scape_string(const std::string&) const;
	//!Resets the request so it can be reused.

	//!Resets the sent and recieved data but does nothing to the proxy or
	//!verbosity values. Follow location data is reset.
	curl_request&			reset();

	private:

	//!Represents a key-value pair of post form data. For internal use.
	struct http_url_encoded_field {
		std::string 		name,
							value;
	};


	//!Reads the header response data to fill human-readable fields.
	void								process_response_headers();
	std::vector<std::string>			headers;		//!< Request headers.
	std::vector<response_header>		response_headers;	//!< Response headers.
	std::vector<http_url_encoded_field>	form_data;		//!< Request form data.

	CURL 								*curl;			//!< Curl handle.
	CURLcode 							res;			//!< Internal curl state, as https://curl.haxx.se/libcurl/c/libcurl-errors.html

	struct curl_httppost 				*formpost;		//!< Internal curl form list beginning.
	struct curl_httppost 				*lastptr;		//!< Internal curl form list ending.
	struct curl_slist 					*headerlist;		//!< Internal curl request header list.

	std::string 						url,			//!< Url to be requested.
		 								response_body,		//!< Will contain the response body upon success.
										proxy,			//!< Proxy url.
		 								str_response_headers,	//!< Will contain the header part of the response upon success.
										payload;		//!< Raw payload.


	long 								status_code;		//!< HTTP status code of the response.
	int									proxy_type;		//!< curl proxy type as in https://curl.haxx.se/libcurl/c/CURLOPT_PROXYTYPE.html
	bool 								follow_location,	//!< Follow location flag.
										verbose,		//!< Verbosity flag.
										accept_decoding,	//!< Decompress flag.
										sent=false;
};

//!Callback function to write the response body contents.
size_t 	curl_request_body_callback(void *, size_t , size_t , void *);
//!Callback function to write the response headers.
size_t 	curl_request_header_callback(char *, size_t , size_t , void *);

}
#endif

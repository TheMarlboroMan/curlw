#include "curl_request.h"
#include <iostream>
#include <cctype>
#include <algorithm>

curl_request::curl_request()
	:curl(nullptr),
	formpost(nullptr),
	lastptr(nullptr),
	headerlist(nullptr),
	proxy_type(-1),
	follow_location(false),
	verbose(false),
	accept_decoding(true) {

}

curl_request::curl_request(const std::string& purl)
	:curl(nullptr),
	formpost(nullptr),
	lastptr(nullptr),
	headerlist(nullptr),
	url(purl),
	proxy_type(-1),
	follow_location(false),
	verbose(false),
	accept_decoding(true) {

}

curl_request::~curl_request() {
	reset();
	curl_easy_cleanup(curl);
}

curl_request& curl_request::set_accept_decoding(bool _v) {

	accept_decoding=_v;
	return *this;
}

curl_request& curl_request::set_verbose(bool _v) {

	verbose=_v;
	return *this;
}

curl_request& curl_request::set_proxy(const std::string& v_) {

	proxy=_v;
	return *this;
}

curl_request& curl_request::set_proxy_type(int _v) {
	
	proxy_type=v;
	return *this;
}

curl_request& curl_request::set_url(const std::string& v) {
	
	url=v;
	return *this;
}

curl_request& curl_request::set_follow_location(bool v) {
	
	follow_location=v;
	return *this;
}

curl_request& curl_request::set_payload(const std::string& _str) {
	
	if(post_data.size()) {
		throw curl_request_post_conflict_exception();		
	}

	payload=_str;
	return *this;
}

void curl_response::send() {
	
	if(!curl) {
		curl=curl_easy_init();
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, tools::curl_request_header_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tools::curl_request_body_callback);	//Función de callback para la response del server.	
	}

	for(const auto& c : headers) {
		headerlist=curl_slist_append(headerlist, c.c_str());
	}

	if(follow_location) {
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	}

	if(verbose) {
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	}

	if(accept_decoding) {
		//Uses all builtin encodings. Apparently it works even if the
		//accept encoding is manually set.
		curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
	}

	//TODO: Fuck this, should be done only once per init.
	char err_buffer[CURL_ERROR_SIZE];
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, err_buffer);

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());	//Decimos a dónde vamos.
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist); //Añadir las headers.

	if(proxy.size()) {
		curl_easy_setopt(curl, CURLOPT_PROXY, proxy.c_str());
		curl_easy_setopt(curl, CURLOPT_PROXYTYPE, proxy_type);
	}

	//TODO: Where is the SET METHOD THINGY????
	if(payload.size()) {
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
	}
	else if(post_data.size()) {

		for(const auto& pd: post_data) {
			curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, pd.name.c_str(), CURLFORM_COPYCONTENTS, pd.value.c_str(), CURLFORM_END);
		}

		//TODO: https://curl.haxx.se/libcurl/c/CURLOPT_CUSTOMREQUEST.html
		//Hmmm... yeah, no, will not do :(
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
	}

	std::string	str_response_headers,
				str_response_body;
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &str_response_headers);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str_response_body);	//Datos que se pasan a la función de callback.

	res=curl_easy_perform(curl);
	int status_code=0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);

	if(res != CURLE_OK) {
		throw curl_request_send_exception(curl_easy_strerror(res), url, err_buffer, res);
	}
	
	sent=true;

	return curl_response(std::move(status_code), std::move(str_response_body), str_response_headers);
}

CURLcode curl_request::get_code() const {

	if(!sent) {
		throw curl_request_not_sent_exception();
	}

	return res;
}

void curl_request::reset() {

	res=CURLE_OK;

	if(headers.size()) {
		curl_slist_free_all(headerlist);
		headerlist=nullptr;
		headers.clear();
	}

	if(post_data.size() && formpost && lastptr) {
		curl_formfree(formpost);
		formpost=nullptr;
		lastptr=nullptr;
		post_data.clear();
	}

	url=std::string();
	payload=std::string();
	follow_location=false;
	accept_decoding=true;
	sent=false;

	if(curl) {
		curl_easy_reset(curl);		
	}
}
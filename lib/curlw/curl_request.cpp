#include <curlw/curl_request.h>
#include <curlw/tools.h>

#include <iostream>
#include <cctype>

using namespace curlw;

curl_request::curl_request()
	:curl(nullptr),
	formpost(nullptr),
	lastptr(nullptr),
	headerlist(nullptr),
	method{methods::GET},
	connection_timeout{default_connection_timeout},
	proxy_type(-1),
	follow_location(false),
	verbose(false),
	accept_decoding(true) {

}

curl_request::curl_request(
	const std::string& purl,
	methods _method
)
	:curl(nullptr),
	formpost(nullptr),
	lastptr(nullptr),
	headerlist(nullptr),
	url(purl),
	method{_method},
	connection_timeout{default_connection_timeout},
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

curl_request& curl_request::set_proxy(const std::string& _v) {

	proxy=_v;
	return *this;
}

curl_request& curl_request::set_proxy_type(int _v) {

	proxy_type=_v;
	return *this;
}

curl_request& curl_request::set_url(const std::string& _v) {

	url=_v;
	return *this;
}

curl_request& curl_request::set_follow_location(bool _v) {

	follow_location=_v;
	return *this;
}

curl_request& curl_request::set_payload(const std::string& _str) {

	check_payload_addition();

	if(post_data.size()) {
		throw curl_request_post_conflict_exception();
	}

	payload=_str;
	return *this;
}

curl_request& curl_request::set_connection_timeout(long _val) {

	if(_val <= 0L) {
		throw curl_request_parameter_exception("set connection timeout must be called with a value larger than zero");
	}

	connection_timeout=_val;
	return *this;
}

curl_request& curl_request::set_method(
	methods _method
) {

	method=_method;
	return *this;
}

curl_response curl_request::send() {

	if(!curl) {
		curl=curl_easy_init();
	}

	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, curlw::curl_request_header_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlw::curl_request_body_callback);	//Función de callback para la response del server.

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

	switch(method) {

		case methods::GET:
			curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
		break;

		case methods::POST:
		case methods::PUT:
		case methods::PATCH:
		case methods::DELETE:

			if(method==methods::POST) {
				curl_easy_setopt(curl, CURLOPT_POST, 1L);
				//curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
			}
			else if(method==methods::PUT) {
				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
			}
			else if(method==methods::PATCH) {
				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
			}
			else if(method==methods::DELETE) {
				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
			}

			if(payload.size()) {
				curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
			}
			else if(post_data.size()) {

				for(const auto& pd: post_data) {
					curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, pd.name.c_str(), CURLFORM_COPYCONTENTS, pd.value.c_str(), CURLFORM_END);
				}
			}
		break;

		case methods::HEAD:
			curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
		break;
	}



	std::string	str_response_headers{},
				str_response_body{};
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &str_response_headers);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str_response_body);	//Datos que se pasan a la función de callback.

	res=curl_easy_perform(curl);
	int status_code=0;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);

	if(res != CURLE_OK) {
		throw curl_request_send_exception(curl_easy_strerror(res), url, err_buffer, res);
	}

	sent=true;
	return curl_response(status_code, std::move(str_response_body), str_response_headers);
}

CURLcode curl_request::get_code() const {

	if(!sent) {
		throw curl_request_not_sent_exception();
	}

	return res;
}

curl_request& curl_request::reset() {

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
	proxy=std::string{};
	payload=std::string();
	method=methods::GET;
	follow_location=false;
	accept_decoding=true;
	sent=false;

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, nullptr);
		curl_easy_reset(curl);
	}

	return *this;
}

void curl_request::check_payload_addition() {

	switch(method) {

		case methods::POST:
		case methods::PUT:
		case methods::PATCH:
		case methods::DELETE:
			return;
		case methods::GET:
			throw curl_request_no_body_allowed_exception("GET");
		case methods::HEAD:
			throw curl_request_no_body_allowed_exception("HEAD");

	}

}

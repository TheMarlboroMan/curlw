#include "curl_request.h"
#include <iostream>
#include <cctype>
#include <algorithm>

using namespace tools;

curl_request_exception::curl_request_exception(const std::string& _str):
	std::runtime_error(_str) {
}

curl_request_post_conflict_exception::curl_request_post_conflict_exception():
	curl_request_exception("curl_request error: cannot have both post string and post fields") {
}

curl_request_send_exception::curl_request_send_exception(const std::string& _e, const std::string& _u, CURLcode _c):
	curl_request_exception("curl_request error : "+_e+" ["+std::to_string(_c)+"] at "+_u) {
}

curl_request_not_sent_exception::curl_request_not_sent_exception():
	curl_request_exception("curl_request_error: the request was not sent, thus response is not accesible") {

}

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

std::string curl_request::get_response_body() const {

	if(!sent) {
		throw curl_request_not_sent_exception();
	}

	return this->response_body;
}

std::vector<curl_request::response_header> curl_request::get_response_headers() const {

	if(!sent) {
		throw curl_request_not_sent_exception();
	}

	return this->response_headers;
}

CURLcode curl_request::get_code() const {

	if(!sent) {
		throw curl_request_not_sent_exception();
	}

	return res;
}

long curl_request::get_status_code() const {

	if(!sent) {
		throw curl_request_not_sent_exception();
	}

	return status_code;
}

void curl_request::reset() {

	res=CURLE_OK;

	if(headers.size()) {
		curl_slist_free_all(headerlist);
		headerlist=nullptr;
		headers.clear();
	}

	response_headers.clear();

	if(post_data.size() && formpost && lastptr) {
		curl_formfree(formpost);
		formpost=nullptr;
		lastptr=nullptr;
		post_data.clear();
	}

	url=std::string();
	response_body=std::string();
	str_response_headers=std::string();
	post_string=std::string();
	status_code=0;
	follow_location=false;
	accept_decoding=true;
	sent=false;

	if(curl) {
		curl_easy_reset(curl);
	}
}

std::string curl_request::url_scape_string(const std::string& c) const {
	char * ptr_c=curl_easy_escape(curl ,c.c_str() , c.size());
	std::string str_res(ptr_c);
	curl_free(ptr_c);
	return str_res;
}

void curl_request::global_init() {
	curl_global_init(CURL_GLOBAL_ALL);
}

void curl_request::post(std::string const& p_field, std::string const& p_value) {
	std::string field=url_scape_string(p_field);
	std::string value=url_scape_string(p_value);
	post_data.push_back({field, value});
}

void curl_request::poststring(std::string const& c) {
	post_string=c;
}

void curl_request::add_header(const std::string& c) {
	headers.push_back(c);
}

void curl_request::add_header(const std::string& k, const std::string& v) {
	headers.push_back(k+":"+v);
}

void curl_request::send() {
	if(!curl) {
		curl=curl_easy_init();
	}

	for(const auto& c : headers) {
		headerlist = curl_slist_append(headerlist, c.c_str());
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

	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, curl_request_header_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_request_body_callback);	//Función de callback para la response del server.
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &str_response_headers);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);	//Datos que se pasan a la función de callback.
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());	//Decimos a dónde vamos.
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist); //Añadir las headers.

	if(proxy.size()) {
		curl_easy_setopt(curl, CURLOPT_PROXY, proxy.c_str());
		curl_easy_setopt(curl, CURLOPT_PROXYTYPE, proxy_type);
	}

	if(post_string.size() && post_data.size()) {
		throw curl_request_post_conflict_exception();
	}

	if(post_string.size()) {
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_string.c_str());
	}
	else if(post_data.size()) {
		for(const auto& pd: post_data) {
			curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, pd.name.c_str(), CURLFORM_COPYCONTENTS, pd.value.c_str(), CURLFORM_END);
		}

		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost); //Esto debe ser para añadir los datos que se van a mandar por POST.
	}

	res=curl_easy_perform(curl);	//Esto es lo que hace realmente la llamada.
	sent=true;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);

	if(res != CURLE_OK) {
		throw curl_request_send_exception(curl_easy_strerror(res), url, res);
	}
	else {
		process_response_headers();
	}
}

//!TODO: This completely ignores the first line with the protocol, response and such.
void curl_request::process_response_headers() {

	auto trim=[](std::string& s) -> std::string {
		auto beg=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
		auto end=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base(); //base turn this into a forward iterator.
		return (end<=beg ? "" : std::string(beg,end));
	};

	size_t ini=0, pos=0;
	while(true) {

		pos=str_response_headers.find(std::string("\n"), pos);
		if(pos==str_response_headers.npos) {
			break;
		}

		auto line=str_response_headers.substr(ini, pos-ini);

		auto colonpos=line.find(":");
		if(colonpos!=line.npos) {
			std::string a=line.substr(0, colonpos), b=line.substr(colonpos+1);
			response_headers.push_back({trim(a), trim(b)});
		}

		ini=pos;
		++pos;
	}
}

size_t tools::curl_request_body_callback(void *ptr, size_t size, size_t count, void *stream) {

	*((std::string*)stream)+=std::string((char*)ptr, size* count);
	return size* count;
}

size_t tools::curl_request_header_callback(char * buffer, size_t size , size_t nitems, void * stream) {

	((std::string*)stream)->append((char*)buffer, 0, size*nitems);
	return size*nitems;
}

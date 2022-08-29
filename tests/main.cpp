#include <iostream>

#include "../include/curlw/curl_request_lib.h"

int main(int /*argc*/, char ** /*argv*/) {

	using namespace curlw;

	try {
		std::cout<<"using "<<curlw::get_lib_version()<<std::endl;
		std::cout<<"macros: "<<CURLW_MAJOR_VERSION<<"."<<CURLW_MINOR_VERSION<<"."<<CURLW_PATCH_VERSION<<std::endl;

		curl_request_global_init();
		curl_request r;

		std::cout<<"POST:"<<r.set_url("https://httpbin.org/post")
			.set_method(curl_request::methods::POST)
			.add_header("accept", "application/json")
			.add_header("content-type", "application/json")
			.set_payload("{\"hi\":\"there\"}")
			.send()
			.to_string()<<std::endl;

		std::cout<<"PATCH"<<r.reset()
			.set_url("https://httpbin.org/patch")
			.set_method(curl_request::methods::PATCH)
			.add_header("accept", "application/json")
			.add_header("content-type", "application/json")
			.set_payload("{\"hi\":\"there\"}")
			.send()
			.to_string()<<std::endl;

		std::cout<<"PUT"<<r.reset().
			set_url("https://httpbin.org/put")
			.set_method(curl_request::methods::PUT)
			.add_header("accept", "application/json")
			.add_header("content-type", "application/json")
			.set_payload("{\"hi\":\"there\"}")
			.send()
			.to_string()<<std::endl;

		std::cout<<"DELETE"<<r.reset().
			set_url("https://httpbin.org/delete")
			.set_method(curl_request::methods::DELETE)
			.add_header("accept", "application/json")
			.add_header("content-type", "application/json")
			.set_payload("{\"hi\":\"there\"}")
			.send()
			.to_string()<<std::endl;

		std::cout<<"GET"<<r.reset().
			set_url("https://httpbin.org/get")
			.set_method(curl_request::methods::GET)
			.add_header("accept", "application/json")
			.add_header("content-type", "application/json")
			.send()
			.to_string()<<std::endl;

		std::cout<<"HEAD"<<r.reset().
			set_url("https://httpbin.org/head")
			.set_method(curl_request::methods::HEAD)
			.add_header("accept", "application/json")
			.add_header("content-type", "application/json")
			.send()
			.to_string()<<std::endl;

		return 0;
	}
	catch(std::exception &e) {

		std::cerr<<"error: "<<e.what()<<std::endl;
		return 1;
	}
}

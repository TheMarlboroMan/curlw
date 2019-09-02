#include <iostream>

#include "../src/curl_request_lib.h"

int main(int /*argc*/, char ** /*argv*/) {

	using namespace tools;

	curl_request_global_init();
	curl_request r;
	
	auto response=r.set_url("https://httpbin.org/post")
		.add_header("accept", "application/json")
		.add_header("content-type", "application/json")
		.set_payload("{\"hi\":\"there\"}")
		.send();

	std::cout<<response.to_string()<<std::endl;

	return 0;
}

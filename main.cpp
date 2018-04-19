#include <iostream>

#include "src/curl_request.h"

int main(int /*argc*/, char ** /*argv*/) {

	using namespace tools;
	curl_request r;
	r.set_url("www.caballorenoir.net");
	r.send();

//	std::cout<<r.get_code()<<" "<<r.get_response()<<std::endl;

	for(const auto& h : r.get_response_headers()) {
		std::cout<<h.name<<":"<<h.value<<std::endl;
	}

	return 1;
}

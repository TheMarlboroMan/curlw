#include "curl_response.h"

#include <algorithm>

using namespace tools;

curl_response::curl_response(int _status_code, std::string&& _body, const std::string& _headers)
	:status_code{_status_code},
	body{_body} {

	process_response_headers(_headers);
}

#include <iostream>

void curl_response::process_response_headers(const std::string& _str) {

	auto trim=[](std::string& s) -> std::string {
		auto beg=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
		auto end=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base(); //base turn this into a forward iterator.
		return (end<=beg ? "" : std::string(beg,end));
	};

	size_t ini=0, pos=0;	
	while(true) {

		pos=_str.find(std::string("\n"), pos);
		if(pos==_str.npos) {
			break;
		}

		auto line=_str.substr(ini, pos-ini);

		if(!status_line.size()) {
			status_line=line;
		}
		else {
			auto colonpos=line.find(":");
			if(colonpos!=line.npos) {
				std::string a=line.substr(0, colonpos), 
							b=line.substr(colonpos+1);
				response_headers.push_back({trim(a), trim(b)});
			}
		}

		ini=pos;
		++pos;
	}
}

std::string curl_response::to_string() const {

	std::string headers;
	for(const auto& r : response_headers) {
		headers+=r.name+":"+r.value+"\n";
	}

	return 	status_line+headers+"\n"+body;
}
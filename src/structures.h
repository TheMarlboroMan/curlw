#ifndef TOOLS_CURL_REQUEST_STRUCTURES_H
#define TOOLS_CURL_REQUEST_STRUCTURES_H

namespace tools {

//!Input structures.
struct curl_payload {

};

//!A pair that can be used to add payload data or headers.
template<typename T>
struct curl_pair {

	std::string				name;
	T						value;
};

}

#endif
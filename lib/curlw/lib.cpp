#include <curlw/lib.h>

#include <sstream>

using namespace curlw;

std::string curlw::get_lib_version() {

	std::stringstream ss;
	ss<<MAJOR_VERSION<<"."<<MINOR_VERSION<<"."<<PATCH_VERSION;
	return ss.str();
}

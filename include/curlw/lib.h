#pragma once

#include <string>

#define CURLW_MAJOR_VERSION MAJOR_VERSION
#define CURLW_MINOR_VERSION MINOR_VERSION
#define CURLW_PATCH_VERSION PATCH_VERSION

namespace curlw {

std::string get_lib_version();
}

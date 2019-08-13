#include "exception.h"

using namespace tools;

curl_request_exception::curl_request_exception(const std::string& _str)
	:std::runtime_error(_str) {
}

curl_request_post_conflict_exception::curl_request_post_conflict_exception()
	:curl_request_exception("curl_request error: cannot have both post string and post fields") {
}

curl_request_send_exception::curl_request_send_exception(const std::string& _e, const std::string& _u, const std::string& _m, CURLcode _c)
	:curl_request_exception("curl_request error : "+_e+" ["+std::to_string(_c)+"] at "+_u+" ["+_m+"]"),
	error{_e},
	url{_u},
	message{_m},
	code{_c}
 	{
}

curl_request_not_sent_exception::curl_request_not_sent_exception()
	:curl_request_exception("curl_request_error: the request was not sent, thus response is not accesible") {

}

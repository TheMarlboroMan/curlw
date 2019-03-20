#include <iostream>
#include <thread>
#include <vector>
#include <numeric>
#include <functional>
#include "src/curl_request.h"

struct job {
	int 					status_code=-1;
	bool					done=false;
	operator	int() {return done ? 1 : 0;}
	void 			request() {
		tools::curl_request r("www.example.com");
		try {
			r.poststring("FIRE AFTER FIRE!!!");
			r.send();
			status_code=r.get_code();
		}
		catch(std::exception& e) {/*I don´t care if it failed*/}
		done=true;
	}
};

int main(int /*argc*/, char ** /*argv*/) {
	tools::curl_request::global_init();
	std::vector<job>	v{10, job{}};
	std::vector<std::thread>	threads;
	for(auto& an : v) {
		threads.push_back(std::thread(&job::request, std::ref(an)));
		threads.back().detach();
	}

	while((int)v.size()!=std::accumulate(std::begin(v), std::end(v), 0));
	for(const auto& an : v) {
		std::cout<<an.status_code<<std::endl;
	}

	return 0;
}

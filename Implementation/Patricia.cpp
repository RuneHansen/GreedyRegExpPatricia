#include "Patricia.h"

void func1(std::list<std::string>* a) {
  for (std::list<std::string>::iterator it = a->begin(); it != a->end(); ++it) {
		if (*it == "aa") {
			(*a).push_back(std::string("aabb"));
			(*a).push_back(std::string("aacd"));
		}
		else if (*it == "ba") {
			(*a).push_back(std::string("babb"));
		}
	}
}

void fillBasic(std::list<std::string>* a) {
	(*a).push_back(std::string("aa"));
	(*a).push_back(std::string("ba"));
}

int main() {
	std::list<std::string> test1;
	update(&test1, fillBasic);
	update(&test1, func1);
	print(test1);

	return 0;
}

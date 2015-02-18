#include "Patricia.h"
#include <string>
#include <list>
#include <iostream>

void func1(std::list<std::string>* a) {
  for (std::list<std::string>::iterator it = a->begin(); it != a->end(); ++it) {
		if (*it == "aa") {
      *it = "";
			(*a).push_back(std::string("aabb"));
			(*a).push_back(std::string("aacd"));
		}
		else if (*it == "ba") {
      *it = "";
			(*a).push_back(std::string("babb"));
		}
	}
}

void fillBasic(std::list<std::string>* a) {
	(*a).push_back(std::string("aa"));
	(*a).push_back(std::string(""));
}

int main() {
	std::list<std::string> test1;
	update(&test1, fillBasic);
  std::cout << "First\n";
	print(test1);
  std::cout << "Second:\n";
	update(&test1, func1);
	print(test1);
  std::string res = split(&test1);
  print(test1);
  std::cout << "\nLast:\n" << res << std::endl;
  char end;
  std::cin >> end;

	return 0;
}

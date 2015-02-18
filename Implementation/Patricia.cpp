#include "Patricia.h"

void func1(std::list<std::string>* a) {
  int size = a.size();
  for(int i = 0; i < size; i++) {
    if(*(a)[i] == "aa") {
      *(a).push_back(std::string("aabb"));
      *(a).push_back(std::string("aacd"));
    } else if (*(a)[i] == "ba") {
      *(a).push_back(std::string("babb"));
    }
  }
}

void fillBasic(std::list<std::string>* a) {
  *(a).push_back(std::string("aa"));
  *(a).push_back(std::string("ba"));
}

int main() {
  std::list<std::string> test1;
  test1.update(&test1, fillBasic);
  test1.update(&test1, func1);
  test1.print(test1);
  
  return 0;
}
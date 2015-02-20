#include "Simple.h"
#include <string>
#include <list>
#include <iostream>
/*
=====  a*  =====

S0[0] = "na"
S0[1] = ""
S0[2] = "0"
S0[3] = "0"
S0[4] = "1"

S1[0] = "na"
S1[1] = "0"
S1[2] = "0"
S1[3] = "0"
S1[4] = "1"

S2[0] = "na"
S2[1] = ""
S2[2] = "0"
S2[3] = ""
S2[4] = "1"

S3[0] = "na"
S3[1] = ""
S3[2] = "0"
S3[3] = "0"
S3[4] = "1"

S4[0] = "na"
S4[1] = "na"
S4[2] = "na"
S4[3] = "na"
S4[4] = "na"

void ma(std::string* S) {
  for (i = 0; i < 5; i++) {
    if (S[i] != "na") S[i] += "0";
  }
}
*/

// =====  {aa + bb}   =====

//S0[0] = "na"
//S0[1] = "na"
//S0[2] = ""
//S0[3] = "na"
//S0[4] = "na"
//S0[5] = "na"
//S0[6] = ""
//S0[7] = "na"

//S2[0] = "na"
//S2[1] = "na"
//S2[2] = "na"
//S2[3] = "na"
//S2[4] = "0"
//S2[5] = "na"
//S2[6] = "na"
//S2[7] = "na"


//transition relation for a specific read char
/*void ma(std::string* S) {
  S0[2] = "na";
  S0[6] = "na";
  S0[4] += "0";
}

void mb(std::string* S) {
  S0[2] = "na";
  S0[6] = "na";
  S0[4] += "1";
}*/

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

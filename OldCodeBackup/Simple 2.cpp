#include "Simple2.h"
#include <string>
#include <list>
#include <iostream>
#include <sstream>
/*
=====  a*  =====

S0[0] = "na"
S0[1] = ""
S0[2] = "0"
S0[3] = "0"
S0[4] = "01"

S1[0] = "na"
S1[1] = "0"
S1[2] = "0"
S1[3] = "0"
S1[4] = "01"

S2[0] = "na"
S2[1] = ""
S2[2] = "0"
S2[3] = ""
S2[4] = "01"

S3[0] = "na"
S3[1] = ""
S3[2] = "0"
S3[3] = "0"
S3[4] = "01"

S4[0] = "na"
S4[1] = "na"
S4[2] = "na"
S4[3] = "na"
S4[4] = "na"

std::string** ma = {{"na", "", "0", "0", "01"},
                   {"na", "0", "0", "0", "01"},
                   {"na", "", "0", "", "01"},
                   {"na", "", "0", "0", "01"},
                   {"na", "na", "na", "na", "na"}};
std::string** me = {{"na", "", "0", "na", "1"},
                    {"na", "na", "0", "na", "1"},
                    {"na", "na", "na", "na", "na"},
                    {"na", "", "0", "na", "1"},
                    {"na", "na", "na", "na", "na"}};

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

void ma(std::string* S) {
  for (int i = 0; i < 5; i++) {
    if (S[i] != "na") S[i] += "0";
  }
}

void me(std::string* S) {
  for (int i = 0; i < 4; i++) {
    S[i] = "na";
  }
}

//S initial state, L is language string compared to the ms, qMax number of states, stream is input stream
//Return is last string from S after stream empty
std::string parse(std::string* S, std::string L, std::string* ms, int qMax, std::istream& stream) {
  char input;
  int lSize = L.size();
  int nr;

  while(stream.get(input)) {
    for(int i = 0; i < lSize; i++) {
      if(L[i] == input) {
        nr = i;
      }
    }
    update(S, qMax, ms + nr*qMax*qMax);
  }

  return S[qMax - 1];
}

int main() {
  std::string S[5];
  S[0] = "";
  S[1] = "";
  S[2] = "0";
  S[3] = "na";
  S[4] = "1";

  std::string ma[25] = {"na", "0", "00", "0", "01",
                          "na", "0", "00", "0", "01",
                          "na", "", "0", "", "1",
                          "na", "0", "00", "0", "01",
                          "na", "na", "na", "na", "na"};
  std::string me[25] = {"na", "", "0", "na", "1",
                          "na", "na", "0", "na", "1",
                          "na", "na", "na", "na", "na",
                          "na", "", "0", "na", "1",
                          "na", "na", "na", "na", "na"};

  std::string test[50] = {"na", "0", "00", "0", "01",
                          "na", "0", "00", "0", "01",
                          "na", "", "0", "", "1",
                          "na", "0", "00", "0", "01",
                          "na", "na", "na", "na", "na",

                          "na", "", "0", "na", "1",
                          "na", "na", "0", "na", "1",
                          "na", "na", "na", "na", "na",
                          "na", "", "0", "na", "1",
                          "na", "na", "na", "na", "na"};


  std::cout << "Before:\n";
  print(S, 5);
  std::istringstream is;
  is.str("aaaaae");
  parse(S, "ae", test, 5, is);
  std::cout << "Stream Done\n";
  print(S, 5);
  /*
  std::cout << "One a:\n";
  update(S, 5, ma);
  print(S, 5);
  std::cout << "Two a:\n";
  update(S, 5, ma);

  print(S,5);

  std::cout << "Epsilon:\n";
  update(S, 5, me);

  print(S, 5);
  */
  char a;
  std::cin >> a;
	return 0;
}

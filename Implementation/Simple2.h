#include <string>
#include <list>
#include <iostream>
#include <stdlib.h>

bool isEmpty(std::string s) {
	return s.empty();
}

//interface for managing a list of strings
/*struct simpleStruct {
std::list<std::string> strings;
void update(void (*m)(std::list<std::string>*));
std::string split();
void print();
};*/

//Q = is the set of states {1, 2, ... , Qmax-1, Qmax}
//S = set of possible paths with the input read so far
//m = represents reading a char, given the current S, returns the new S.
//"na" = is a path that ends in a dead state

//print all possible paths with input read so far
void print(std::string* S, int Qmax) {
  for (int i = 0; i < Qmax; i++) {
    std::cout << S[i] << "\n";
  }
}

//changes the current set and thus S (when a char is read)
void update(std::string* S, void(*m)(std::string* S)) {
	m(S);
}

//Find longest prefix common to all strings in the list,
//remove the prefix from all strings in the list and return the prefix.
std::string split(std::string* S, int Qmax) {
	int count = 0;
	int stringLength = S[0].size(); //breaks at shortest string
	int success = 1;
	char tmp = S[0].at(0);
	for (int c = 0; c < stringLength; c++) {
		for (int i = 0; i < Qmax; i++) {
			if (S[i].at(count) != '\0' && S[i].at(count) != tmp) {
				success = 0;
				break;
			}
		}

		if (!success) {
			break;
		}

		count++;
		tmp = S[0].at(count);
	}

	if (count < 1) {
		return std::string("");
	}
  char* ttmp = (char*) malloc((count + 1) * sizeof(char));
	S[0].copy(ttmp, count, 0);
  ttmp[count] = '\0';
  std::string ret(ttmp);
  free(ttmp);
  for (int i = 0; i < Qmax; i++){
		S[i].erase(0, count);
	}
	return ret;
}

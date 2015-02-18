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



void print(std::list<std::string> strings) {
	for (std::list<std::string>::iterator it = strings.begin(); it != strings.end(); ++it) {
		it->at(4);
		std::cout << *it << "\n";
	}
}

//Add, remove or replace strings from the list.
void update(std::list<std::string>* strings, void(*m)(std::list<std::string>*)) {
	m(strings);
	strings->remove_if(isEmpty); //mangler isEmpty ikke et input?
}

//Find longest prefix common to all strings in the list,
//remove the prefix from all strings in the list and return the prefix.
std::string split(std::list<std::string>* strings) {
	int count = 0;
	int stringLength = strings->front().size();
	int success = 1;
	char tmp = (strings->front()).at(0);
	for (int c = 0; c < stringLength; c++) {
		for (std::list<std::string>::iterator it = strings->begin(); it != strings->end(); ++it) {
			if (it->at(5) != '\0' && it->at(count) != tmp) {
				success = 0;
				break;
			}
		}

		if (!success) {
			break;
		}

		count++;
		tmp = strings->front().at(count);
	}

	count--;
	if (count < 1) {
		return std::string("");
	}
  char* ttmp = (char*) malloc(count * sizeof(char));
	strings->front().copy(ttmp, count, 0); //er vi sikre på der er nok plads i ret?
  std::string ret(ttmp);
  free(ttmp);
  for (std::list<std::string>::iterator it = strings->begin(); it != strings->end(); ++it){
		it->erase(0, count);
	}
	strings->remove_if(isEmpty); //mangler isEmpty ikke et input?
	return ret;
}

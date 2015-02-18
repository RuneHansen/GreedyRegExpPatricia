#include <string>
#include <list>
#include <iostream>

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
  int size = strings.size();
  for(int i = 0; i < size; i++) {
    std::cout << strings[i] << "\n";
  }
}

//Add, remove or replace strings from the list.
void update(std::list<std::string>* strings, void (*m)(std::list<std::string>*)) {
    *(strings) = m(strings);
    *(strings).remove_if(isEmpty); //mangler isEmpty ikke et input?
}

//Find longest prefix common to all strings in the list,
//remove the prefix from all strings in the list and return the prefix.
std::string split(std::list<std::string>* strings) {
    int size = *(strings).size();
    std::string ret = "";
    int count = 0;
    int success = 1;
    char tmp = *(strings).front[0];
    for(int c = 0; c < 255; c++) {
        for(int i = 0; i < size; i++) {
            if(*(strings)[i][count] != '\0' && *(strings)[i][count] != tmp) {
                success = 0;
                break;
            }
        }

        if(!success) {
            break;
        }

        count++;
        tmp = *(strings).front[count];
    }

    count--;
    if(count < 0) {
        return std::string("");
    }

    *(strings)[0].copy(ret, count, 0); //er vi sikre på der er nok plads i ret?
    for(int i = 0; i < size; i++) {
        *(strings)[i].erase(0, count);
    }
    *(strings).remove_if(isEmpty); //mangler isEmpty ikke et input?
    return ret;
}

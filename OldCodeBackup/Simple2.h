#include <string>
#include <list>
#include <iostream>
#include <stdlib.h>
#include <new>

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
//S[X] is the shortest path to state x; after reading the input
//m[Y][X] = Is the shortest path from Y to X that reads a char
//"na" = is a path that ends in a dead state

//print all possible paths with input read so far
void print(std::string* S, int Qmax) {
  for (int i = 0; i < Qmax; i++) {
    if (S[i] != "na") {
      std::cout << "S[" << i << "] = " << S[i] << "\n";
    }
  }
}

//changes the current set and thus S (when a char is read)
void update(std::string* S, const int Qmax, const std::string* m) {

  std::string** newS = (std::string**) malloc(sizeof(std::string) * Qmax);


  for(int i = 0; i < Qmax; i++) {
    std::string* tmp = new std::string();
    newS[i] = tmp;
  }

  for (int i = 0; i < Qmax; i++) {
    (*newS[i]) = "na";
    for (int j = 0; j < Qmax; j++) {
        if (S[j] != "na" && //Is there a path from j to i?
            m[j*Qmax+i] != "na" &&
            ((*newS[i]) == "na" || //Is it the shortest?
             S[j].size()+m[j*Qmax+i].size() < (*newS[i]).size())) {
          (*newS[i]) = S[j]+m[j*Qmax+i];
        }
    }
  }
    for (int i = 0; i < 5; i++) {
        S[i] = (*newS[i]);
    }
  for(int i = 0; i < Qmax; i++) {
    delete newS[i];
  }
  free(newS);
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
  char* ttmp = (char*)malloc((count + 1) * sizeof(char));
  S[0].copy(ttmp, count, 0);
  ttmp[count] = '\0';
  std::string ret(ttmp);
  free(ttmp);
  for (int i = 0; i < Qmax; i++){
    S[i].erase(0, count);
  }
  return ret;
}

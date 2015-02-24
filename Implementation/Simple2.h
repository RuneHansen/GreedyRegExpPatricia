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
void update2(std::string* S, const int Qmax, std::string m[5][5]) {


  std::string newS[5];

  for (int i = 0; i < Qmax; i++) {
    newS[i] = "na";
    for (int j = 0; j < Qmax; j++) {
        //std::string newPath = S[j]+m[j][i];
   // std::cout << "S[j] " << S[j] << " og " << m[j][i] << " og " << newS[i] << "\n";
        if (S[j] != "na" && //Is there a path from j to i?
            m[j][i] != "na" &&
            (newS[i] == "na" || //Is it the shortest?
             S[j].size()+m[j][i].size() < newS[i].size())) {
          newS[i] = S[j]+m[j][i];
        }
    }

  }
    for (int i = 0; i < 5; i++) {
        S[i] = newS[i];
    }
/*
  std::string tmp[5];
  std::string tmp2[5];

  for (int i = 0; i < qMax; i++) {
    for (int j = 0; j < qMax; j++) {
      tmp[j] = "na";
    }
    for (int j = 0; j < qMax; j++) {
      if (S[j] != "na" && m[j][i] != "na"){
        tmp[j] = S[j] + m[j][i];
      }
    }
    for (int j = 1; j < qMax; j++){
      if ((tmp[j].size() < tmp[0].size() || tmp[0] == "na") && tmp[j] != "na"){
        tmp[0] = tmp[j];
      }
    }
    tmp2[i] = tmp[0];
  }

  for (int i = 0; i < qMax; i++) {
    S[i] = tmp2[i];
  }
  */
  /*
  for (int i = 0; i < qMax; i++) {
    tmp = "na";
    for (int j = 0; j < qMax; j++) {
      if (m[j][i] != "na" && S[i] != "na" ) { //&& m[j][i] < tmp
        //if (m[j][i] == "" && tmp != "na") { break; }
        if ((tmp == "na") || m[j][i].size() < tmp.size() || m[j][i].size() == tmp.size() && m[j][i] < tmp)
          tmp = S[j] + m[j][i];
      }
    }

    if (tmp == "na") {
      S[i] = "na";
    }
    else {
      S[i] += tmp;
    }
  }*/
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

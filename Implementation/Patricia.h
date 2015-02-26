#include <string>
#include <list>
#include <iostream>
#include <stdlib.h>
#include <new>

bool isEmpty(std::string s) {
  return s.empty();
}

struct patNode = {
  patNode* parent;
  std::string bitstring;
  patNode* left;
  patNode* right;
};

int longestCommon(std::string &s1, std::string &s2) {
  int slength = s1.size() < s2.size() ? s1.size() : s2.size();
  int count = 0;

  for(int i = 0; i < slength; i++) {
    if(s1[i] == s2[i])
      count++;
    else
      break;
  }
  return count;
}

patNode* insertPat(patNode* node, std::string* s) {
  if(*s == "") {
    return node;
  }

  int child = node->parent->left == node ? 0 : 1;
  int common = 0;
  if(child) {
    common = longestCommon(node->bitstring,
                           node->parent->left->bitstring);
    if(common > 0) {
    std::string tmp0 = node->bitstring.substr(common);
    //tilføj børn ombytning

    node->parent->left->bitstring = node->parent->left->bitstring.substr(common);

    patNode* newParent = (patNode*) malloc(sizeof(patNode));
    patNode* newLeaf = (patNode*) malloc(sizeof(patNode));

    newParent->bitstring = node->bitstring.substr(0, common)
    }

  }

  longestCommon(node->bitstring)



}

/*
patNode* insertPat(patNode* node, std::string* s) {
  if(s == "") {
    return node;
  }
  if(node->left == NULL && node->right == NULL) {
    node->bitstring += *s;
    return node;
  }
  if(s[0] == "0") {
    if(node->left == NULL) {
      node->left = (patNode*) malloc(sizeof(patNode));
      node->left->bitstring = *s;
      return node->left;
    }
      return insertChild(node->left, s);
  }
  if(node->right == NULL) {
    node->right = (patNode*) malloc(sizeof(patNode));
    node->right->bitstring = *s;
    return node->right;
  }

  return insertChild(node->right, s);
}

patNode* insertChild(patNode* node, std::string s) {
  int slength = node->bitstring.size() < s->size() ? node->bitstring.size() : s->size();
  int count = 0;

  for(int i = 0; i < slength; i++) {
    if(s[i] == node->bitstring[i])
      count++;
    else
      break;
  }

  if(count == node->bitstring.size()) {
    return node;
  }

  std::string tmp0 = node->bitstring.substr(count);
  std::string tmp1 = s.substr(count);

  node->bitstring = node->bitstring.substr(0, count);

  if(tmp0.size() == 0) {

  }

  if(tmp1[0] == "0") {
    insert
  }

}*/

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
             S[j]+m[j*Qmax+i] < (*newS[i]))) {
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

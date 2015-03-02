#include "Simple_aNFASimulator.h"
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include "parser/regex.h"
#include "parser/lex.h"
#include "parser/parser.h"


// ============== This is a simple implementation of aNFA simulation ==================

// S[x] is a pointer to the smallest bitstring representing a path to the state x, using the input read so far.
// L is the language, it is a string with one occurence of each character in the language.
// qMax is total number of states in the aNFA.
// stream is the input stream

// simulate() call update() for each character in the input.
// update() updates the initial state S. Update is defined in Simple_aNFASimulator.h

// In this simple implementation bitstrings are represented by strings containing only ones and zeroes.


void aNFAgen(BitC_Regex* E, aNFAnode* i, aNFAnode* f) {
  if (E) {
    switch(E->op) {
    case BitC_RegexOp_Lit :
      i->input = E->litChar;
      i->left = f;
      break;
    case BitC_RegexOp_LitString :
      aNFAnode* prev = (aNFAnode*) malloc (sizeof(aNFAnode));
      i->left = prev;
      prev->left = f;
      prev->input = E->litString[0];
      for(int i = 1; E->litString[i] != '\0'; i++) {
        aNFAnode* cur = (aNFAnode*) malloc (sizeof(aNFAnode));
        prev->left = cur;
        cur->input = E->litString[i];
        cur->left = f;
        prev = cur;
      }
      break;
    case BitC_RegexOp_Plus :
      aNFAnode* s1 = (aNFAnode*) malloc (sizeof(aNFAnode));
      aNFAnode* s2 = (aNFAnode*) malloc (sizeof(aNFAnode));
      aNFAnode* f1 = (aNFAnode*) malloc (sizeof(aNFAnode));
      aNFAnode* f2 = (aNFAnode*) malloc (sizeof(aNFAnode));
      i->left = s1;
      i->right = s2;
      f1->left = f;
      f2->left = f;
      aNFAgen(E->sub1->subs[0],s1,f1);
      aNFAgen(E->sub1->subs[1],s2,f2);
      break;
    case BitC_RegexOp_Star :
      aNFAnode* loop = (aNFAnode*) malloc (sizeof(aNFAnode));
      aNFAnode* q = (aNFAnode*) malloc (sizeof(aNFAnode));
      i->left = loop;
      loop->left = q;
      loop->right = f;
      aNFAgen(E->sub1,q,loop);
      break;
    case BitC_RegexOp_Concat2 :
      aNFAnode* q = (aNFAnode*) malloc (sizeof(aNFAnode));
      aNFAgen(E->subs[0],i,q);
      aNFAgen(E->subs[1],q,f);
      break;
    case BitC_RegexOp_Concat :
      aNFAnode* prev = (aNFAnode*) malloc (sizeof(aNFAnode));
      aNFAgen(E->subs[0],i,prev);
      for(int i = 1; i < E->nsub; i++) {
        aNFAnode* cur = (aNFAnode*) malloc (sizeof(aNFAnode));
        aNFAgen(E->subs[i],prev,cur);
        cur->left = f;
        prev = cur;
      }
      break;
    }
  }
}

// Call update() for each character in the input stream.
void simulate(std::string* S, std::string L, std::string* ms, int qMax, std::istream& stream) {
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
}

// Simulates reading a char
// Change S, that is the current set of paths reachable with the imput read so for.
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

// Find longest prefix common to all strings in the list,
// remove the prefix from all strings in the list and return the prefix.
std::string split(std::string* S, int Qmax) {

  // The index of the characters we are comparing
  int cInd = 0;

  //How many letters we should compare at most, just has to be longer than the shortest string
  int stringLength = S[0].size();

  //Whether we should break the outer loop.
  int success = 1;

  char tmp = S[0].at(0);
  for (int c = 0; c < stringLength; c++) { // Each char
    for (int i = 0; i < Qmax; i++) { // in each string
      // Break at the end of the longest common prefix and/or the shortest string.
      if (S[i].at(cInd) != '\0' && S[i].at(cInd) != tmp) {
        success = 0;
        break;
      }
    }

    // End at the end of the longest common prefix
    if (!success) {
      break;
    }

    cInd++;
    tmp = S[0].at(cInd);
  }

  // cInd is now the index of the final character in the longest common prefix
  // Extract longest common prefix
  std::string prefix;
  if (cInd < 1) {
    prefix = std::string("");
  } else {
    char* ttmp = (char*)malloc((cInd + 1) * sizeof(char));
    S[0].copy(ttmp, cInd, 0);
    ttmp[cInd] = '\0';
    prefix = std::string(ttmp);

    // Free memory
    free(ttmp);
  }
  for (int i = 0; i < Qmax; i++){
    S[i].erase(0, cInd);
  }

  // Return the longest common prefix
  return prefix;
}

// Print all possible bitstring-paths with input read so far
void print(std::string* S, int Qmax) {
  for (int i = 0; i < Qmax; i++) {
    if (S[i] != "na") {
      std::cout << "S[" << i << "] = " << S[i] << "\n";
    }
  }
}

#include <string>
#include <list>
#include <iostream>
#include <stdlib.h>
#include <new>

// This is the header for our simple aNFA simulator.
// It currently holds update(), that does most of the work in the simulaiton process.

// Q is the set of states {1, 2, ... , Qmax-1, Qmax}
// S[X] is the smallest bitstring representing a path to state X, after the input read so far.
// m[Y][X] = Is the shortest path from state Y to state X that reads a char
// "na" = is a path that ends in a dead state.

// Print all possible bitstring-paths with input read so far
void print(std::string* S, int Qmax) {
  for (int i = 0; i < Qmax; i++) {
    if (S[i] != "na") {
      std::cout << "S[" << i << "] = " << S[i] << "\n";
    }
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

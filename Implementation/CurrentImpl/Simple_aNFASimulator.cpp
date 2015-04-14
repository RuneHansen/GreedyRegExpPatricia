#include "Simple_aNFASimulator.h"
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include "regex.h"
#include "lex.h"
#include "parser.h"
#include "tranMatrix.h"


// ============== This is a simple implementation of aNFA simulation ==================

// In this simple implementation bitstrings are represented by strings containing only ones and zeroes.
// The primary function here is 'simulate'. It calls the other functions and is the only function accessible from outside.
// 1. Parse the regular expression.
// 2. Create the aNFA using the functions aNFAgen and addNr.
// 3. Create the transition matrices using the functions buildMatrix and createString.
// 4. Run the simulation using the function update.


// Print all possible bitstring-paths with input read so far
static void printPaths(std::string** S, int numStates) {
  for (int i = 0; i < numStates; i++) {
    if (*(S[i]) != "na") {
      std::cout << "S[" << i << "] = " << *(S[i]) << "\n";
    }
  }
}

// Simulate reading a char.
// Change S, the current set of paths reachable with the imput read so far.
// numStates is the total number of states in the aNFA. and m is a transition matrix.
void update(std::string** S, const int numStates, std::string** m) {

  std::string** newS = (std::string**) malloc(sizeof(std::string) * numStates);

  for(int i = 0; i < numStates; i++) {
    std::string* tmp = new std::string();
    newS[i] = tmp;
  }

  for (int i = 0; i < numStates; i++) {
    (*newS[i]) = "na";
    for (int j = 0; j < numStates; j++) {

      if (*(S[j]) != "na" && //Is there a path from j to i?
          m[j*numStates+i] != NULL &&
          ((*newS[i]) == "na" || //Is it the lexicographically least?
            (*S[j]) + *(m[j*numStates+i]) < *(newS[i]))) {

        (*newS[i]) = *(S[j])+(*m[j*numStates+i]);
      }
    }
  }

  for (int i = 0; i < numStates; i++) {
        *(S[i]) = (*newS[i]);
  }

  for(int i = 0; i < numStates; i++) {
    delete newS[i];
  }

  free(newS);
}

// Run aNFA simulation
// Takes regEx = a regular expression, and test_input = an input string.
// Returns the lexicographically least bitstring,
// representing a path through to the aNFA finishing state, while reading the input.
std::string* s_simulate(std::string regEx, std::istream* input) {

  // Parse the string-form regular expression, to the regular expression type BitC_Regex
  BitC_Regex *regex = NULL;
  BitC_YY_scan_string(regEx.c_str());
  BitC_YYparse(&regex);
  BitC_YYlex_destroy();

  // Generate the aNFA nodes with connections
  std::string language;// will contain one exactly one occurense, of each different literal,
                       // that appear in the regular expression.
  aNFAnode* initialState = aNFAnodeConstructor();
  aNFAnode* finishingState = aNFAnodeConstructor();
  aNFAgen(regex, initialState, finishingState, &language);

  // Add state numbers. numStates is the total number of states.
  int numStates = addNr(initialState, 0);

  // Create transition matrices. allM contains all the transition matrices.
  std::string** allM = (std::string**) malloc(numStates * numStates *
                             language.size() * sizeof(std::string*));
  for(int i = 0; i < language.size(); i++) {
    buildMatrix(initialState, numStates, language.at(i), allM + (i*numStates*numStates));
  }
  //printMatrix(allM, numStates);

  // Create S for the initial state
  // S[q] is the lexicographically least bitstring,
  //  representing a path through the aNFA to state q, using the input read so far.
  std::string** S = (std::string**) malloc(sizeof(std::string*) * numStates);
  for(int i = 0; i < numStates; i++) {
    S[i] = new std::string();
    *S[i] = createString(initialState, i, '\0');
  }

  // Print simulation arguments
  std::cout << "Simple aNFA simulation:\n";
  std::cout << "regex = " << regEx << "\n";

  // Run simulation
  char curChar;
  int lSize = language.size();
  int charNr;// placement in language and allM
  while(input->get(curChar)) {
    for(int i = 0; i < lSize; i++) {
      if(language[i] == curChar) {
        charNr = i;
      }
    }
    update(S, numStates, allM + (charNr*numStates*numStates));
  }


  std::string* output = new std::string();
  *output = *S[finishingState->nr];

  // Free memory
  for(int i = 0; i < numStates; i++) {
    delete S[i];
  }
  free(S);
  freeANFA(initialState, numStates);
  freeMatrix(allM, numStates, language.size());

  return output;
}


// Find longest prefix common to all strings in the list,
// remove the prefix from all strings in the list and return the prefix.
std::string split(std::string** S, int numStates) {

  // The index of the characters we are comparing
  int cInd = 0;

  //How many letters we should compare at most, just has to be longer than the shortest string
  int stringLength = S[0]->size();

  //Whether we should break the outer loop.
  int success = 1;

  char tmp = S[0]->at(0);
  for (int c = 0; c < stringLength; c++) { // Each char
    for (int i = 0; i < numStates; i++) { // in each string
      // Break at the end of the longest common prefix and/or the shortest string.
      if (S[i]->at(cInd) != '\0' && S[i]->at(cInd) != tmp) {
        success = 0;
        break;
      }
    }

    // End at the end of the longest common prefix
    if (!success) {
      break;
    }

    cInd++;
    tmp = S[0]->at(cInd);
  }

  // cInd is now the index of the final character in the longest common prefix
  // Extract longest common prefix
  std::string prefix;
  if (cInd < 1) {
    prefix = std::string("");
  } else {
    char* ttmp = (char*)malloc((cInd + 1) * sizeof(char));
    S[0]->copy(ttmp, cInd, 0);
    ttmp[cInd] = '\0';
    prefix = std::string(ttmp);

    // Free memory
    free(ttmp);
  }
  for (int i = 0; i < numStates; i++){
    S[i]->erase(0, cInd);
  }

  // Return the longest common prefix
  return prefix;
}

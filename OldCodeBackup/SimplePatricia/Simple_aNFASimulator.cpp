#include "Simple_aNFASimulator.h"
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include "regex.h"
#include "lex.h"
#include "parser.h"
#include "aNFAgen.h"


// ============== This is a simple implementation of aNFA simulation ==================

// In this simple implementation bitstrings are represented by strings containing only ones and zeroes.
// The primary function here is 'simulate'. It calls the other functions and is the only function accessible from outside.
// 1. Parse the regular expression.
// 2. Create the aNFA using the functions aNFAgen and addNr.
// 3. Create the transition matrices using the functions buildMatrix and createString.
// 4. Run the simulation using the function update.

// Given a relations matrix and the number of states in the aNFA (the matrix dimentions).
// Prints the matrix, if numStates < 20.
void printMatrix(std::string** matrix, int numStates) {
  if(numStates < 20) {
    for(int i = 0; i < numStates; i++) {
      for(int j = 0; j < numStates; j++) {
        if(*matrix[i*numStates + j] == "") {
          std::cout << "e ";
        } else {
          std::cout << *matrix[i*numStates + j] + " ";
        }
      }
      std::cout << std::endl;
    }
    return;
  }
  std::cout << "The transition matrix is too large to be printed.\n";
}

// Part of the aNFA construction process.
// Given the starting state/node of an aNFA, without state numbers, and the integer 0.
// Gives numbers to each aNFAnode, that are unique within that aNFA.
// Returns the total number of states (numStates)
static int addNr(aNFAnode* E, int nr) {
  //std::cout << "E->nr = " << E->nr << std::endl;
  if(E->nr == 0) {
    E->nr = nr++;
    //std::cout << "Nr: " << nr << ", E->nr: " << E->nr << "\n";
    if(E->right != NULL) {
      return addNr(E->right, addNr(E->left, nr));
    }
    if(E->left != NULL) {
      return addNr(E->left, nr);
    }
  }
  return nr;
}

// Find the aNFAnode with the statenumber n.
// Given E = starting node of the aNFA,
//  and d = the total number of aNFA states.
static aNFAnode* findN(aNFAnode* E, int n, int d) {
  aNFAnode* ret;
  //std::cout << "Looking for " << n << std::endl;
  //std::cout << "Currently in " << E->nr << std::endl;

  if(E->nr == n) {
    //std::cout << "Found it\n";
    return E;
  }
  if(d == 0) {
    return E;
  }

  if(E->nr != n && E->right != NULL) {
    if(rand() % 2) {
      //std::cout << "Took zero\n";
      ret = findN(E->right, n, d-1);
      if(ret->nr != n)
        return findN(E->left, n, d-1);
      return ret;
    } else {
      //std::cout << "Took one\n";
      ret = findN(E->left, n, d-1);
      if(ret->nr != n)
        return findN(E->right, n, d-1);
      return ret;
    }
  }
  if(E->nr != n && E->left != NULL) {
    ret = findN(E->left, n, d-1);
    return ret;
  }
  return E;
}

// Is a given character in a given char class or not?
static bool inCharClass(char c, BitC_CharClass n) {
  // Are there any ranges?
  if(n.nranges == 0) {
    return 0;
  }

  // Is this an "any"?
  if(n.nranges == (size_t) -1) {
    return c != '\0';
  }

  // Are we even looking for anything?
  //  Prevents '\0' from being accepted by uninclusive charCrasses.
  if(c == '\0') {
    return 0;
  }

  // Look for character in all ranges
  int success = !n.inclusive;
  for (int i = 0; i < n.nranges; i++) {
    for (int j = (int) n.ranges[i].from; j <=  (int) n.ranges[i].to; j++) {
      if((int) c == j) {
        return !success;
      }
    }
  }
  return success;
}

// Create the lexicographically least bitstring,
//  representing a path from node E to 'target' in the aNFA and reading c on the way.
static std::string createString(aNFAnode* E, int target, char c) {

  // Is this the node we are looking for?
  if(!c && E->nr == target) {
    if(E->input || E->charClass.nranges != 0 || E->left == NULL)
      return "";
    return "na";
  }

  // Can we pass through this node?
  int comp = ((!E->input  || E->input == c) &&
                E->charClass.nranges == 0
              || inCharClass(c, E->charClass)) ;
  if(!comp) {
    return "na";
  }

  // Is this the finishing state?
  if(E->left == NULL) {
    return "na";
  }

  // Do we read our character when passing through this node?
  if(c && E->right == NULL) {
    if(E->input == c || inCharClass(c, E->charClass)) {
      return createString(E->left, target, '\0');
    }
      return createString(E->left, target, c);
  }

  // A split path, return the best string
  if(E->right != NULL) {
    std::string tmp =  createString(E->left, target, c);
    std::string tmp2 = createString(E->right, target, c);
    if(tmp == "na") {
      if(tmp2 == "na") {
        return tmp2;
      }
      return "1" + tmp2;
    }
    return "0" + tmp;
  }

  // Only one path, no required input
  return createString(E->left, target, c);
}

// Build the transition matrix for a specific character c
// E is the initial node/state of the aNFA.
// 'matrix' is a pointer to allocated space with room enough for the matrix.
static void buildMatrix(aNFAnode* E, int numStates, char c,  std::string** matrix) {
  std::string** retMat = matrix;

  // Set default values
  for(int i = 0; i < numStates*numStates; i++) {
      retMat[i] = new std::string("na");
  }

  // Set all paths
  aNFAnode* tmp;
  for(int i = 0; i < numStates; i++) {
    tmp = findN(E, i, numStates + 1);

    for(int j = 0; j < numStates; j++) {
      *retMat[i*numStates + j] = createString(tmp, j, c );
    }
  }
}



// Free all the transition matrices
static void freeMatrix(std::string** matrix, int mSize, int lSize) {
  for(int l = 0; l < lSize; l++) {
    for(int i = 0; i < mSize; i++) {
      for(int j = 0; j < mSize; j++) {
        delete matrix[l*mSize*mSize + i*mSize + j];
      }
    }
  }
  free(matrix);
}

// Free all the nodes in aNFA
static void freeANFA(aNFAnode* node, int nr) {
  aNFAnode** nodes = (aNFAnode**) malloc(sizeof(aNFAnode*) * nr);
  for(int i = 0; i < nr; i++) {
    nodes[i] = findN(node, i, nr + 1);
  }
  for(int i = 0; i < nr; i++) {
    free(nodes[i]);
  }
  free(nodes);
}

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
static void update(std::string** S, const int numStates, std::string** m) {

  std::string** newS = (std::string**) malloc(sizeof(std::string) * numStates);

  for(int i = 0; i < numStates; i++) {
    std::string* tmp = new std::string();
    newS[i] = tmp;
  }

  for (int i = 0; i < numStates; i++) {
    (*newS[i]) = "na";
    for (int j = 0; j < numStates; j++) {

      if (*(S[j]) != "na" && //Is there a path from j to i?
          (*m[j*numStates+i]) != "na" &&
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
  printMatrix(allM, numStates);

  // Create S for the initial state (using transition matrix epsilon).
  // S[q] is the lexicographically least bitstring,
  //  representing a path through the aNFA to state q, using the input read so far.
  std::string** S = (std::string**) malloc(sizeof(std::string*) * numStates);
  for(int i = 0; i < numStates; i++) {
    S[i] = new std::string();
    *S[i] = createString(initialState, i, '\0');
  }

  // Print simulation arguments
  std::cout << "aNFA simulation:\n";
  std::cout << "regex = " << regEx << "\n";
  //std::cout << "input = " << test_input << "\n";

  /*
  // Test input validity
  for (int i = 0; i < test_input.length(); i++) {
    if (language.find(test_input[i]) == std::string::npos) {
        std::cout << "INVALID INPUT!\n";
        std::cout << "Change the std::string test_input in main().\n";
        std::cout << "Input must be valid for the regular expression.\n";
        std::cout << "Simulation canceled.\n";
        std::string* r = new std::string("Invalid input");
        return r;
    }
  }

  // Convert input string to a stream.
  std::istringstream is;
  is.str(test_input);
  */
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

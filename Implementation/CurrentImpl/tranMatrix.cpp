#include "Simple_aNFASimulator.h"
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include "regex.h"
#include "lex.h"
#include "parser.h"
#include "aNFAgen.h"


// Given a relations matrix and the number of states in the aNFA (the matrix dimentions).
// Prints the matrix, if numStates < 20.
void printMatrix(std::string** matrix, int numStates) {
  if(numStates < 20) {
    for(int i = 0; i < numStates; i++) {
      for(int j = 0; j < numStates; j++) {
        if(matrix[i*numStates + j] != NULL) {
          if(*matrix[i*numStates + j] == "") {
            std::cout << "e ";
          } else {
            std::cout << *matrix[i*numStates + j] + " ";
          }
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
int addNr(aNFAnode* E, int nr) {
  if(E->nr == 0) {
    E->nr = nr++;
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
aNFAnode* findN(aNFAnode* E, int n, int d) {
  aNFAnode* ret;

  if(E->nr == n) {
    return E;
  }
  if(d == 0) {
    return E;
  }

  if(E->nr != n && E->right != NULL) {
    if(rand() % 2) {
      ret = findN(E->right, n, d-1);
      if(ret->nr != n)
        return findN(E->left, n, d-1);
      return ret;
    } else {
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
bool inCharClass(char c, BitC_CharClass n) {
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
  for (int i = 0; i < (int) n.nranges; i++) {
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
std::string createString(aNFAnode* E, int target, char c) {

  // Is this the node we are looking for?
  if(!c && E->nr == target) {
    if(E->input || E->charClass.nranges != 0 || E->left == NULL)
      return "";
    return "na";
  }

  // Can we pass through this node?
  int comp = (((!E->input  || E->input == c) &&
                E->charClass.nranges == 0)
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
void buildMatrix(aNFAnode* E, int numStates, char c,  std::string** matrix) {
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
      if(*retMat[i*numStates + j] == "na") {
        delete retMat[i*numStates + j];
        retMat[i*numStates + j] = NULL;
      }
    }
  }
}



// Free all the transition matrices
void freeMatrix(std::string** matrix, int mSize, int lSize) {
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
void freeANFA(aNFAnode* node, int nr) {
  aNFAnode** nodes = (aNFAnode**) malloc(sizeof(aNFAnode*) * nr);
  for(int i = 0; i < nr; i++) {
    nodes[i] = findN(node, i, nr + 1);
  }
  for(int i = 0; i < nr; i++) {
    free(nodes[i]);
  }
  free(nodes);
}
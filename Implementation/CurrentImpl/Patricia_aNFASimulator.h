#include <string>
#include <list>
#include <iostream>
#include <stdlib.h>
#include <new>
#include "regex.h"


// This is the header for our simple aNFA simulator.

//Patricia trie node
struct patNode {
  patNode* parent;
  std::string* bitstring;
  int active;
  patNode* left;
  patNode* right;
};

//Contains an active state nr and its patricia node representation
struct activeStatePath {
  int nr;
  patNode* node;
};

struct activeStatePath2 {
  int nr;
  std::string* string;
};

// Contains the transition path to nr by string
struct transitionPath {
  int nr;
  std::string* string;
};

// regEx is the regular expression, test_input is the string we read
std::string* p_simulate(std::string regEx, std::istream* input);

std::string* s2_simulate(std::string regEx, std::istream* input);

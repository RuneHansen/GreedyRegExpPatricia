#include <string>
#include <list>
#include <iostream>
#include <stdlib.h>
#include <new>
#include "regex.h"


// This is the header for our simple aNFA simulator.

//Patricia trie node
struct patNode = {
  patNode* parent;
  std::string bitstring;
  patNode* left;
  patNode* right;
};


// regEx is the regular expression, test_input is the string we read
std::string* simulate(std::string regEx, std::string test_input);

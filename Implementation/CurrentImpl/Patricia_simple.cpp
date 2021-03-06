#include "Patricia_aNFASimulator.h"
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include "regex.h"
#include "lex.h"
#include "parser.h"
#include <time.h>
#include <sys/time.h>
#include "tranMatrix.h"


int cstringTime = 0;
int insertPatTime = 0;

// ============== This is a simple implementation of aNFA simulation ==================

// In this simple implementation bitstrings are represented by strings containing only ones and zeroes.
// The primary function here is 'simulate'. It calls the other functions and is the only function accessible from outside.
// 1. Parse the regular expression.
// 2. Create the aNFA using the functions aNFAgen and addNr.
// 3. Create the transition matrices using the functions buildMatrix and createString.
// 4. Run the simulation using the function update.

//Insert postfix s at patricia trie node
patNode* insertPat(patNode* node, std::string* s) {
  //std::cout << "Called insertPat\n";
  
  if(s == NULL || *s == "na") {
    return NULL;
  }

  if(*s == "") {
    node->active = 1;
    return node;
  }
  
  if(node == NULL) {
    return NULL;
  }

  patNode *oldSuffix = (*s)[0] == '1' ? node->right : node->left;

  if(oldSuffix == NULL) {
    oldSuffix = (patNode*) calloc(1, sizeof(patNode));
    oldSuffix->bitstring = new std::string(*s);
    oldSuffix->left = NULL;
    oldSuffix->right = NULL;
    oldSuffix->active = 1;
    oldSuffix->parent = node;
    return oldSuffix;
  }
  
  int isSame, isPrefix, isPostfix, isMixed;
  
  size_t length = s->length() < oldSuffix->bitstring->length() ? oldSuffix->bitstring->length() : s->length();

  size_t i; //length of langest common prefix
  for (i = 0; i < length; i++) {
    if ((*s)[i] != oldSuffix->bitstring->at(i)) {
      break;
    }
  }
  
  isSame = i == s->length() && s->length() == oldSuffix->bitstring->length();
  isPostfix = i == oldSuffix->bitstring->length() && oldSuffix->bitstring->length() < s->length();
  isPrefix = i == s->length();
  isMixed = !isSame && !isPostfix && !isPrefix;
  
  patNode* commonPrefix = (patNode*) calloc(1, sizeof(patNode));


  //they are identical
  //return child
  if (i == s->length() && s->length() == oldSuffix->bitstring->length()) {
    oldSuffix->active = 1;
    return oldSuffix;
  }

  //child.bitstring is a prefix of s
  //call insert on the child with remainder of s
  if (i == oldSuffix->bitstring->length() && oldSuffix->bitstring->length() < s->length()) {
    std::string tmp = s->substr(i);
    return insertPat(oldSuffix, &tmp);
  }


    commonPrefix->bitstring = new std::string(s->substr(0,i));
    commonPrefix->left = NULL;
    commonPrefix->right = NULL;
    commonPrefix->parent = node;
    *(oldSuffix->bitstring) = oldSuffix->bitstring->substr(i);
    oldSuffix->parent = commonPrefix;

  //s is a prefix of child.bitstring
  //new node with s as string and remainder og child.bitstring as child
  if (i == s->length()) {
    if (oldSuffix->bitstring->at(0) == '0') {
      commonPrefix->left = oldSuffix;
    } else {
      commonPrefix->right = oldSuffix;
    }
    commonPrefix->active = 1;
    return commonPrefix;
  }

  //they differ at some point
  //new node with common prefix as string and remainder of each as children
    patNode* ourSuffix = (patNode*) calloc(1, sizeof(patNode));
    ourSuffix->bitstring = new std::string(s->substr(i));
    ourSuffix->left = NULL;
    ourSuffix->right = NULL;
    ourSuffix->parent = commonPrefix;

    if (oldSuffix->bitstring->at(0) == '0') {
      commonPrefix->left = oldSuffix;
      commonPrefix->right = ourSuffix;
    } else {
      commonPrefix->right = oldSuffix;
      commonPrefix->left = ourSuffix;
    }
    ourSuffix->active = 1;
    return ourSuffix;
}


// GetString returns the string generated by running to the root.
// The returned string is as from reading from the root.
std::string getStringFromPat(patNode* node) {
  if(node->parent == NULL) {
    return "";
  }
  
  return getStringFromPat(node->parent) + *(node->bitstring);
}

//If the root only has one child, it will output the longest common prefix
std::string split(patNode* node) {
  if(node->left == NULL && node->right != NULL) {
    node->right->parent = NULL;
    std::string ret = *(node->right->bitstring);
    *(node->right->bitstring) = "";
    patNode* tmp = node;
    node = tmp->right;
    delete tmp->bitstring;
    free(tmp);
    return ret;
  }
  if(node->right == NULL && node->left != NULL) {
    node->left->parent = NULL;
    std::string ret = *(node->left->bitstring);
    *(node->left->bitstring) = "";
    patNode* tmp = node;
    node = tmp->left;
    delete tmp->bitstring;
    free(tmp);
    return ret;
  }
  return "";
}

// Recursively runs through the tree removing dead leaves and
// connects single branches if they are inactive.
void cleanUp(patNode* node) {
  if(node->left != NULL) {
    cleanUp(node->left);
  }
  if(node->right != NULL) {
    cleanUp(node->right);
  }
  if(node->active) {
    return;
  }
  if(node->left == NULL && node->right == NULL) {
    if(node->parent->left == node) {
      node->parent->left = NULL;
    } else {
      node->parent->right = NULL;
    }
    delete node->bitstring;
    free(node);
  }
  if(node->left != NULL && node->right == NULL) {
    *(node->left->bitstring) = *(node->bitstring) + *(node->left->bitstring);
    node->left->parent = node->parent;
    if(node->parent->left == node) {
      node->parent->left = node->left;
    } else {
      node->parent->right = node->left;
    }
    delete node->bitstring;
    free(node);
  }
  if (node->right != NULL && node->left == NULL) {
    *(node->right->bitstring) = *(node->bitstring) + *(node->right->bitstring);
    node->right->parent = node->parent;
    if(node->parent->right == node) {
      node->parent->right = node->right;
    } else {
      node->parent->left = node->right;
    }
    delete node->bitstring;
    free(node);
  }
}

long totalTime = 0;

// A simple update implementation for applying a single character
// runs through all combinations and inserts the new nodes.
// updating the given S.
void update(patNode** S, const int numStates, std::string** m) {
  struct timeval time;
  
  patNode** newS = (patNode**) calloc(numStates, sizeof(patNode*));
  std::string** ending = calloc(numStates, sizeof(std::string*));
  
  gettimeofday(&time, NULL);
  long timespent = (time.tv_sec*1000) + (time.tv_usec / 1000);
  
  for (int i = 0; i < numStates; i++) {
    for (int j = 0; j < numStates; j++) {
     if (S[j] != NULL && //Is there a path from j to i?
          m[j*numStates+i] != NULL &&
          (newS[i] == NULL || //Is it the lexicographically least?
           getStringFromPat(S[j]) + *m[j*numStates+i] < getStringFromPat(newS[i]) + *ending[i])) {
            ending[i] = m[j*numStates+i];
            newS[i] = S[j];
      }
    }
  }

  gettimeofday(&time, NULL);  //END-TIME

  timespent = (((time.tv_sec * 1000) + (time.tv_usec / 1000)) - timespent);
  
  totalTime += timespent;
  
  for (int i = 0; i < numStates; i++) {
    if( S[i] != NULL)
      S[i]->active = 0;
    S[i] = insertPat(newS[i], ending[i]);
  }
  free(newS);
  free(ending);
}

//Frees all the memory from a patricia trie, starting at node.
void freePat(patNode *node) {
  if (node->left != NULL) {
    freePat(node->left);
  }
  if (node->right != NULL) {
    freePat(node->right);
  }
  free(node);
}

//Prints the node and its children values, for testing
void printPat(patNode* node) {
  std::cout << "Mine : " << *(node->bitstring) << "\n";
  std::cout << "Left\n";
  printPat(node->left);
  std::cout << "Right\n";
  printPat(node->right);
}

// Run aNFA simulation
// Takes regEx = a regular expression, and test_input = an input string.
// Returns the lexicographically least bitstring,
// representing a path through to the aNFA finishing state, while reading the input.
std::string* ps_simulate(std::string regEx, std::istream* input) {

  // Parse the string-form regular expression, to the regular expression type BitC_Regex
  BitC_Regex *regex = NULL;
  BitC_YY_scan_string(regEx.c_str());
  BitC_YYparse(&regex);
  BitC_YYlex_destroy();

  // Generate the aNFA nodes with connections
  std::string alphabet;// will contain one exactly one occurense, of each different literal,
                       // that appear in the regular expression.
  aNFAnode* initialState = aNFAnodeConstructor();
  aNFAnode* finishingState = aNFAnodeConstructor();
  aNFAgen(regex, initialState, finishingState, &alphabet);

  
  // Add state numbers. numStates is the total number of states.
  int numStates = addNr(initialState, 0);

  // Create transition matrices. allM contains all the transition matrices.
  std::string** allM = (std::string**) malloc(numStates * numStates *
                             alphabet.size() * sizeof(std::string*));
  for(size_t i = 0; i < alphabet.size(); i++) {
    buildMatrix(initialState, numStates, alphabet.at(i), allM + (i*numStates*numStates));
  }
  
  //printMatrix(allM, numStates);
  
  // Create S for the initial state (using transition matrix epsilon).
  // S[q] is the lexicographically least bitstring,
  //  representing a path through the aNFA to state q, using the input read so far.
  patNode* root = (patNode*) calloc(1, sizeof(patNode));
  root->active = 1;
  root->bitstring = new std::string("");
  
  
  patNode** S = (patNode**) malloc(sizeof(patNode*) * numStates);
  for(int i = 0; i < numStates; i++) {
    std::string tmp = createString(initialState, i, '\0');
    S[i] = insertPat(root, &tmp);
  }

  // Print simulation arguments
  std::cout << "aNFA simulation:\n";
  std::cout << "regex = " << regEx << "\n";
  //std::cout << "input = " << test_input << "\n";
  
  // Run simulation
  char curChar;
  int lSize = alphabet.size();
  int charNr;// placement in alphabet and allM
  int i_nr = 0;
  
  std::string* output = new std::string("");
  
  while(input->get(curChar)) {
    for(int i = 0; i < lSize; i++) {
      if(alphabet[i] == curChar) {
        charNr = i;
      }
    }
    std::cout << "Doing update\n";
    update(S, numStates, allM + (charNr*numStates*numStates));
    std::cout << "Done update\n";
    root->active = 1;
    cleanUp(root);
    std::cout << "Done cleanup\n";
    *output += split(root);
    std::cout << "Done split\n";

    i_nr++;
  }


  std::string check = "na";

  std::string check = getStringFromPat(S[finishingState->nr]);
  if(check == "na") {
    *output = "na";
  } else {
    *output += check;
  }
  std::cout << "Total duration of triple loop " << (double) totalTime / 1000 << std::endl;
  //free(S);
  //delete S;
  freeANFA(initialState, numStates);
  freeMatrix(allM, numStates, alphabet.size());
  freePat(root);

  return output;
}
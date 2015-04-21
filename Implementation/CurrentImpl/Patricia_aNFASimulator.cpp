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
  
  if(s == NULL || *s == "na" || node == NULL) {
    std::cout << "inesertPat returned NULL\n";
    return NULL;
  }

  if(*s == "") {
    node->active = 1;
    return node;
  }

  patNode *oldSuffix = (*s)[0] == '1' ? node->right : node->left;

  if(oldSuffix == NULL) {
    oldSuffix = (patNode*) calloc(1, sizeof(patNode));
    oldSuffix->bitstring = new std::string(*s);
    oldSuffix->left = NULL;
    oldSuffix->right = NULL;
    oldSuffix->active = 1;
    oldSuffix->parent = node;
    if((*s)[0] == '1') {
      node->right = oldSuffix;
    } else {
      node->left = oldSuffix;
    }
    return oldSuffix;
  }
  
  int isSame, isPrefix, isPostfix, isMixed;
  
  size_t length = s->length() < oldSuffix->bitstring->length() ? s->length() : oldSuffix->bitstring->length();

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
  if (isSame) {
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
    if(commonPrefix->bitstring->at(0) == '0') {
      node->left = commonPrefix;
    } else {
      node->right = commonPrefix;
    }
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

//Compares two transitionPath strings
bool compare_transitionPath(const transitionPath& s1, const transitionPath& s2) {
  return *s1.string < *s2.string;
}

//Prints the lists of the new M array
void printNewM(std::list<transitionPath>** newM, int numStates, int lSize) {
  for(int i = 0; i < lSize; i++) {
    std::cout << "Letter i = " << i << "\n";
    for(int j = 0; j < numStates; j++) {
      for (std::list<transitionPath>::iterator it=newM[i*numStates + j]->begin(); it != newM[i*numStates + j]->end(); ++it) {
        std::cout << "it->string " << *it->string << ", nr=" << it->nr << ", "; 
      }
    std::cout << "\n";
    }
    std::cout << "\n";
  }
}

// Construct the new M matrix from the old form
// newM contains a sorted list for each state. 
// Each list contains all paths from a specific state, though not NULLs.
// 
void buildNewM(std::string** matrix, std::list<transitionPath>** newM, int numStates, int lSize) {
  for(int i = 0; i < lSize; i++) {
    for(int j = 0; j < numStates; j++) {
    std::list<transitionPath>* current = new std::list<transitionPath>;
    
    for(int k = 0; k < numStates; k++) {
      if(matrix[i*numStates*numStates + j * numStates + k] != NULL) {
        transitionPath* tmp = (transitionPath*) calloc(1, sizeof(transitionPath));
        tmp->string = matrix[i*numStates*numStates + j * numStates + k];
        tmp->nr = k;
        current->push_back(*tmp);
      }
    }
    
    current->sort(compare_transitionPath);
    newM[i*numStates + j] = current;
    
    }
  }
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
std::string split(patNode** node) {
  if((*node)->active) {
    return "";
  }
  if((*node)->left == NULL && (*node)->right != NULL) {
    (*node)->right->parent = NULL;
    std::string ret = *((*node)->right->bitstring);
    *((*node)->right->bitstring) = "";
    patNode* tmp = *node;
    
    *node = (*node)->right;
    
    delete tmp->bitstring;
    free(tmp);
    return ret;
  }
  if((*node)->right == NULL && (*node)->left != NULL) {
    (*node)->left->parent = NULL;
    std::string ret = *((*node)->left->bitstring);
    *((*node)->left->bitstring) = "";
    patNode* tmp = *node;

    *node = (*node)->left;

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
  if(node->active || node->parent == NULL) {
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
    return;
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
    return;
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
    return;
  }
}

long totalTime = 0;

// Update takes the newM matrix and a sorted list of active states
// For each active state i beginning with the lexicographically least,
// for each path to j from that state beginning with the lexicographically least
// if we have not already found a path to j we add that path to newS[j]
// and update found[j].
void update(std::list<activeStatePath>* S, const int numStates, std::list<transitionPath>** m) {
    std::list<activeStatePath> newS; // = new std::list<activeStatePath>();
    // Contains the currently found new states
    int *found = calloc(numStates, sizeof(int));
    
    for (std::list<activeStatePath>::iterator it=S->begin(); it != S->end(); ++it) {
      it->node->active = 0;
      for(std::list<transitionPath>::iterator pf=m[it->nr]->begin(); pf != m[it->nr]->end(); ++pf) {
        if(!found[pf->nr]) {
            activeStatePath* tmp = (activeStatePath*) malloc(sizeof(activeStatePath));
            tmp->nr = pf->nr;
            tmp->node = insertPat(it->node, pf->string);
            found[pf->nr] = 1;
            newS.push_back(*tmp);
        }
      }
    }
    
    free(found);
    *S = newS;
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

//Compares two nodes strings, used for sorting.
bool compare_activeStatePath(const activeStatePath& s1, const activeStatePath& s2) {
  return getStringFromPat(s1.node) < getStringFromPat(s2.node);
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
std::string* p_simulate(std::string regEx, std::istream* input) {

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
  
  // Create newM
  std::list<transitionPath>** newM = calloc(numStates*alphabet.size(), sizeof(std::list<transitionPath>*));
  buildNewM(allM, newM, numStates, alphabet.size());
  
  //printNewM(newM, numStates, alphabet.size());

  // Create S for the initial state
  // S is a list containing a patnode for each active state q
  // representing the lexicographically least path through the aNFA to 
  // state q, using the input read so far.
  patNode* root = (patNode*) calloc(1, sizeof(patNode));
  //root->active = 1;
  root->bitstring = new std::string("");

  std::cout << "Making S\n";
  std::list<activeStatePath>* S = new std::list<activeStatePath>();
  for(int i = 0; i < numStates; i++) {
    std::string tmp = createString(initialState, i, '\0');
    if(tmp != "na") {
      activeStatePath* newActiveStatePath = malloc(sizeof(activeStatePath));
      newActiveStatePath->nr = i;
      newActiveStatePath->node = insertPat(root, &tmp); 
      S->push_back(*newActiveStatePath);
    }
  }
  S->sort(compare_activeStatePath);

  // Print simulation arguments
  std::cout << "Patricia aNFA simulation:\n";
  std::cout << "regex = " << regEx << "\n";
  
  // Run simulation
  char curChar;
  int lSize = alphabet.size();
  int charNr;// placement in alphabet and allM
  int i_nr = 0;
  /*for (std::list<activeStatePath>::iterator it=S->begin(); it != S->end(); ++it) {
      std::cout << "(s=" << getStringFromPat(it->node) << ",n=" << it->nr << ") ";
  }
  std::cout << "\n"; 
  */
  
  std::string* output = new std::string("");
  
  // Reading one character at a time, do update, cleanup and split
  while(input->get(curChar)) {
    for(int i = 0; i < lSize; i++) {
      if(alphabet[i] == curChar) {
        charNr = i;
      }
    }
    update(S, numStates, newM + (charNr*numStates));

    cleanUp(root);
    
    *output += split(&root);
    
    /*
    for (std::list<activeStatePath>::iterator it=S->begin(); it != S->end(); ++it) {
      std::cout << "(s=" << getStringFromPat(it->node) << ",n=" << it->nr << ") ";
    }
    std::cout << "\n";
    */
    i_nr++;
  }

  //Find last part of output, if it does not exist, return "na"
  std::string check = "na";
  for (std::list<activeStatePath>::iterator it=S->begin(); it != S->end(); ++it) {
    if(it->nr == finishingState->nr) {
      patNode* node = it->node;
      check = "";
      while(node != NULL) {
        //std::cout << "Left: " << node->left << " right: " << node->right << std::endl;
        //std::cout << "Parent : " << node->parent << std::endl;
        check = *node->bitstring + check;
        node = node->parent;
      }
    }
  } 
  if(check == "na") {
    *output = "na";
  } else {
    *output += check;
  }
  //std::cout << "Total duration of triple loop " << (double) totalTime / 1000 << std::endl;
  
  //Freeing memory
  //free(S);
  //delete S;
  freeANFA(initialState, numStates);
  freeMatrix(allM, numStates, alphabet.size());
  freePat(root);
  std::cout << "Done\n";
  return output;
}
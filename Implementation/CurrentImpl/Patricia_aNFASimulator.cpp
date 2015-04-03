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

  patNode *next = (*s)[0] == '1' ? node->right : node->left;
  patNode *notNext = (*s)[0] == '0' ? node->right : node->left;
  
  if(next == NULL) {
    next = (patNode*) calloc(1, sizeof(patNode));
    next->bitstring = new std::string(*s);
    next->left = NULL;
    next->right = NULL;
    next->active = 1;
    next->parent = node;
    return next;
  }
  
  int isSame, isPrefix, isPostfix, isMixed;
  
  size_t length = s->length() < next->bitstring->length() ? next->bitstring->length() : s->length();

  size_t i; //length of langest common prefix
  for (i = 0; i < length; i++) {
    if ((*s)[i] != next->bitstring->at(i)) {
      break;
    }
  }
  
  isSame = i == s->length() && s->length() == next->bitstring->length();
  isPostfix = i == next->bitstring->length() && next->bitstring->length() < s->length();
  isPrefix = i == s->length();
  isMixed = !isSame && !isPostfix && !isPrefix;
  
  patNode* next2 = (patNode*) calloc(1, sizeof(patNode));


  //they are identical
  //return child
  if (i == s->length() && s->length() == next->bitstring->length()) {
    next->active = 1;
    return next;
  }

  //child.bitstring is a prefix of s
  //call insert on the child with remainder of s
  if (i == next->bitstring->length() && next->bitstring->length() < s->length()) {
    std::string tmp = s->substr(i);
    return insertPat(next, &tmp);
  }


    next2->bitstring = new std::string(s->substr(0,i));
    next2->left = NULL;
    next2->right = NULL;
    next2->parent = node;
    *(next->bitstring) = next->bitstring->substr(i);
    next->parent = next2;

  //s is a prefix of child.bitstring
  //new node with s as string and remainder og child.bitstring as child
  if (i == s->length()) {
    if (next->bitstring->at(0) == '0') {
      next2->left = next;
    } else {
      next2->right = next;
    }
    next2->active = 1;
    return next2;
  }

  //they differ at some point
  //new node with common prefix as string and remainder of each as children
    patNode* next3 = (patNode*) calloc(1, sizeof(patNode));
    next3->bitstring = new std::string(s->substr(i));
    next3->left = NULL;
    next3->right = NULL;
    next3->parent = next2;

    if (next->bitstring->at(0) == '0') {
      next2->left = next;
      next2->right = next3;
    } else {
      next2->right = next;
      next2->left = next3;
    }
    next3->active = 1;
    return next3;
}

bool compare_PS(const PS& s1, const PS& s2) {
  return *s1.string < *s2.string;
}

void printNewM(std::list<PS>** newM, int numStates, int lSize) {
  for(int i = 0; i < lSize; i++) {
    std::cout << "Letter i = " << i << "\n";
    for(int j = 0; j < numStates; j++) {
      for (std::list<PS>::iterator it=newM[i*numStates + j]->begin(); it != newM[i*numStates + j]->end(); ++it) {
        std::cout << "it->string " << *it->string << ", nr=" << it->nr << ", "; 
      }
    std::cout << "\n";
    }
    std::cout << "\n";
  }
}

void buildNewM(std::string** matrix, std::list<PS>** newM, int numStates, int lSize) {
  for(int i = 0; i < lSize; i++) {
    for(int j = 0; j < numStates; j++) {
    std::list<PS>* current = new std::list<PS>;
    
    for(int k = 0; k < numStates; k++) {
      if(matrix[i*numStates*numStates + j * numStates + k] != NULL) {
        PS* tmp = (PS*) calloc(1, sizeof(PS));
        tmp->string = matrix[i*numStates*numStates + j * numStates + k];
        tmp->nr = k;
        current->push_back(*tmp);
      }
    }
    
    current->sort(compare_PS);
    newM[i*numStates + j] = current;
    
    }
  }
}

std::string getString(patNode* node) {
  if(node->parent == NULL) {
    return "";
  }
  
  return getString(node->parent) + *(node->bitstring);
}

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

void updateS(patNode** S, const int numStates, std::string** m) {
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
           getString(S[j]) + *m[j*numStates+i] < getString(newS[i]) + *ending[i])) {
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

void update(std::list<SS>* S, const int numStates, std::list<PS>** m) {
    std::list<SS> newS; // = new std::list<SS>();
    int *found = calloc(numStates, sizeof(int));
    
    for (std::list<SS>::iterator it=S->begin(); it != S->end(); ++it) {
      it->node->active = 0;
      for(std::list<PS>::iterator pf=m[it->nr]->begin(); pf != m[it->nr]->end(); ++pf) {
        if(!found[pf->nr]) {
            SS* tmp = (SS*) malloc(sizeof(SS));
            tmp->nr = pf->nr;
            tmp->node = insertPat(it->node, pf->string);
            found[pf->nr] = 1;
            newS.push_back(*tmp);
        }
      }
    }
    
    //delete S;
    free(found);
    *S = newS;
}


void freePat(patNode *node) {
  if (node->left != NULL) {
    freePat(node->left);
  }
  if (node->right != NULL) {
    freePat(node->right);
  }
  free(node);
}

bool compare_SS(const SS& s1, const SS& s2) {
  return getString(s1.node) < getString(s2.node);
}

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
  for(size_t i = 0; i < language.size(); i++) {
    buildMatrix(initialState, numStates, language.at(i), allM + (i*numStates*numStates));
  }
  
  //printMatrix(allM, numStates);
  
  std::list<PS>** newM = calloc(numStates*language.size(), sizeof(std::list<PS>*));
  buildNewM(allM, newM, numStates, language.size());
  
  //printNewM(newM, numStates, language.size());

  // Create S for the initial state (using transition matrix epsilon).
  // S[q] is the lexicographically least bitstring,
  //  representing a path through the aNFA to state q, using the input read so far.
  patNode* root = (patNode*) calloc(1, sizeof(patNode));
  root->active = 1;
  root->bitstring = new std::string("");
  
  /*
  patNode** S = (patNode**) malloc(sizeof(patNode*) * numStates);
  for(int i = 0; i < numStates; i++) {
    std::string tmp = createString(initialState, i, '\0');
    S[i] = insertPat(root, &tmp);
  } */
  
  std::list<SS>* S = new std::list<SS>();
  //std::string** tmp = malloc(numStates * sizeof(std::string*));
  for(int i = 0; i < numStates; i++) {
    std::string tmp = createString(initialState, i, '\0');
    //tmp[i] = new std::string();
    //*tmp[i] = createString(initialState, i, '\0');
    if(tmp != "na") {
      SS* tmpSS = malloc(sizeof(SS));
      tmpSS->nr = i;
      tmpSS->node = insertPat(root, &tmp); 
      S->push_back(*tmpSS);
    }
  }
  S->sort(compare_SS);

  // Print simulation arguments
  std::cout << "aNFA simulation:\n";
  std::cout << "regex = " << regEx << "\n";
  //std::cout << "input = " << test_input << "\n";

  /* Test input validity 
  for (size_t i = 0; i < test_input.length(); i++) {
    if (language.find(test_input[i]) == std::string::npos) {
        std::cout << "INVALID INPUT!\n";
        std::cout << "Change the std::string test_input in main().\n";
        std::cout << "Input must be valid for the regular expression.\n";
        std::cout << "Simulation canceled.\n";
        std::string* r = new std::string("Invalid input");
        return r;
    }
  }

  Convert input string to a stream.
  std::istringstream is;
  is.str(test_input);
  */
  
  // Run simulation
  char curChar;
  int lSize = language.size();
  int charNr;// placement in language and allM
  int i_nr = 0;
  /*for (std::list<SS>::iterator it=S->begin(); it != S->end(); ++it) {
      std::cout << "(s=" << getString(it->node) << ",n=" << it->nr << ") ";
  }
  std::cout << "\n"; 
  */
  
  std::string* output = new std::string("");
  
  
  while(input->get(curChar)) {
    for(int i = 0; i < lSize; i++) {
      if(language[i] == curChar) {
        charNr = i;
      }
    }
    std::cout << "Doing update\n";
    update(S, numStates, newM + (charNr*numStates));
    std::cout << "Done update\n";
    root->active = 1;
    cleanUp(root);
    std::cout << "Done cleanup\n";
    *output += split(root);
    std::cout << "Done split\n";
    /*
    for (std::list<SS>::iterator it=S->begin(); it != S->end(); ++it) {
      std::cout << "(s=" << getString(it->node) << ",n=" << it->nr << ") ";
    }
    std::cout << "\n";
    */
    //S->sort(compare_SS);
    /*if(!(i_nr % 500)) {
      //std::cout << "Done " << i_nr << " input chars\n";
    } */
    i_nr++;
  }


  std::string check = "na";
  //*output = getString(S[finishingState->nr]);
  //std::cout << "cstring time: " << cstringTime << " iPatTime: " << insertPatTime << '\n';
  for (std::list<SS>::iterator it=S->begin(); it != S->end(); ++it) {
    if(it->nr == finishingState->nr) {
      //std::cout << "found output\n"; 
      patNode* node = it->node;
      check = "";
      while(node != NULL) {
        check = *node->bitstring + check;
        node = node->parent;
      }
    }
    //SS* tmp = it;
    //free(tmp);
  } 
  //std::string check = getString(S[finishingState->nr]);
  if(check == "na") {
    *output = "na";
  } else {
    *output += check;
  }
  std::cout << "Total duration of triple loop " << (double) totalTime / 1000 << std::endl;
  //free(S);
  //delete S;
  freeANFA(initialState, numStates);
  freeMatrix(allM, numStates, language.size());
  freePat(root);

  return output;
}
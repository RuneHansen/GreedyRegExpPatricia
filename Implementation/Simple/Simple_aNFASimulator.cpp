#include "Simple_aNFASimulator.h"
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include "regex.h"
#include "lex.h"
#include "parser.h"
#include "aNFAgen.cpp"


// ============== This is a simple implementation of aNFA simulation ==================

// S[x] is a pointer to the smallest bitstring representing a path to the state x, using the input read so far.
// L is the language, it is a string with one occurence of each character in the language.
// qMax is total number of states in the aNFA.
// stream is the input stream

// simulate() call update() for each character in the input.
// update() updates the initial state S. Update is defined in Simple_aNFASimulator.h

// In this simple implementation bitstrings are represented by strings containing only ones and zeroes.

//returns the language for a regular expression given its aNFA,
// as a string with one occurence of each character in the language.
std::string findLanguage(aNFAnode* E) {
  std::string language("");
  //get language from sub1
  std::cout << "E " << E << " starter kald af selv\n";
  if (E->left) {
    language = findLanguage(E->left);
    if (E->right) {  //get language from sub2
      std::string temp = findLanguage(E->right);
      for (int i = 0; i < temp.length(); i++) {
        int fail = 0;
        for (int j = 0; j < language.length(); i++) {
          if (temp[i] == language[j]) {
            fail = 1;
            break;
          }
        }
        if(!fail) {
          language += temp[i];
        }
      }
    }
  }
  std::cout << "Kald af selv slut, for " << E << std::endl;

  //add this char
  if(E->input) {
    int fail = 0;
    for (int i = 0; i < language.length(); i++) {
      if (language[i] == E->input) {
        fail = 1;
        break;
      }
    }
    if (!fail) {
      language += E->input;
    }
  }

  //reutrn language
  return language;
}


void printMatrix(std::string** matrix, int sizeQ) {
  if(sizeQ < 20) {
    for(int i = 0; i < sizeQ; i++) {
      for(int j = 0; j < sizeQ; j++) {
        if(*matrix[i*sizeQ + j] == "") {
          std::cout << "e ";
        } else {
          std::cout << *matrix[i*sizeQ + j] + " ";
        }
      }
      std::cout << std::endl;
    }
    return;
  }
  std::cout << "Too big\n";
}

int addNr(aNFAnode* E, int nr) {
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

aNFAnode* findN(aNFAnode* E, int n, int d) {
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

void buildMatrix(aNFAnode* E, int sizeN, char c,  std::string** matrix) {
  std::string** retMat = matrix;

  //std::string* retMat = (std::string*) malloc(sizeof(std::string) * sizeN * sizeN);
  for(int i = 0; i < sizeN*sizeN; i++) {
      retMat[i] = new std::string("na");
  }
  //std::cout << "Har kaldt new\n";
  aNFAnode* tmp;
  for(int i = 0; i < sizeN; i++) {
    std::cout << i << ", " << E->nr << std::endl;
    tmp = findN(E, i, sizeN + 1);

    //std::cout << "Found i " << i << std::endl;
    if(i != tmp->nr) {
      std::cout << "Wrong find " << i << " " << tmp->nr << std::endl;
    }
    //if(i == 3)
      //std::cout << "Find succeed\n";
    for(int j = 0; j < sizeN; j++) {
      *retMat[i*sizeN + j] = createString(tmp, j, c );
    }
    //std::cout << "Done: " << i << std::endl;
    //std::cout << retMat[3*sizeN + 3] << std::endl;

  }
  //return retMat;
}

bool inCharClass(char c, BitC_CharClass n) {
  //std::cout << "Tester inCharClass\n";
  if(n.nranges == 0) {
    return 0;
  }

  if(n.nranges == (size_t) -1) {
    return c != '\0';
  }

  if(c == '\0') {
    return 0;
  }

  //std::cout << "Is charClass\n";

  int success = !n.inclusive;
  for (int i = 0; i < n.nranges; i++) {
    for (int j = (int) n.ranges[i].from; j <=  (int) n.ranges[i].to; j++) {
      if((int) c == j) {
        //std::cout << "Char in charClass, " << success << "\n";
        return !success;
      }
    }
  }
  //std::cout << "Char not in charClass\n";
  return success;
}

std::string createString(aNFAnode* E, int target, char c) {
  //std::cout << "Called createString, target = " << target << " c = " << c <<"\n";

  //Are we done?
  if(!c && E->nr == target) {
    return "";
  }
  int comp = ((!E->input  || E->input == c) &&
                E->charClass.nranges == 0
              || inCharClass(c, E->charClass)) ;
  //std::cout << "Comp done\n";
  //got input but wrong progress
  if(!comp) {
    return "na";
  }
  //We are at end node, not target
  if(E->left == NULL) {
    return "na";
  }
  //We havent read char, check if we read it
  if(c && E->right == NULL) {
    if(E->input == c || inCharClass(c, E->charClass)) {
      //std::cout << "Found correct char\n";
      return createString(E->left, target, '\0');
    }
      return createString(E->left, target, c);
  }
  //A split path, return the best string
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
  //Only one path, no required input
  return createString(E->left, target, c);
}


// Call update() for each character in the input stream.
void simulate(std::string** S, std::string L, std::string** ms, int qMax, std::istream& stream) {
  char input;
  int lSize = L.size();
  int nr;

  while(stream.get(input)) {
    for(int i = 0; i < lSize; i++) {
      if(L[i] == input) {
        nr = i;
      }
    }
    std::cout << "Updating\n";
    update(S, qMax, ms + (nr*qMax*qMax));
  }
}

// Simulates reading a char
// Change S, that is the current set of paths reachable with the imput read so for.
void update(std::string** S, const int Qmax, std::string** m) {

  std::string** newS = (std::string**) malloc(sizeof(std::string) * Qmax);


  for(int i = 0; i < Qmax; i++) {
    std::string* tmp = new std::string();
    newS[i] = tmp;
  }

  for (int i = 0; i < Qmax; i++) {
    (*newS[i]) = "na";
    for (int j = 0; j < Qmax; j++) {
        if (*(S[j]) != "na" && //Is there a path from j to i?
            (*m[j*Qmax+i]) != "na" &&
            ((*newS[i]) == "na" || //Is it the shortest?
             S[j]->size() + m[j*Qmax+i]->size() < newS[i]->size())) {
          (*newS[i]) = *(S[j])+(*m[j*Qmax+i]);
        }
    }
  }
    for (int i = 0; i < Qmax; i++) {
        *(S[i]) = (*newS[i]);
    }
  for(int i = 0; i < Qmax; i++) {
    delete newS[i];
  }
  free(newS);
}

// Find longest prefix common to all strings in the list,
// remove the prefix from all strings in the list and return the prefix.
std::string split(std::string** S, int Qmax) {

  // The index of the characters we are comparing
  int cInd = 0;

  //How many letters we should compare at most, just has to be longer than the shortest string
  int stringLength = S[0]->size();

  //Whether we should break the outer loop.
  int success = 1;

  char tmp = S[0]->at(0);
  for (int c = 0; c < stringLength; c++) { // Each char
    for (int i = 0; i < Qmax; i++) { // in each string
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
  for (int i = 0; i < Qmax; i++){
    S[i]->erase(0, cInd);
  }

  // Return the longest common prefix
  return prefix;
}

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

// Print all possible bitstring-paths with input read so far
void printPaths(std::string** S, int Qmax) {
  for (int i = 0; i < Qmax; i++) {
    if (*(S[i]) != "na") {
      std::cout << "S[" << i << "] = " << *(S[i]) << "\n";
    }
  }
}

std::string* regExComplete(std::string regEx, std::string test_input) {
  std::string** S; // The starting state
  std::string language;
  BitC_Regex *regex = NULL;
  aNFAnode* aNFAfirst = aNFAnodeConstructor();
  aNFAnode* aNFAlast = aNFAnodeConstructor();
  int matrixSize;
  std::string** ma;

  std::cout << "Parser nu\n";


  BitC_YY_scan_string(regEx.c_str());
  BitC_YYparse(&regex);
  BitC_YYlex_destroy();

  //regex = BitC_RegexSimplify(regex);

  std::cout << "Har Parset, generer aNFA\n";

  aNFAgen(regex, aNFAfirst, aNFAlast, &language);

  std::cout << "Har genereret aNFA, tilføjer tal\n";

  matrixSize = addNr(aNFAfirst, 0);

  std::cout << "Har fundet tal, " << matrixSize << " finder sprog\n";
  std::cout << "Har fundet sprog " << language << ", laver plads til ma\n";

  ma = (std::string**) malloc(matrixSize * matrixSize *
                             language.size() * sizeof(std::string*));

  std::cout << "Har lavet plads, bygger matricer\n";
  for(int i = 0; i < language.size(); i++) {
    //ma[i*matrixSize*matrixSize] = buildMatrix(&aNFAfirst, matrixSize, language.at(i));
    buildMatrix(aNFAfirst, matrixSize, language.at(i), ma + (i*matrixSize*matrixSize));
  }

  std::cout << "Matricer bygget, laver S\n";

  S = (std::string**) malloc(sizeof(std::string*) * matrixSize);
  for(int i = 0; i < matrixSize; i++) {
    S[i] = new std::string();
    *S[i] = createString(aNFAfirst, i, '\0');
  }

  std::cout << "S er skabt\n";

  std::cout << "Your matrix\n";
  for(int i = 0; i < language.size(); i++) {
    std::cout << "Letter " << language[i] << std::endl;
    printMatrix(ma + i*matrixSize*matrixSize, matrixSize);
    std::cout << std::endl;
  }

  std::cout << "aNFA simulation:\n";
  std::cout << "regex = " << regEx << "\n";
  std::cout << "input = " << test_input << "\n";

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

  std::cout << "Initial state:\n"; // Before simulation
  printPaths(S, matrixSize);

  // Convert input string to a stream.
  std::istringstream is;
  is.str(test_input);

  // Run simulation
  simulate(S, language, ma, matrixSize, is);

  std::string* retS = new std::string();
  *retS = *S[aNFAlast->nr];





  std::cout << "Freeing Memory\n";


  for(int i = 0; i < matrixSize; i++) {
    delete S[i];
  }
  free(S);
  std::cout << "S freed\n";
  freeANFA(aNFAfirst, matrixSize);
  std::cout << "aNFA freed\n";
  freeMatrix(ma, matrixSize, language.size());
  std::cout << "Matrices freed\n";

  return retS;
}

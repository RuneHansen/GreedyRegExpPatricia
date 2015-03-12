#include "Simple_aNFASimulator.h"
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include "regex.h"
#include "lex.h"
#include "parser.h"


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

aNFAnode* aNFAnodeConstructor() {
  aNFAnode* ret = (aNFAnode*) malloc(sizeof(aNFAnode));
  ret->nr = 0;
  ret->input = '\0';
  ret->charClass.nranges = 0;
  ret->left = NULL;
  ret->right = NULL;
  return ret;
}

void aNFAgen(BitC_Regex* E, aNFAnode* i, aNFAnode* f, std::string* language) {
  if (E) {
    switch(E->op) {
    case BitC_RegexOp_Lit :
      {
      std::cout << "Laver lit\n";
      i->input = E->litChar;
      i->left = f;
      if(language->find(i->input) == std::string::npos) {
        *language += i->input;
      }
      break;
      }
    case BitC_RegexOp_LitString :
      {
      std::cout << "Laver litstring\n";
      aNFAnode* prev = aNFAnodeConstructor();
      i->left = prev;
      prev->left = f;
      prev->input = E->litString[0];
      if(language->find(prev->input) == std::string::npos) {
        *language += prev->input;
      }
      for(int i = 1; E->litString[i] != '\0'; i++) {
        aNFAnode* cur = aNFAnodeConstructor();
        prev->left = cur;
        cur->input = E->litString[i];
        cur->left = f;
        prev = cur;
        if(language->find(prev->input) == std::string::npos) {
          *language += prev->input;
        }
      }
      break;
      }
    case BitC_RegexOp_Plus :
      {
      std::cout << "Laver plus\n";
      aNFAnode* loop = aNFAnodeConstructor();
      aNFAnode* q = aNFAnodeConstructor();
      i->left = q;
      aNFAgen(E->sub1, q, loop, language);
      loop->left = q;
      loop->right = f;
      break;
      }
    case BitC_RegexOp_Star :
      {
      std::cout << "Laver star\n";
      aNFAnode* loop = aNFAnodeConstructor();
      aNFAnode* q = aNFAnodeConstructor();
      i->left = loop;
      loop->left = q;
      loop->right = f;
      aNFAgen(E->sub1,q,loop, language);
      break;
      }
    /*case BitC_RegexOp_Concat2 :
      {
      aNFAnode* q = (aNFAnode*) malloc (sizeof(aNFAnode));
      aNFAgen(E->subs[0],i,q);
      aNFAgen(E->subs[1],q,f);
      break;
      } */
    case BitC_RegexOp_Concat :
    {
      std::cout << "Laver concat\n";
      aNFAnode* prev = aNFAnodeConstructor();
      aNFAgen(E->subs[0],i,prev, language);
      for(int i = 1; i < E->nsub; i++) {
        aNFAnode* cur = aNFAnodeConstructor();
        aNFAgen(E->subs[i],prev,cur, language);
        cur->left = f;
        prev = cur;
      }
      break;
    }
    case BitC_RegexOp_Alt:
      {
      std::cout << "Laver alt\n";
      aNFAnode* s = aNFAnodeConstructor();
      aNFAnode* q = aNFAnodeConstructor();
      aNFAnode* s2;
      i->left = q;
      i->right = s;
      aNFAgen(E->subs[0], q, f, language);

      int n = E->nsub;
      //std::cout << "nsub number " << n << std::endl;
      //std::cout << "Nr's " << s->nr << " " << q->nr << " " << i->nr << " " << f->nr << "\n"; 
      for(int j = 1; j < n; j++) {
        s2 = aNFAnodeConstructor();
        q = aNFAnodeConstructor();
        s->left = q;
        //std::cout << "q's nr " << q->nr << std::endl;
        aNFAgen(E->subs[j], q, f, language);
        //std::cout << "Nr2's" << q->nr << " " << s2->nr << std::endl;
        if(j == n-1) {
          free(s2);
        } else {
          s->right = s2;
          s = s2;
        }
      }
      break;
      }
    case BitC_RegexOp_Any:
      {
      std::cout << "Laver any\n";
      i->charClass.nranges = (size_t) -1;
      i->left = f;
      break;
      }
    case BitC_RegexOp_BeginText:
      std::cout << "Lavede BeginText\n";
      break;
    case BitC_RegexOp_Capture:
      {
      std::cout << "Laver capture\n";

      aNFAgen(E->sub1, i, f, language);
      break;
      }
    case BitC_RegexOp_CharClass:
      {
        std::cout << "Laver charClass\n";
        for(int i = 0; i < E->charClass.nranges; i++) {
          for(int j = (int) E->charClass.ranges[i].from; j <= (int) E->charClass.ranges[i].to; j++) {
            if(language->find( (char) j ) == std::string::npos) {
              *language += (char) j;
            }
          }
        }
        i->charClass.ranges = (BitC_CharRange*) malloc(sizeof(BitC_CharRange) * E->charClass.nranges);
        for(int j = 0; j < E->charClass.nranges; j++) {
          i->charClass.ranges[j].to = E->charClass.ranges[j].to;
          i->charClass.ranges[j].from = E->charClass.ranges[j].from;
        }
        i->charClass.nranges = E->charClass.nranges;
        i->charClass.inclusive = E->charClass.inclusive;
        i->left = f;
        break;
      }
    case BitC_RegexOp_EndText:
      {
      std::cout << "Lavede EndText " << E->nsub << std::endl;
      aNFAgen(E->sub1, i, f, language);
      break;
      }
    case BitC_RegexOp_Question:
      {
      std::cout << "Laver Question\n";
      i->right = f;
      aNFAnode* q = aNFAnodeConstructor();
      i->left = q;
      aNFAgen(E->sub1, q, f, language);
      break;
      }
    case BitC_RegexOp_Repeat:
      std::cout << "Lavede Repeat\n";
      break;
    case BitC_RegexOp_Unit:
      {
      std::cout << "Laver unit\n";
      i->left = f;
      break;
      }
    default:
      {
      std::cout << "Hit default, " << E->op << std::endl;
      }
    }
  }
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

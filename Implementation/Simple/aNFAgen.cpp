#include <string>
#include <list>
#include <iostream>
#include <sstream>

#include <stdlib.h>
#include <stdio.h>

//State node in an aNFA
struct aNFAnode {
  char input = '\0';// Character that must be read to pass on
  BitC_CharClass charClass; // Used in case on charclass
  // Connections to other nodes. Left is allways used in case of only one connection.
  aNFAnode* left = NULL;
  aNFAnode* right = NULL;
  int nr = 0;
};

//Create an empty aNFA node
aNFAnode* aNFAnodeConstructor() {
  aNFAnode* ret = (aNFAnode*) malloc(sizeof(aNFAnode));
  ret->nr = 0;
  ret->input = '\0';
  ret->charClass.nranges = 0;
  ret->left = NULL;
  ret->right = NULL;
  return ret;
}

//Generates the complete aNFA
//E is the regular expression that the aNFA must represent.
//i is the initial state
//f is the finishing state
//language must be set to include exactly one occurense, of each different literal,
    //in the regular expression.
void aNFAgen(BitC_Regex* E, aNFAnode* i, aNFAnode* f, std::string* language) {
  if (E) {
    switch(E->op) {
    case BitC_RegexOp_Lit :
      {
      i->input = E->litChar;
      i->left = f;
      if(language->find(i->input) == std::string::npos) {
        *language += i->input;
      }
      break;
      }
    case BitC_RegexOp_LitString :
      {
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
      aNFAnode* loop = aNFAnodeConstructor();
      aNFAnode* q = aNFAnodeConstructor();
      i->left = loop;
      loop->left = q;
      loop->right = f;
      aNFAgen(E->sub1,q,loop, language);
      break;
      }
    case BitC_RegexOp_Concat :
    {
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
      aNFAnode* s = aNFAnodeConstructor();
      aNFAnode* q = aNFAnodeConstructor();
      aNFAnode* s2;
      i->left = q;
      i->right = s;
      aNFAgen(E->subs[0], q, f, language);

      int n = E->nsub;
      for(int j = 1; j < n; j++) {
        s2 = aNFAnodeConstructor();
        q = aNFAnodeConstructor();
        s->left = q;
        aNFAgen(E->subs[j], q, f, language);
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
      i->charClass.nranges = (size_t) -1;
      i->left = f;
      break;
      }
    case BitC_RegexOp_BeginText:
      break;
    case BitC_RegexOp_Capture:
      {

      aNFAgen(E->sub1, i, f, language);
      break;
      }
    case BitC_RegexOp_CharClass:
      {
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
      aNFAgen(E->sub1, i, f, language);
      break;
      }
    case BitC_RegexOp_Question:
      {
      i->right = f;
      aNFAnode* q = aNFAnodeConstructor();
      i->left = q;
      aNFAgen(E->sub1, q, f, language);
      break;
      }
    case BitC_RegexOp_Repeat:
      break;
    case BitC_RegexOp_Unit:
      {
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

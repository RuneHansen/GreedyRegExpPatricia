#include "regex.h"

//State node in an aNFA
struct aNFAnode {
  char input;// Character that must be read to pass on
  BitC_CharClass charClass; // Used in case on charclass
  // Connections to other nodes. Left is allways used in case of only one connection.
  aNFAnode* left;
  aNFAnode* right;
  aNFAnode* i1;
  aNFAnode* i2;  
  int nr;
};

aNFAnode* aNFAnodeConstructor();

void aNFAgen(BitC_Regex* E, aNFAnode* i, aNFAnode* f, std::string* language);
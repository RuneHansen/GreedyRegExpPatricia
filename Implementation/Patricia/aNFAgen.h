#include "regex.h"

//State node in an aNFA
struct aNFAnode {
  char input = '\0';// Character that must be read to pass on
  BitC_CharClass charClass; // Used in case on charclass
  // Connections to other nodes. Left is allways used in case of only one connection.
  aNFAnode* left = NULL;
  aNFAnode* right = NULL;
  int nr = 0;
};

aNFAnode* aNFAnodeConstructor();

void aNFAgen(BitC_Regex* E, aNFAnode* i, aNFAnode* f, std::string* language);
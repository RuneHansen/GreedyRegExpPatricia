#include <string>
#include "aNFAgen.h"

void printMatrix(std::string** matrix, int numStates);
int addNr(aNFAnode* E, int nr);
aNFAnode* findN(aNFAnode* E, int n, int d);
int inCharClass(char c, BitC_CharClass n);
std::string createString(aNFAnode* E, int target, char c);
void buildMatrix(aNFAnode* E, int numStates, char c,  std::string** matrix);
void freeMatrix(std::string** matrix, int mSize, int lSize);
void freeANFA(aNFAnode* node, int nr);







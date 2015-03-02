#include <string>
#include <list>
#include <iostream>
#include <stdlib.h>
#include <new>

// This is the header for our simple aNFA simulator.

// Q is the set of states {1, 2, ... , Qmax-1, Qmax}
// S[X] is the smallest bitstring representing a path to state X, after the input read so far.
// m[Y][X] is the shortest path from state Y to state X that reads a char
// ms ia an array of m arrays
// stream is the input stream

struct aNFAnode {
  char input;
  aNFAnode* left;
  aNFAnode* right;
};

void aNFAgen(BitC_Regex* E, aNFAnode* i, aNFAnode* f);

// Print all possible bitstring-paths with input read so far
void print(std::string* S, int Qmax);

// Simulates reading a char
// Change S, that is the current set of paths reachable with the imput read so for.
void update(std::string* S, const int Qmax, const std::string* m);

// Find longest prefix common to all strings in the list,
// remove the prefix from all strings in the list and return the prefix.
std::string split(std::string* S, int Qmax);


void simulate(std::string* S, std::string L, std::string* ms, int qMax, std::istream& stream);

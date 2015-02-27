#include <string>
#include <list>
#include <iostream>
#include <stdlib.h>
#include <new>

// This is the header for our simple aNFA simulator.
// It currently holds update(), that does most of the work in the simulaiton process.

// Q is the set of states {1, 2, ... , Qmax-1, Qmax}
// S[X] is the smallest bitstring representing a path to state X, after the input read so far.
// m[Y][X] = Is the shortest path from state Y to state X that reads a char
// "na" = is a path that ends in a dead state.

// Print all possible bitstring-paths with input read so far
void print(std::string* S, int Qmax);

// Simulates reading a char
// Change S, that is the current set of paths reachable with the imput read so for.
void update(std::string* S, const int Qmax, const std::string* m);

// Find longest prefix common to all strings in the list,
// remove the prefix from all strings in the list and return the prefix.
std::string split(std::string* S, int Qmax);


void simulate(std::string* S, std::string L, std::string* ms, int qMax, std::istream& stream);
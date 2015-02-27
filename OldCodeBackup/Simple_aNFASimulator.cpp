#include "Simple_aNFASimulator.h"
#include <string>
#include <list>
#include <iostream>
#include <sstream>

// ============== This is a simple implementation of aNFA simulation ==================
// It is currently hardcoded to the aNFA generated from the regular expression a*.
// You can set the input in Main(). The input must be valid for the regular expression a*.

// Main() just defines the aNFA (a*), as the initial state S, and the relations ma and me. Then it calls simulate()

// simulate call update() for each character in the input.
// update() updates the initial state S. Update is defined in Simple_aNFASimulator.h

// in the simple implementation bitstrings are represented by strings containing only ones and zeroes.

// S[x] is a pointer to the smallest bitstring representing a path to the state x, using the input read so far.
// L is the language, it is a string with one occurence of each character in the language.
// qMax is total number of states in the aNFA.
// stream is the input stream

// Call update() for each character in the input stream.
void simulate(std::string* S, std::string L, std::string* ms, int qMax, std::istream& stream) {
  char input;
  int lSize = L.size();
  int nr;

  while(stream.get(input)) {
    for(int i = 0; i < lSize; i++) {
      if(L[i] == input) {
        nr = i;
      }
    }
    update(S, qMax, ms + nr*qMax*qMax);
  }
}

// Define aNFA (a*) and run simulation
int main() {
  std::string S[5]; // The starting state
  S[0] = "";
  S[1] = "";
  S[2] = "0";
  S[3] = "na";
  S[4] = "1";

  // The relational matrix for reading an "a"
  std::string ma[25] = {"na", "0", "00", "0", "01",
                          "na", "0", "00", "0", "01",
                          "na", "", "0", "", "1",
                          "na", "0", "00", "0", "01",
                          "na", "na", "na", "na", "na"};

// ===================== THIS IS WHERE YOU SET THE IMPUT ======================
  std::string test_input = std::string("aaa");
// ============================================================================

  std::cout << "aNFA simulation:\n";
  std::cout << "input = " << test_input << "\n";

// test input validity
  for (int i = 0; i < test_input.length(); i++) {
    if (test_input[i] != 'a') {
        std::cout << "INVALID INPUT!\n";
        std::cout << "Change the std::string test_input in main().\n";
        std::cout << "Input must be valid for the regular expression a*.\n";
        std::cout << "Simulation canceled.\n";
        return 0;
    }
  }

  std::cout << "Initial state:\n"; // before simulation
  print(S, 5);

  // convert input string to a stream
  std::istringstream is;
  is.str(test_input);

  // run simulation
  simulate(S, "a", ma, 5, is);
  std::cout << "Finishing state:\n"; // after simulation
  print(S, 5);

// Make sure the window does not close automatically.
  std::cout << "\nEnter one character to finish the program.\n";
  char a;
  std::cin >> a;
	return 0;
}

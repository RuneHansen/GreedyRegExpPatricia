#include "Simple_aNFASimulator.h"
#include <string>
#include <list>
#include <iostream>
#include <sstream>

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

// ===================== THIS IS WHERE YOU SET THE INPUT ======================
  std::string test_input = std::string("aaa");
// ============================================================================

  std::cout << "aNFA simulation:\n";
  std::cout << "input = " << test_input << "\n";

  // Test input validity
  for (int i = 0; i < test_input.length(); i++) {
    if (test_input[i] != 'a') {
        std::cout << "INVALID INPUT!\n";
        std::cout << "Change the std::string test_input in main().\n";
        std::cout << "Input must be valid for the regular expression a*.\n";
        std::cout << "Simulation canceled.\n";
        return 0;
    }
  }

  std::cout << "Initial state:\n"; // Before simulation
  print(S, 5);

  // Convert input string to a stream.
  std::istringstream is;
  is.str(test_input);

  // Run simulation
  simulate(S, "a", ma, 5, is);
  std::cout << "Finishing state:\n"; // After simulation
  print(S, 5);

  // Make sure the window does not close automatically.
  std::cout << "\nEnter one character to finish the program.\n";
  char a;
  std::cin >> a;
	return 0;
}

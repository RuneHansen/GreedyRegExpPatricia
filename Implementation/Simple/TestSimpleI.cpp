#include "Simple_aNFASimulator.h"
#include <string>
#include <list>
#include <iostream>
#include <sstream>


// Define aNFA (a*) and run simulation
int main() {
  std::string* S; // The starting state
  string language;
  BitC_Regex *regex = NULL;
  aNFAnode aNFAfirst;
  aNFAnode aNFAlast;
  int matrixSize;
  int terminal = 0;
  char regEx[100];
  std::string test_input;
  std::string* ma;
  if(terminal) {
    std::cout << "Type your regex:\n";
    std::cin.getline(regEx, 100);
    std::cout << "Type your input string:\n";
    std::cin.getline(test_input, 100);

    BitC_YY_scan_string(regEx);
    BitC_YYparse(&regex);
    BitC_YYlex_destroy();
    aNFAgen(regex, &aNFAfirst, &aNFAlast);
    matrixSize = addNr(&aNFAfirst, 0);
    matrixSize--;

    language = findLanguage(aNFAfirst);
    ma = (std::string*) malloc(matrixSize * matrixSize *
                               language.size() * sizeof(string*));
    for(int i = 0; i < language.size(); i++) {
      ma[i*matrixSize*matrixSize] = buildMatrix(&aNFAfirst, matrixSize, language[i]);
    }

    std::cout << "Your matrix\n";
    printMatrix(ma, matrixSize);

  } else {
    // The relational matrix for reading an "a"
    ma = (string*) malloc(sizeof(string*) * 25);
    ma = {"na", "0", "00", "0", "01",
                          "na", "0", "00", "0", "01",
                          "na", "", "0", "", "1",
                          "na", "0", "00", "0", "01",
                          "na", "na", "na", "na", "na"};

  // ===================== THIS IS WHERE YOU SET THE INPUT ======================
    test_input = std::string("aaa");
  // ============================================================================
    S[0] = "";
    S[1] = "";
    S[2] = "0";
    S[3] = "na";
    S[4] = "1";
  }


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
  print(S, matrixSize);

  // Convert input string to a stream.
  std::istringstream is;
  is.str(test_input);

  // Run simulation
  simulate(S, language, ma, matrixSize, is);
  std::cout << "Finishing state:\n"; // After simulation
  print(S, matrixSize);

  // Make sure the window does not close automatically.
  std::cout << "\nEnter one character to finish the program.\n";
  char a;
  std::cin >> a;
	return 0;
}

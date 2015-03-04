#include "Simple_aNFASimulator.h"
#include <string>
#include <list>
#include <iostream>
#include <sstream>

#include <stdlib.h>
#include <stdio.h>

#include "regex.h"
#include "lex.h"
#include "parser.h"


// Define aNFA (a*) and run simulation
int main() {
  std::string* S; // The starting state
  std::string language;
  BitC_Regex *regex = NULL;
  aNFAnode aNFAfirst;
  aNFAnode aNFAlast;
  int matrixSize;
  int terminal = 1;
  char regEx[100];
  std::string test_input;
  std::string* ma;
  if(terminal) {
    std::cout << "Type your regex:\n";
    std::cin.getline(regEx, 100);
    std::cout << "Type your input string:\n";
    char tmpI[100];
    std::cin.getline(tmpI, 100);
    test_input = std::string(tmpI);

    BitC_YY_scan_string(regEx);
    BitC_YYparse(&regex);
    BitC_YYlex_destroy();
    aNFAgen(regex, &aNFAfirst, &aNFAlast);
    matrixSize = addNr(&aNFAfirst, 0) - 1;

    language = findLanguage(&aNFAfirst);
    ma = (std::string*) malloc(matrixSize * matrixSize *
                               language.size() * sizeof(std::string*));
    for(int i = 0; i < language.size(); i++) {
      //ma[i*matrixSize*matrixSize] = buildMatrix(&aNFAfirst, matrixSize, language.at(i));
      buildMatrix(&aNFAfirst, matrixSize, language.at(i), ma + (i*matrixSize*matrixSize));
    }
    S = (std::string*) malloc(sizeof(std::string*) * matrixSize);
    for(int i = 0; i < matrixSize; i++) {
      //std::string* tmp = new std::string();
      //tmp = &(createString(&aNFAnode, i, '\0'));
      S[i] = createString(&aNFAfirst, i, '\0');
    }

    std::cout << "Your matrix\n";
    printMatrix(ma, matrixSize);

  } else { /*
    // The relational matrix for reading an "a"
    matrixSize = 5;
    ma = (string*) malloc(sizeof(string*) * 25);
    ma = {"na", "0", "00", "0", "01",
                          "na", "0", "00", "0", "01",
                          "na", "", "0", "", "1",
                          "na", "0", "00", "0", "01",
                          "na", "na", "na", "na", "na"};

  // ===================== THIS IS WHERE YOU SET THE INPUT ======================
    test_input = std::string("aaa");
  // ============================================================================
    S = (string*) malloc(sizeof(string) * 5);
    S[0] = "";
    S[1] = "";
    S[2] = "0";
    S[3] = "na";
    S[4] = "1"; */
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
  printPaths(S, matrixSize);

  // Convert input string to a stream.
  std::istringstream is;
  is.str(test_input);

  // Run simulation
  simulate(S, language, ma, matrixSize, is);
  std::cout << "Finishing state:\n"; // After simulation
  printPaths(S, matrixSize);

  // Make sure the window does not close automatically.
  std::cout << "\nEnter one character to finish the program.\n";
  char a;
  std::cin >> a;
	return 0;
}

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

std::string* regExComplete(std::string regEx, std::string test_input) {
  std::string** S; // The starting state
  std::string language;
  BitC_Regex *regex = NULL;
  aNFAnode* aNFAfirst = (aNFAnode*) malloc(sizeof(aNFAnode));
  aNFAnode* aNFAlast = (aNFAnode*) malloc(sizeof(aNFAnode));
  int matrixSize;
  std::string** ma;

  std::cout << "Parser nu\n";
  
  
  BitC_YY_scan_string(regEx.c_str());
  BitC_YYparse(&regex);
  BitC_YYlex_destroy();
  
  //regex = BitC_RegexSimplify(regex);
  
  std::cout << "Har Parset, generer aNFA\n";
  
  aNFAgen(regex, aNFAfirst, aNFAlast, &language);
  
  std::cout << "Har genereret aNFA, tilføjer tal\n";
  
  matrixSize = addNr(aNFAfirst, 0);

  std::cout << "Har fundet tal, " << matrixSize << " finder sprog\n";
  std::cout << "Har fundet sprog " << language << ", laver plads til ma\n";
  
  ma = (std::string**) malloc(matrixSize * matrixSize *
                             language.size() * sizeof(std::string*));
                             
  std::cout << "Har lavet plads, bygger matricer\n";
  for(int i = 0; i < language.size(); i++) {
    //ma[i*matrixSize*matrixSize] = buildMatrix(&aNFAfirst, matrixSize, language.at(i));
    buildMatrix(aNFAfirst, matrixSize, language.at(i), ma + (i*matrixSize*matrixSize));
  }
  
  std::cout << "Matricer bygget, laver S\n";
  
  S = (std::string**) malloc(sizeof(std::string*) * matrixSize);
  for(int i = 0; i < matrixSize; i++) {
    S[i] = new std::string();
    *S[i] = createString(aNFAfirst, i, '\0');
  }
    
  std::cout << "S er skabt\n";

  std::cout << "Your matrix\n";
  for(int i = 0; i < language.size(); i++) {
    std::cout << "Letter " << language[i] << std::endl;
    printMatrix(ma + i*matrixSize*matrixSize, matrixSize);
    std::cout << std::endl;
  }

  std::cout << "aNFA simulation:\n";
  std::cout << "input = " << test_input << "\n";
  
  // Test input validity
  for (int i = 0; i < test_input.length(); i++) {
    if (language.find(test_input[i]) == std::string::npos) {
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

  return S[aNFAlast->nr];
}

int main() {
  std::string* result;
  char input[100];
  char regEx[100];
  std::cout << "Regex: \n";
  std::cin.getline(regEx, 100);
  std::cout << "Input:\n";
  std::cin.getline(input, 100);

  result = regExComplete(regEx, input);
  
  std::cout << "Result: " << *result << std::endl;

  return 0;
}


// Define aNFA (a*) and run simulation
/*int main() {
  std::string** S; // The starting state
  std::string language;
  BitC_Regex *regex = NULL;
  aNFAnode* aNFAfirst = (aNFAnode*) malloc(sizeof(aNFAnode));
  aNFAnode* aNFAlast = (aNFAnode*) malloc(sizeof(aNFAnode));
  int matrixSize;
  int terminal = 1;
  char regEx[100];
  std::string test_input;
  std::string** ma;
  if(terminal) {
    std::cout << "Type your regex:\n";
    std::cin.getline(regEx, 100);
    std::cout << "Type your input string:\n";
    char tmpI[100];
    std::cin.getline(tmpI, 100);
    test_input = std::string(tmpI);

    std::cout << "Parser nu\n";
    
    BitC_YY_scan_string(regEx);
    BitC_YYparse(&regex);
    BitC_YYlex_destroy();
    
    //regex = BitC_RegexSimplify(regex);
    
    std::cout << "aNFAlast ptrs = " << aNFAlast->left << " " << aNFAlast->right << "\n";
    
    std::cout << "Har Parset, generer aNFA\n";
    
    aNFAgen(regex, aNFAfirst, aNFAlast, &language);
    
    std::cout << "Har genereret aNFA, tilføjer tal\n";
    
    matrixSize = addNr(aNFAfirst, 0);

    std::cout << "Har fundet tal, " << matrixSize << " finder sprog\n";
    
    //language = findLanguage(aNFAfirst);
    
    std::cout << "Har fundet sprog " << language << ", laver plads til ma\n";
    
    ma = (std::string**) malloc(matrixSize * matrixSize *
                               language.size() * sizeof(std::string*));
                               
    std::cout << "Har lavet plads, bygger matricer\n";
    for(int i = 0; i < language.size(); i++) {
      //ma[i*matrixSize*matrixSize] = buildMatrix(&aNFAfirst, matrixSize, language.at(i));
      buildMatrix(aNFAfirst, matrixSize, language.at(i), ma + (i*matrixSize*matrixSize));
    }
    
    std::cout << "Matricer bygget, laver S\n";
    
    S = (std::string**) malloc(sizeof(std::string*) * matrixSize);
    for(int i = 0; i < matrixSize; i++) {
      S[i] = new std::string();
      *S[i] = createString(aNFAfirst, i, '\0');
    }
    
    std::cout << "S er skabt\n";

    std::cout << "Your matrix\n";
    for(int i = 0; i < language.size(); i++) {
      std::cout << "Letter " << language[i] << std::endl;
      printMatrix(ma + i*matrixSize*matrixSize, matrixSize);
      std::cout << std::endl;
    }
  } else { /*
    // The relational matrix for reading an "a"
    matrixSize = 5;
    ma = (string*) malloc(sizeof(string*) * 25);
    ma =                 {"na", "0", "00", "0", "01",
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
    S[4] = "1"; 
  } */

  /*
  std::cout << "aNFA simulation:\n";
  std::cout << "input = " << test_input << "\n";
  
  // Test input validity
  for (int i = 0; i < test_input.length(); i++) {
    if (language.find(test_input[i]) == std::string::npos) {
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
  std::cout << "Last state: " << aNFAlast->nr << std::endl;
  printPaths(S, matrixSize);
  

  // Make sure the window does not close automatically.
  std::cout << "\nEnter one character to finish the program.\n";
  char a;
  std::cin >> a; 
	return 0;
} */

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

  std::string* retS = new std::string();
  *retS = *S[aNFAlast->nr];





  std::cout << "Freeing Memory\n";


  for(int i = 0; i < matrixSize; i++) {
    delete S[i];
  }
  free(S);
  std::cout << "S freed\n";
  freeANFA(aNFAfirst, matrixSize);
  std::cout << "aNFA freed\n";
  freeMatrix(ma, matrixSize, language.size());
  std::cout << "Matrices freed\n";

  return retS;
}

testCase* fillTC(std::string r, std::string i, std::string o);

void moreTests() {
  std::list <testCase> cases;

  cases.append(fillTC("a.c", "abc", ""));
  cases.append(fillTC("a.c", "axb", ""));
  cases.append(fillTC("a.*", "axyzb", "0001"));
  cases.append(fillTC("a[bc]d", "abd", ""));
  cases.append(fillTC("a[b-d]e", "ace", ""));
  cases.append(fillTC("a[b-d]", "ac", ""));
  cases.append(fillTC("a[\]]b", "a]b", ""));
  cases.append(fillTC("a[^bc]d", "aed", ""));
  cases.append(fillTC("a[-b]c", "adc", ""));
  cases.append(fillTC("ab|cd", "ab", "0"));
  cases.append(fillTC("ab|cd", "cd", "1"));
  cases.append(fillTC("((a))", "a", ""));
  cases.append(fillTC("(a)b(c)", "abc", ""));
  cases.append(fillTC("a+b+c", "aabbc", "0101"));
  cases.append(fillTC("(a+|b)*", "ab", "001011"));
  cases.append(fillTC("(a+|b)+", "ab", "001"));
  cases.append(fillTC("(a+|b)?", "a", "00"));
  cases.append(fillTC("[^ab]*", "cde", "0001"));
  cases.append(fillTC("a*", "", "1"));
  cases.append(fillTC("a|b|c|d|e", "e", "1111"));
  cases.append(fillTC("(a|b|c|d|e)f", "ef", "1111"));
  cases.append(fillTC("abcd*efg", "abcdefg", "1"));
  cases.append(fillTC("ab*", "abbb", "0001"));
  cases.append(fillTC("(ab|cd)e", "cde", "1"));
  cases.append(fillTC("[abhgefdc]ij", "hij", ""));
  cases.append(fillTC("(a|b)c*d", "bcd", "101"));
  cases.append(fillTC("(ab|ab*)bc", "abc", "11"));
  cases.append(fillTC("a([bc]*)c*", "abc", "0011"));
  cases.append(fillTC("a([bc]*)(c*d)", "abcd", "0011"));
  cases.append(fillTC("a([bc]*)(c+d)", "abcd", "011"));
  cases.append(fillTC("a[bcd]*dcdcde", "adcdcde", "1"));
  cases.append(fillTC("(ab|a)b*c", "abc", "01"));
  cases.append(fillTC("((a)(b)c)(d)", "abcd", ""));
  cases.append(fillTC("[a-zA-Z_][a-zA-Z0-9_]*", "alpha", "00001"));
  cases.append(fillTC("a(bc+|b[eh])g|.h", "abhbh", "11"));

  /*
  cases.append(fillTC("(bc+d$|ef*g.|h?i(j|k))", "effgz", ""));
  cases.append(fillTC("a.c", "axb", ""));
  cases.append(fillTC("a.c", "axb", ""));
  cases.append(fillTC("a.c", "axb", ""));
  cases.append(fillTC("a.c", "axb", ""));
  cases.append(fillTC("a.c", "axb", ""));

    ('(bc+d$|ef*g.|h?i(j|k))', 'effgz', SUCCEED, 'found+"-"+g1+"-"+g2', 'effgz-effgz-None'),
    ('(bc+d$|ef*g.|h?i(j|k))', 'ij', SUCCEED, 'found+"-"+g1+"-"+g2', 'ij-ij-j'),
    ('(bc+d$|ef*g.|h?i(j|k))', 'effg', FAIL),
    ('(bc+d$|ef*g.|h?i(j|k))', 'bcdd', FAIL),
    ('(bc+d$|ef*g.|h?i(j|k))', 'reffgz', SUCCEED, 'found+"-"+g1+"-"+g2', 'effgz-effgz-None'),
    ('(((((((((a)))))))))', 'a', SUCCEED, 'found', 'a'),
    ('multiple words of text', 'uh-uh', FAIL),
    ('multiple words', 'multiple words, yeah', SUCCEED, 'found', 'multiple words'),

*/
}


int newTest() {
  std::cout << "RNG test " << (char) 122 << "\n";

  int numTests = 10; //<<<<<===================================== HUSK MIG!!!!!!!!!!!!!!!!!!!!!!!!!
  std::string** regexs = malloc(numTests * sizeof(std::string*));
  std::string** inputs = malloc(numTests * sizeof(std::string*));
  std::string** bitstrings = malloc(numTests * sizeof(std::string*));

  regexs[0] = new std::string("abc");
  inputs[0] = new std::string("abc");
  bitstrings[0] = new std::string("");

  regexs[1] = new std::string("ab*c");
  inputs[1] = new std::string("abc");
  bitstrings[1] = new std::string("01");

  regexs[2] = new std::string("ab*bc");
  inputs[2] = new std::string("abc");
  bitstrings[2] = new std::string("1");

  regexs[3] = new std::string("ab*bc");
  inputs[3] = new std::string("abbc");
  bitstrings[3] = new std::string("01");

  regexs[4] = new std::string("ab*bc");
  inputs[4] = new std::string("abbbbc");
  bitstrings[4] = new std::string("0001");

  regexs[5] = new std::string("ab+bc");
  inputs[5] = new std::string("abbc");
  bitstrings[5] = new std::string("1");

  regexs[6] = new std::string("ab+bc");
  inputs[6] = new std::string("abbbbc");
  bitstrings[6] = new std::string("001");

  regexs[7] = new std::string("ab?bc");
  inputs[7] = new std::string("abbc");
  bitstrings[7] = new std::string("0");

  regexs[8] = new std::string("ab?bc");
  inputs[8] = new std::string("abc");
  bitstrings[8] = new std::string("1");

  regexs[9] = new std::string("ab?c");
  inputs[9] = new std::string("abc");
  bitstrings[9] = new std::string("0");

  for (int i = 0; i < numTests; i++) {
    if (bitstring[i] != *regExComplete(*regexs[i],*inputs[i])) {
      std::cout << "FAILED! At test number " << i
       << ", returned " << *regExComplete(*regexs[i],*inputs[i])
        << ", when " << *bitstring[i] << " was expected.\n";
        return 0;
    }
  }
  std::cout << "SUCCESS! All tests completed."
  return 0;
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

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



struct testCase {
  std::string regex;
  std::string input;
  std::string bitstring;
};

testCase fillTC(std::string r, std::string i, std::string b) {
  //testCase* ret = (testCase*) malloc(sizeof(testCase));
  testCase ret;
  ret.regex = r;
  ret.input = i;
  ret.bitstring = b;
  return ret;
}

void newTest() {
  std::cout << "RNG test " << (char) 122 << "\n";

  std::list<testCase> cases;

  cases.push_back(fillTC("ac.*bc", "acbc", "1"));
  cases.push_back(fillTC("ac.*bc", "acbcbc", "001"));
  cases.push_back(fillTC("abafd", "abafd", ""));
  cases.push_back(fillTC("a*", "aaaa", "00001"));
  cases.push_back(fillTC("(ab)|(ab)", "ab", "0"));
  cases.push_back(fillTC("(aab)?ab", "ab", "1"));
  cases.push_back(fillTC("(aab)?ab", "aabab", "0"));
  cases.push_back(fillTC("(aab)|ab", "ab", "1"));
  cases.push_back(fillTC("(((((((((a)))))))))", "a", ""));
  //cases.push_back(fillTC("ab((.*), (.*))", "ab(a, b)", "00"));
  //cases.push_back(fillTC("\t\n\v\r\f\a\g", "\t\n\v\r\f\ag", ""));
  cases.push_back(fillTC("([a-c]*)?", "acc", "00001"));
  cases.push_back(fillTC("([a-c]*)?", "a", "001"));


  cases.push_back(fillTC("ab.c", "abbc", ""));
  cases.push_back(fillTC("ax.c", "axxc", ""));
  cases.push_back(fillTC("ab[x-z].*", "abxxyzb", "00001"));
  cases.push_back(fillTC("a[bc]d", "abd", ""));
  cases.push_back(fillTC("a[b-d]e", "ace", ""));
  cases.push_back(fillTC("a[b-d]", "ac", ""));
  cases.push_back(fillTC("a[\\]]b", "a]b", ""));
  cases.push_back(fillTC("a[^bc]de", "aede", ""));
  cases.push_back(fillTC("a[^-b]cd", "adcd", ""));
  cases.push_back(fillTC("ab|cd", "ab", "0"));
  cases.push_back(fillTC("ab|cd", "cd", "1"));
  cases.push_back(fillTC("((a))", "a", ""));
  cases.push_back(fillTC("(a)b(c)", "abc", ""));
  cases.push_back(fillTC("a+b+c", "aabbc", "0101"));
  cases.push_back(fillTC("(a+|b)*", "ab", "001011"));
  cases.push_back(fillTC("(a+|b)+", "ab", "01011"));
  cases.push_back(fillTC("(a+|b)?", "a", "001"));
  cases.push_back(fillTC("cde[^ab]*", "cdecde", "0001"));
  cases.push_back(fillTC("a*", "", "1"));
  cases.push_back(fillTC("a|b|c|d|e", "e", "1"));
  cases.push_back(fillTC("a|b|c|d|e", "c", "001"));
  cases.push_back(fillTC("(a|b|c|d|e)f", "ef", "1"));
  cases.push_back(fillTC("abcd*efg", "abcdefg", "01"));
  cases.push_back(fillTC("ab*", "abbb", "0001"));
  cases.push_back(fillTC("(ab|cd)e", "cde", "1"));
  cases.push_back(fillTC("[abhgefdc]ij", "hij", ""));
  cases.push_back(fillTC("(a|b)c*d", "bcd", "101"));
  cases.push_back(fillTC("(ab|ab*)bc", "abc", "11"));
  cases.push_back(fillTC("a([bc]*)c*", "abc", "0011"));
  cases.push_back(fillTC("a([bc]*)(c*d)", "abcd", "0011"));
  cases.push_back(fillTC("a([bc]*)(c+d)", "abcd", "011"));
  cases.push_back(fillTC("a[bcd]*dcdcde", "adcdcde", "1"));
  cases.push_back(fillTC("(ab|a)b*c", "abc", "01"));
  cases.push_back(fillTC("((a)(b)c)(d)", "abcd", ""));
  cases.push_back(fillTC("[a-zA-Z_][a-zA-Z0-9_]*", "alpha", "00001"));
  cases.push_back(fillTC("a(bc+|b[e-h])g|.h", "bh", "1"));


  for (std::list<testCase>::iterator it=cases.begin(); it != cases.end(); ++it) {

    std::string tmp = *regExComplete(it->regex, it->input);
    if (it->bitstring != tmp) {
      std::cout << "FAILED! At (regex, input): "
       << "(" << it->regex
       << ", " << it->input << ")"
       << ", returned " << tmp
       << ", when " << it->bitstring << " was expected.\n";
      return;
    }
  }
  std::cout << "SUCCESS! All tests completed.\n";
  return;
}


int main() {
  char c[10];
  std::cout << "Testset (t) or input (i): ";
  std::cin.getline( c, 10);

  if(c[0] == 'i') {
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
  } else {
    newTest();
  }
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

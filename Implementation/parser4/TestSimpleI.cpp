#include "Simple_aNFASimulator.h"
#include "Patricia_aNFASimulator.h"
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>

#include <stdlib.h>
#include <stdio.h>

#include "regex.h"
#include "lex.h"
#include "parser.h"

//=========================== SIMPLE TEST ===============================
// Runs a simulation based on user input, or runs the test suite (newTest()).

// Regular expression, input string and expected output bitstring, for one test simulation.
struct testCase {
  std::string regex;
  std::string input;
  std::string bitstring;
};

// Constructor for testCase
testCase fillTC(std::string r, std::string i, std::string b) {
  testCase ret;
  ret.regex = r;
  ret.input = i;
  ret.bitstring = b;
  return ret;
}

// A test, consisting of a list og testCases
void newTest(char c) {
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

  //Run each test case
  for (std::list<testCase>::iterator it=cases.begin(); it != cases.end(); ++it) {
    std::istringstream is;
    is.str(it->input);
    std::string tmp;
    if(c == 's')
      tmp = *s_simulate(it->regex, &is);
    else
      tmp = *p_simulate(it->regex, &is);
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

void genFile(std::string language, size_t size) {
  std::ofstream myfile;
  myfile.open("test_input");
  for(size_t i = 0; i < size; i++) {
    myfile << language[i % language.size()];
  }
  myfile.close();
}


// Takes arguments from user, or runs the test suite
int main() {
  char c[10];
  std::cout << "Testset (t), performance(p) or input (i): ";
  std::cin.getline( c, 10);

  if(c[0] == 'i') {
    std::string* result;
    char input[100];
    char regEx[100];
    std::cout << "Regex: \n";
    std::cin.getline(regEx, 100);
    std::cout << "Input:\n";
    std::cin.getline(input, 100);
    
    std::istringstream is;
    is.str(input);
    if(c[1] == 's')
      result = s_simulate(regEx, &is);
    else
      result = p_simulate(regEx, &is);

    std::cout << "Result: " << *result << std::endl;
    return 0;
  } else if (c[0] == 'p') {
    if(c[1] == 'g') {
      std::cout << "Generating test data\n";
      genFile("abcdefghijklmnopqrstuvwxyz", 3000);
    }
    std::ifstream infile("test_input");
    if(infile.is_open()) {
      clock_t time = clock();
      std::string* result;
      if(c[2] == 's')
        result = s_simulate("([a-z]([a-c]+|[^x-z])?)*", &infile);
      else 
        result = p_simulate("([a-z]([a-c]+|[^x-z])?)*", &infile);
      time = clock() - time;
      
      std::cout << "Result: " << *result << std::endl;
      std::cout << "Time: " <<  ((double) time / CLOCKS_PER_SEC) << std::endl;
      infile.close();
    }

  } else {
    newTest(c[1]);
  }
  return 0;
}

#include "Simple_aNFASimulator.h"
#include "Patricia_aNFASimulator.h"
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>
#include <sys/time.h>


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
  std::cout << "Unittest(u), performance(p), input (i) or quit(q): ";
  std::cin.getline( c, 10);

  while(c[0] != 'q') {
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
      struct timeval time;
      gettimeofday(&time, NULL);
      long timeP = time.tv_sec * 1000 + time.tv_usec / 1000;

      if(c[1] == 's') {
        result = s_simulate(regEx, &is);
      } else {
        result = p_simulate(regEx, &is);
      }
      gettimeofday(&time, NULL);
      timeP = (time.tv_sec * 1000 + time.tv_usec / 1000) - timeP;

      
      std::cout << "Result: " << *result << ", time: " << ((double) timeP / 1000) << std::endl;
    } else if (c[0] == 'p') {
      std::cout << "Input size: 3k(1), 30k(2), 300k(3)\n";
      std::cin.getline(c, 10);
      if(c[0] == '1') {
        std::cout << "Generating test data\n";
        genFile("abcdefghijklmnopqrstuvwxyz", 3000);
      }
      if(c[0] == '2') {
        std::cout << "Generating test data\n";
        genFile("abcdefghijklmnopqrstuvwxyz", 30000);
      }
      if(c[0] == '3') {
        std::cout << "Generating test data\n";
        genFile("abcdefghijklmnopqrstuvwxyz", 3000000);
      }
      std::cout << "Compare implementations? y/n\n";
      std::cin.getline(c, 10);
      

      
      std::ifstream infile2;
      std::string* result;
      long timeS = 0;
      long timeP = 0;
      struct timeval time;
        
      std::ifstream infile("test_input");
      if(infile.is_open()) {
        gettimeofday(&time, NULL);
        timeP = time.tv_sec * 1000 + time.tv_usec / 1000;
        
        result = p_simulate("([a-z]([a-c]+|[^x-z])?)*", &infile);
        
        gettimeofday(&time, NULL);
        timeP = (time.tv_sec * 1000 + time.tv_usec / 1000) - timeP;
        
        infile.close();
      }
      if(c[0] == 'y' || c[0] == 'Y') {
        std::ifstream infile("test_input");
          if(infile.is_open()) {
            gettimeofday(&time, NULL);
            timeS = time.tv_sec * 1000 + time.tv_usec / 1000;
          
            result = s_simulate("([a-z]([a-c]+|[^x-z])?)*", &infile);
            
            gettimeofday(&time, NULL);
            timeS = (time.tv_sec * 1000 + time.tv_usec / 1000) - timeS;
            infile.close();
          }
        }
        
        //std::cout << "Result: " << *result << std::endl;
        std::cout << "Time for patricia: " <<  timeP << "ms\n";
        if(timeS != 0) {
          std::cout << "Time for simple: " <<  timeS << "ms\n";          
        }
    } else {
      newTest(c[1]);
    }
    std::cout << "unit(u), performance(p), input (i) or quit(q): ";
    std::cin.getline( c, 10);
  }
  
  return 0;
}

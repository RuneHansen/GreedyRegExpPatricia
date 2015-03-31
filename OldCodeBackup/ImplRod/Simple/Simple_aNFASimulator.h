#include <string>
#include <list>
#include <iostream>
#include <stdlib.h>
#include <new>
#include "../regex.h"


// This is the header for our simple aNFA simulator.

// regEx is the regular expression, test_input is the string we read
std::string* s_simulate(std::string regEx, std::istream* input);

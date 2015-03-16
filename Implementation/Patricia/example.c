#include <stdlib.h>
#include <stdio.h>

#include "regex.h"
#include "lex.h"
#include "parser.h"

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("Usage: %s <regex>\n", argv[0]);
    exit(1);
  }

  BitC_Regex *regex = NULL;
  BitC_YY_scan_string(argv[1]);
  BitC_YYparse(&regex);
  BitC_YYlex_destroy();

  if (regex != NULL)
  {
    regex = BitC_RegexSimplify(regex);
    char str[1024];
    BitC_PrintRegex(regex, str, sizeof(str));
    printf("Parsed this: %s\n", str);
    BitC_FreeRegex(regex);
  }
  else
    return 1;
}

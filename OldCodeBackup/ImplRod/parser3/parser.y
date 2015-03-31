%{
  #include <stdio.h>
  #include <stdlib.h>

  #include "regex.h"
  #include "lex.h"

  int BitC_YYerror (BitC_Regex **regex, char const *s)
  {
    fprintf (stderr, "Parse error: %s\n", s);
    return 1;
  }

  typedef struct RangeList
  {
    struct BitC_CharRange value;
    struct RangeList *next;
  } RangeList;

  static size_t ListToArray(RangeList *list, struct BitC_CharRange **out)
  {
    size_t count = 0;

    RangeList *current;
    for (current = list; current; current = current->next)
      count++;

    if (count > 0)
    {
      int i = 0;
      *out = (BitC_CharRange*) malloc(sizeof(BitC_CharRange) * count);
      current = list;
      RangeList *tmp;
      do
      {
        (*out)[i++] = current->value;
        tmp = current;
        current = current->next;
        free(tmp);
      } while (current);
    }
    return count;
  }
%}

%code provides {
extern int BitC_YYparse(BitC_Regex **regex);
extern int BitC_YYerror(BitC_Regex **regex, char const *s);
}

%output "parser.c"
%defines "parser.h"

%parse-param { BitC_Regex **regex }

%union {
  int value;
  char *digit_string;
  BitC_Regex *regex;
  struct RangeList *range_list;
  struct BitC_CharRange range;
  struct { UInt32 min; SInt32 max; } repeat_range;
}

%token UNKNOWN
%token CLASS_NEG
%token CLASS_POS
%token LPAREN_NONCAP
%token REPEAT_START;
%token TOK_WILDCARD;

%token POSIX_BEGIN_POS
%token POSIX_BEGIN_NEG
%token POSIX_END
%token POSIX_CHARCLASS

%token <value> CHAR
%token <digit_string> DIGITS

%type <regex> regex1;
%type <regex> regex2;
%type <regex> regex3;
%type <regex> regex4;
%type <range_list> range;
%type <range> range1;
%type <repeat_range> repeat;

%destructor { BitC_FreeRegex($$); } <regex>

%%

input:
regex1  { *regex = $1; }
;

regex1:
  regex1 '|' regex2 { $$ = BitC_MkRegex_Alt2($1, $3); }
| regex2            { $$ = $1; }
| /*empty*/         { $$ = BitC_MkRegex_Unit(); }
;

regex2:
  regex3 regex2 { $$ = BitC_MkRegex_Concat2($1, $2); }
| regex3
;

regex3:
  regex3 '*'                      { $$ = BitC_MkRegex_Star($1); }
| regex3 '+'                      { $$ = BitC_MkRegex_Plus($1); }
| regex3 '?'                      { $$ = BitC_MkRegex_Question($1); }
| regex3 REPEAT_START repeat '}'  { $$ = BitC_MkRegex_Repeat($1, $3.min, $3.max); }
| regex4
;

regex4:
  '(' regex1 ')'                 { $$ = BitC_MkRegex_Capture($2); }
| LPAREN_NONCAP regex1 ')'       { $$ = $2; }
| CLASS_POS range ']'            { struct BitC_CharRange *range = NULL;
                                   size_t n = ListToArray($2, &range);
                                   $$ = BitC_MkRegex_CharClass(
                                          BitC_MkCharClass(range, n, true));
                                 }
| CLASS_NEG range ']'            { struct BitC_CharRange *range = NULL;
                                   size_t n = ListToArray($2, &range);
                                   $$ = BitC_MkRegex_CharClass(
                                          BitC_MkCharClass(range, n, false)); }
| CHAR                           { $$ = BitC_MkRegex_Lit($1); }
| TOK_WILDCARD                   { $$ = BitC_MkRegex_Any(); }
;

range:
  range1 range                   { $$ = (RangeList *) malloc(sizeof(RangeList));
                                   $$->value = $1; $$->next = $2; }
| range1                         { $$ = (RangeList *) malloc(sizeof(RangeList));
                                   $$->value = $1; $$->next = NULL; }
;

range1:
  CHAR '-' CHAR                  { $$ = BitC_MkCharRange($1, $3); }
| CHAR                           { $$ = BitC_MkCharRange($1, $1); }
| '-'                            { $$ = BitC_MkCharRange('-', '-'); }
  // TODO: Handle posix character classes
| POSIX_BEGIN_POS POSIX_CHARCLASS POSIX_END
               { yyerror(NULL, "POSIX classes unsupported"); }
| POSIX_BEGIN_NEG POSIX_CHARCLASS POSIX_END
               { yyerror(NULL, "POSIX classes unsupported"); }
;

repeat:
  DIGITS ',' DIGITS { $$.min = atoi($1); $$.max = atoi($3); }
| DIGITS ','        { $$.min = atoi($1); $$.max = -1; }
| DIGITS            { $$.min = atoi($1); $$.max = atoi($1); }
;

%%

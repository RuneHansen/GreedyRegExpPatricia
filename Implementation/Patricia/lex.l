%{

#include <stdio.h>
#include "regex.h"
#include "parser.h"

#define yylval BitC_YYlval
%}

%option noyywrap
%option nounput
%option noinput
%option prefix="BitC_YY"
%option outfile="lex.c" header-file="lex.h"

%s SC_CLS
%s SC_POSIX
%s REPEAT

re_meta    [?$|()*+^]

%%

<INITIAL>{
     {re_meta}    { return yytext[0]; }
     "."          { return TOK_WILDCARD; }
     "(?:"        { return LPAREN_NONCAP; }
     "["          { BEGIN(SC_CLS); return CLASS_POS; }
     "[^"         { BEGIN(SC_CLS); return CLASS_NEG; }
     "{"          { BEGIN(REPEAT); return REPEAT_START; }
}
<REPEAT>{
     "}"         { BEGIN(INITIAL); return yytext[0]; }
     [0-9]+      { yylval.digit_string = yytext; return DIGITS; }
     ","         { return yytext[0]; }
     .           { return UNKNOWN; }
}
<SC_CLS,SC_POSIX>{
     "]"         { BEGIN(INITIAL); return yytext[0]; }
     -/[^]]      { return yytext[0]; }
     "[:"        { BEGIN(SC_POSIX); return POSIX_BEGIN_POS; }
     "[^:"       { BEGIN(SC_POSIX); return POSIX_BEGIN_NEG; }
     ":]"        { BEGIN(SC_CLS); return POSIX_END; }
}
<INITIAL,SC_CLS>{
     "\\".        { yylval.value = yytext[1];
                    return CHAR; }
     .            { yylval.value = yytext[0];
                    return CHAR; }
}
<SC_POSIX>{
  "alnum"  { yylval.value = 0;  return POSIX_CHARCLASS; }
  "alpha"  { yylval.value = 1;  return POSIX_CHARCLASS; }
  "ascii"  { yylval.value = 2;  return POSIX_CHARCLASS; }
  "blank"  { yylval.value = 3;  return POSIX_CHARCLASS; }
  "cntrl"  { yylval.value = 4;  return POSIX_CHARCLASS; }
  "digit"  { yylval.value = 5;  return POSIX_CHARCLASS; }
  "graph"  { yylval.value = 6;  return POSIX_CHARCLASS; }
  "lower"  { yylval.value = 7;  return POSIX_CHARCLASS; }
  "print"  { yylval.value = 8;  return POSIX_CHARCLASS; }
  "punct"  { yylval.value = 9;  return POSIX_CHARCLASS; }
  "space"  { yylval.value = 10; return POSIX_CHARCLASS; }
  "upper"  { yylval.value = 11; return POSIX_CHARCLASS; }
  "word"   { yylval.value = 12; return POSIX_CHARCLASS; }
  "xdigit" { yylval.value = 13; return POSIX_CHARCLASS; }
  .        { return UNKNOWN; }
}

%%

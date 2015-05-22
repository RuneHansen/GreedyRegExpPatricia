/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_BITC_YY_PARSER_H_INCLUDED
# define YY_BITC_YY_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int BitC_YYdebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    UNKNOWN = 258,
    CLASS_NEG = 259,
    CLASS_POS = 260,
    LPAREN_NONCAP = 261,
    REPEAT_START = 262,
    TOK_WILDCARD = 263,
    POSIX_BEGIN_POS = 264,
    POSIX_BEGIN_NEG = 265,
    POSIX_END = 266,
    POSIX_CHARCLASS = 267,
    CHAR = 268,
    DIGITS = 269
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 56 "parser.y" /* yacc.c:1909  */

  int value;
  char *digit_string;
  BitC_Regex *regex;
  struct RangeList *range_list;
  struct BitC_CharRange range;
  struct { UInt32 min; SInt32 max; } repeat_range;

#line 78 "parser.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE BitC_YYlval;

int BitC_YYparse (BitC_Regex **regex);
/* "%code provides" blocks.  */
#line 46 "parser.y" /* yacc.c:1909  */

extern int BitC_YYparse(BitC_Regex **regex);
extern int BitC_YYerror(BitC_Regex **regex, char const *s);

#line 94 "parser.h" /* yacc.c:1909  */

#endif /* !YY_BITC_YY_PARSER_H_INCLUDED  */

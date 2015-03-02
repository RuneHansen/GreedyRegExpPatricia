/**
 * @file regex.h
 * Data structures for representing the syntax of regular expressions,
 * as well as methods for creating syntax trees.
 */

#ifndef BITC_REGEX_H
#define BITC_REGEX_H

#include "std.h"
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Constructors for regular expressions.
 */
typedef enum BitC_RegexOp
{
  BitC_RegexOp_Unit,       /*!< unit (parses the empty string) */
  BitC_RegexOp_Lit,       /*!< (simple) e.g. 'a' */
  BitC_RegexOp_LitString, /*!< (simple) e.g. 'abc' */
  BitC_RegexOp_Star,      /*!< (simple) e.g. 'a*' */
  BitC_RegexOp_Plus,      /*!< e.g. 'a+' */
  BitC_RegexOp_Question,  /*!< (simple) e.g. 'a?' */
  BitC_RegexOp_Capture,   /*!< (simple) e.g. '(ab)' */
  BitC_RegexOp_Repeat,    /*!< e.g. '{1,2}' */
  BitC_RegexOp_Alt,       /*!< (simple) n-ary alternation */
  BitC_RegexOp_Concat,    /*!< (simple) n-ary concatenation */
  BitC_RegexOp_CharClass, /*!< (simple) e.g. '[a-z]' */
  BitC_RegexOp_Any,       /*!< (simple) e.g '.' */
  BitC_RegexOp_BeginText, /*!< (simple) e.g. '^' */
  BitC_RegexOp_EndText    /*!< (simple) e.g. '$' */
} BitC_RegexOp;

/**
 * A single character range.
 */
typedef struct BitC_CharRange
{
  char from;
  char to;
} BitC_CharRange;

/**
 * Representation of character classes.
 */
typedef struct BitC_CharClass
{
  bool inclusive;
  size_t nranges;
  BitC_CharRange *ranges;
} BitC_CharClass;

/**
 * Representation of regular expressions.
 */
typedef struct BitC_Regex
{
  BitC_RegexOp op; /*!< Constructor. */
  // Operator arguments
  union
  {
    BitC_CharClass charClass;
    struct
    {
      UInt32 repeatMin;
      SInt32 repeatMax; // -1 means no upper bound
    };
    char *litString;
    char litChar;
  };
  size_t nsub;
  union
  {
    struct BitC_Regex **subs; // nsub > 1
    struct BitC_Regex *sub1;  // nsub == 1
  };
} BitC_Regex;


/* Note: In all constructor methods, ownership of input pointers are
 * transferred to the newly created expression object.
 * Duplicate pointers are allowed. */

/**
 * Recusively free the memory occupied by the given regular
 * expression.
 *
 * @param regexp Expression tree to free.
 */
extern void BitC_FreeRegex(BitC_Regex *regexp);

/**
 * Translate all subexpressions using one of the following operators:
 *   - ::BitC_RegexOp_Plus
 *   - ::BitC_RegexOp_Repeat
 *   - ::BitC_RegexOp_CharClass
 *   - ::BitC_RegexOp_Question
 *
 * The resulting expression will be equivalent to the original, but
 * will only use the following operators:
 *   - ::BitC_RegexOp_Unit
 *   - ::BitC_RegexOp_Lit
 *   - ::BitC_RegexOp_LitString
 *   - ::BitC_RegexOp_Star
 *   - ::BitC_RegexOp_Capture
 *   - ::BitC_RegexOp_Alt
 *   - ::BitC_RegexOp_Concat
 *   - ::BitC_RegexOp_CharClass
 *     - Restricted to only positive character classes containing
 *       exactly one range.
 *   - ::BitC_RegexOp_Any
 *   - ::BitC_RegexOp_BeginText
 *   - ::BitC_RegexOp_EndText
 * @param regex Input regular expression. The pointer will be invalid
 * upon return.
 * @return Pointer to simplified expression.
 */
extern BitC_Regex *BitC_RegexSimplify(BitC_Regex *regex);


extern BitC_Regex *BitC_MkRegex_Unit();
/**
 * Create a new regular expression literal.
 * @param c Literal character.
 */
extern BitC_Regex *BitC_MkRegex_Lit(char c);
extern BitC_Regex *BitC_MkRegex_LitString(const char *str, bool isStatic);
extern BitC_Regex *BitC_MkRegex_Star(BitC_Regex *sub);
extern BitC_Regex *BitC_MkRegex_Plus(BitC_Regex *sub);
extern BitC_Regex *BitC_MkRegex_Question(BitC_Regex *sub);
extern BitC_Regex *BitC_MkRegex_Capture(BitC_Regex *sub);
extern BitC_Regex *BitC_MkRegex_Repeat(BitC_Regex *sub, UInt32 min, SInt32 max);
extern BitC_Regex *BitC_MkRegex_Alt(BitC_Regex **sub, size_t nsub);
extern BitC_Regex *BitC_MkRegex_Concat(BitC_Regex **sub, size_t nsub);
extern BitC_Regex *BitC_MkRegex_Alt2(BitC_Regex *sub1, BitC_Regex *sub2);
extern BitC_Regex *BitC_MkRegex_Concat2(BitC_Regex *sub1, BitC_Regex *sub2);
extern BitC_Regex *BitC_MkRegex_CharClass(BitC_CharClass class);
extern BitC_Regex *BitC_MkRegex_Any();
extern BitC_Regex *BitC_MkRegex_BeginText();
extern BitC_Regex *BitC_MkRegex_EndText();

extern BitC_CharClass BitC_MkCharClass(BitC_CharRange *ranges, size_t nranges, bool inclusive);
extern BitC_CharRange BitC_MkCharRange(char from, char to);

/**
 * Very simple printing function. Does not take operator precedence
 * into account, but defaults to adding non-capturing groups to ensure
 * atomicity of everything.
 *
 * @param re Regex to show.
 * @param toStr Destination string.
 * @param n Size of destination string, including the null terminator.
 */
extern int BitC_PrintRegex(BitC_Regex *re, char *tostr, size_t n);

#endif // BITC_REGEX_H

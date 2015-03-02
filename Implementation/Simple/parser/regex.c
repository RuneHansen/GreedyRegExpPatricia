#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "regex.h"

static struct BitC_Regex *
allocRegex(BitC_RegexOp op)
{
  struct BitC_Regex *regex = malloc(sizeof(struct BitC_Regex));
  memset(regex, 0, sizeof(struct BitC_Regex));
  regex->op = op;
  return regex;
}

extern void BitC_FreeRegex(BitC_Regex *regex)
{
  switch(regex->op)
  {
  case BitC_RegexOp_Unit:
  case BitC_RegexOp_Any:
  case BitC_RegexOp_BeginText:
  case BitC_RegexOp_EndText:
  case BitC_RegexOp_Lit:
    break;
  case BitC_RegexOp_LitString:
    free(regex->litString);
    break;
  case BitC_RegexOp_Alt:
  case BitC_RegexOp_Concat:
    for (int i = 0; i < regex->nsub; i++)
      BitC_FreeRegex(regex->subs[i]);
    free(regex->subs);
    break;
  case BitC_RegexOp_Star:
  case BitC_RegexOp_Plus:
  case BitC_RegexOp_Question:
  case BitC_RegexOp_Capture:
  case BitC_RegexOp_Repeat:
    if (regex->nsub > 0)
      BitC_FreeRegex(regex->sub1);
    break;
  case BitC_RegexOp_CharClass:
    free((struct BitC_CharRange*)regex->charClass.ranges);
  }
  free((struct BitC_Regex*)regex);
}

BitC_Regex *BitC_MkRegex_Unit()
{
  return allocRegex(BitC_RegexOp_Unit);
}

BitC_Regex *
BitC_MkRegex_Lit(char c)
{
  struct BitC_Regex *regex = allocRegex(BitC_RegexOp_Lit);
  regex->litChar = c;
  return regex;
}

BitC_Regex *
BitC_MkRegex_StringLit(char *str, bool isStatic)
{
  struct BitC_Regex *regex = allocRegex(BitC_RegexOp_LitString);
  // Copy static strings to heap.
  // Not strictly necessary, but simplifies freeing, plus ensures
  // immutability.
  if (isStatic)
  {
    int n = strlen(str);
    char *hStr = malloc(n+1);
    strcpy(hStr, str);
    str = hStr;
  }
  regex->litString = str;
  return regex;
}

BitC_Regex *
BitC_MkRegex_Capture(BitC_Regex *sub1)
{
  struct BitC_Regex *regex = allocRegex(BitC_RegexOp_Capture);
  regex->nsub = 1;
  regex->sub1 = sub1;
  return regex;
}

BitC_Regex *
BitC_MkRegex_Any()
{
  return allocRegex(BitC_RegexOp_Any);
}

BitC_Regex *
BitC_MkRegex_Alt(BitC_Regex **subs, size_t nsub)
{
  struct BitC_Regex *regex = allocRegex(BitC_RegexOp_Alt);
  regex->nsub = nsub;
  regex->subs = subs;
  return regex;
}

BitC_Regex *
BitC_MkRegex_Star(BitC_Regex *sub)
{
  struct BitC_Regex *regex = allocRegex(BitC_RegexOp_Star);
  regex->nsub = 1;
  regex->sub1 = sub;
  return regex;
}

BitC_Regex *
BitC_MkRegex_Plus(BitC_Regex *sub)
{
  struct BitC_Regex *regex = allocRegex(BitC_RegexOp_Plus);
  regex->nsub = 1;
  regex->sub1 = sub;
  return regex;
}

BitC_Regex *
BitC_MkRegex_Question(BitC_Regex *sub)
{
  struct BitC_Regex *regex = allocRegex(BitC_RegexOp_Question);
  regex->nsub = 1;
  regex->sub1 = sub;
  return regex;
}

BitC_Regex *
BitC_MkRegex_Repeat(BitC_Regex *sub, UInt32 min, SInt32 max)
{
  if (max < min)
  {
    FATAL("Invalid range.\n");
    exit(EXIT_FAILURE);
  }

  struct BitC_Regex *regex = allocRegex(BitC_RegexOp_Repeat);
  regex->nsub = 1;
  regex->sub1 = sub;
  regex->repeatMin = min;
  regex->repeatMax = max;
  return regex;
}

BitC_Regex *
BitC_MkRegex_Concat(BitC_Regex **subs, size_t nsub)
{
  struct BitC_Regex *regex = allocRegex(BitC_RegexOp_Concat);
  regex->nsub = nsub;
  regex->subs = subs;
  return regex;
}

BitC_Regex *
BitC_MkRegex_Alt2(BitC_Regex *re1, BitC_Regex *re2)
{
  BitC_Regex **subs = malloc(sizeof(BitC_Regex*[2]));
  subs[0] = re1;
  subs[1] = re2;
  return BitC_MkRegex_Alt(subs, 2);
}

BitC_Regex *
BitC_MkRegex_Concat2(BitC_Regex *re1, BitC_Regex *re2)
{
  BitC_Regex **subs = malloc(sizeof(BitC_Regex*[2]));
  subs[0] = re1;
  subs[1] = re2;
  return BitC_MkRegex_Concat(subs, 2);
}

BitC_Regex *
BitC_MkRegex_CharClass(BitC_CharClass class)
{
  struct BitC_Regex *regex = allocRegex(BitC_RegexOp_CharClass);
  regex->charClass = class;
  return regex;
}

BitC_CharClass
BitC_MkCharClass(BitC_CharRange *ranges, size_t nranges, bool inclusive)
{
  struct BitC_CharClass charClass;
  charClass.inclusive = inclusive;
  charClass.nranges = nranges;
  charClass.ranges = ranges;
  return charClass;
}

BitC_CharRange
BitC_MkCharRange(char from, char to)
{
  BitC_CharRange range = { from, to };
  return range;
}

BitC_Regex *BitC_RegexCopy(BitC_Regex *regex)
{
  struct BitC_Regex *copy = malloc(sizeof(BitC_Regex));
  memcpy(copy, regex, sizeof(BitC_Regex));
  switch (regex->op)
  {
  case BitC_RegexOp_LitString:
    copy->litString = malloc(strlen(regex->litString));
    strcpy(copy->litString, regex->litString);
    break;
  case BitC_RegexOp_CharClass:
    copy->charClass.ranges = malloc(sizeof(BitC_CharRange)*regex->charClass.nranges);
    memcpy(copy->charClass.ranges,
           regex->charClass.ranges,
           sizeof(BitC_CharRange)*regex->charClass.nranges);
    break;
  default:
    if (regex->nsub > 0)
    {
      if (regex->nsub == 1)
        regex->sub1 = BitC_RegexCopy(regex->sub1);
      else
      {
        copy->subs = malloc(sizeof(BitC_Regex*)*regex->nsub);
        for (int i = 0; i < regex->nsub; i++)
          copy->subs[i] = BitC_RegexCopy(regex->subs[i]);
      }
    }
  }

  return copy;
}

BitC_Regex *BitC_RegexSimplify(BitC_Regex *regex)
{
  switch (regex->op)
  {
  case BitC_RegexOp_Plus:
  {
    BitC_Regex *sub = BitC_RegexSimplify(regex->sub1);
    BitC_Regex *subCopy = BitC_RegexCopy(sub);
    regex->sub1 = NULL;
    regex->nsub = 0;
    BitC_FreeRegex(regex);
    return BitC_MkRegex_Concat2(sub, BitC_MkRegex_Star(subCopy));
  }
  case BitC_RegexOp_Question:
  {
    BitC_Regex *sub = BitC_RegexSimplify(regex->sub1);
    regex->sub1 = NULL;
    regex->nsub = 0;
    BitC_FreeRegex(regex);
    return BitC_MkRegex_Alt2(sub, BitC_MkRegex_Unit());
  }
  case BitC_RegexOp_Repeat:
  {
    BitC_Regex *sub = BitC_RegexSimplify(regex->sub1);

    UInt32 repeatMin = regex->repeatMin;
    SInt32 repeatMax = regex->repeatMax;
    regex->sub1 = NULL;
    regex->nsub = 0;
    BitC_FreeRegex(regex);

    if (repeatMin == 0 && repeatMax == 0)
      return sub;

    BitC_Regex *left = NULL;
    if (repeatMin > 0)
    {
      if (repeatMin == 1)
        left = sub;
      else
      {
        BitC_Regex **subs = malloc(sizeof(BitC_Regex*[repeatMin]));
        for (int i = 0; i < repeatMin - 1; i++)
          subs[i] = BitC_RegexCopy(sub);
        subs[repeatMin-1] = sub;
        left = BitC_MkRegex_Concat(subs, repeatMin);
      }
    }
    BitC_Regex *right = NULL;
    if (repeatMax < 0)
    {
      right = BitC_MkRegex_Star(left ? BitC_RegexCopy(sub) : sub);
    }
    else if (repeatMax - repeatMin > 0)
    {
      BitC_Regex **subs = malloc(sizeof(BitC_Regex*[repeatMax-repeatMin]));
      for (int i = 0; i < repeatMax - repeatMin - 1; i++)
        subs[i] = BitC_MkRegex_Alt2(BitC_RegexCopy(sub), BitC_MkRegex_Unit());
      subs[repeatMax - repeatMin - 1] =
        BitC_MkRegex_Alt2(left ? BitC_RegexCopy(sub) : sub, BitC_MkRegex_Unit());
      right = BitC_MkRegex_Concat(subs, repeatMax - repeatMin);
    }

    if (left && right)
      return BitC_MkRegex_Concat2(left, right);
    else if (left)
      return left;
    else
      return right;
  default:
    if (regex->nsub == 1)
      regex->sub1 = BitC_RegexSimplify(regex->sub1);
    else
      for (int i = 0; i < regex->nsub; i++)
        regex->subs[i] = BitC_RegexSimplify(regex->subs[i]);
    return regex;
  }
  }
}

int BitC_PrintRegex(BitC_Regex *re, char *toStr, size_t n)
{
  #define OUT(f, x) i += snprintf(toStr+i, n-i, f, x); n-= i;
  #define OUTS(s) OUT("%s", s);

  int i = 0;
  if (re)
  {
    switch (re->op)
    {
    case BitC_RegexOp_Unit:
      OUTS("\\1");
      break;
    case BitC_RegexOp_Any:
      OUTS(".");
      break;
    case BitC_RegexOp_BeginText:
      OUTS("^");
      break;
    case BitC_RegexOp_EndText:
      OUTS("$");
      break;
    case BitC_RegexOp_Lit:
      OUT("%c", re->litChar);
      break;
    case BitC_RegexOp_LitString:
      OUTS(re->litString);
      break;
    case BitC_RegexOp_Star:
      OUTS("(?:");
      i+=BitC_PrintRegex(re->sub1, toStr+i, n);
      OUTS(")*");
      break;
    case BitC_RegexOp_Plus:
      OUTS("(?:");
      i+=BitC_PrintRegex(re->sub1, toStr+i, n);
      OUTS(")+");
      break;
    case BitC_RegexOp_Concat:
      for (int j = 0; j < re->nsub; j++)
        i+=BitC_PrintRegex(re->subs[j], toStr+i, n);
      break;
    case BitC_RegexOp_Alt:
      OUTS("(?:");
      for (int j = 0; j < re->nsub; j++)
      {
        i+=BitC_PrintRegex(re->subs[j], toStr+i, n);
        if (j < re->nsub-1)
          OUTS("|");
      }
      OUTS(")");
      break;
    case BitC_RegexOp_Question:
      i+=BitC_PrintRegex(re->sub1, toStr+i, n);
      OUTS("?");
      break;
    case BitC_RegexOp_Capture:
      OUTS("(");
      i+=BitC_PrintRegex(re->sub1, toStr+i, n);
      OUTS(")");
      break;
    case BitC_RegexOp_Repeat:
      i+=BitC_PrintRegex(re->sub1, toStr+i, n);
      OUT("{%d", re->repeatMin);
      if (re->repeatMin == re->repeatMax)
        { OUTS("}"); }
      else if (re->repeatMax == -1)
        { OUTS(",}"); }
      else
        { OUT(",%d}", re->repeatMax); }
      break;
    case BitC_RegexOp_CharClass:
      OUTS("[");
      if (!re->charClass.inclusive)
        OUTS("^");
      for (int j = 0; j < re->charClass.nranges; j++)
      {
        BitC_CharRange rng = re->charClass.ranges[j];
        OUT("%c", rng.from);
        if (rng.from != rng.to)
          OUT("-%c", rng.to);
      }
      OUTS("]");
      break;
    }
  }

  return i;
  #undef OUT
  #undef OUTS
}

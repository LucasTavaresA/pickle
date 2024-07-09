#ifndef CHAR_C
#define CHAR_C

static inline int isalpha(int c)
{
  return ((unsigned)c | 32) - 'a' < 26;
}

static inline int isdigit(int c)
{
  return (unsigned)c - '0' < 10;
}

static inline int isalnum(int c)
{
  return isalpha(c) || isdigit(c);
}

static inline int isupper(int c)
{
  return (unsigned)c - 'A' < 26;
}

static inline int tolower(int c)
{
  if (isupper(c))
    return c | 32;
  return c;
}

static inline int isspace(int c)
{
	return c == ' ' || (unsigned)c-'\t' < 5;
}

#endif  // CHAR_C

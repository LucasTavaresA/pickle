#ifndef CHAR_C
#define CHAR_C

static int isalpha(int c)
{
  return ((unsigned)c | 32) - 'a' < 26;
}

static int isdigit(int c)
{
  return (unsigned)c - '0' < 10;
}

static int isalnum(int c)
{
  return isalpha(c) || isdigit(c);
}

static int isupper(int c)
{
  return (unsigned)c - 'A' < 26;
}

static int tolower(int c)
{
  if (isupper(c))
    return c | 32;
  return c;
}

#endif  // CHAR_C

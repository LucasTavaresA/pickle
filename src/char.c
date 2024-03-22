int isalpha(int c)
{
  return ((unsigned)c | 32) - 'a' < 26;
}

int isdigit(int c)
{
  return (unsigned)c - '0' < 10;
}

int isalnum(int c)
{
  return isalpha(c) || isdigit(c);
}

int isupper(int c)
{
  return (unsigned)c - 'A' < 26;
}

int tolower(int c)
{
  if (isupper(c))
    return c | 32;
  return c;
}
#ifndef PICKLE_LOG
#define PICKLE_LOG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONTRAST_LIMIT 85

#ifdef RELEASE
#define LogIfTrue(...)
#define LogIfBadContrast(...)
#define LogSet(...)
#define LogDraw(...)
#define LogAppend(...)
#else
static char LogMessage[10240] = "";

#define LogAppend(formatStr, ...) \
  sprintf(LogMessage + strlen(LogMessage), formatStr, ##__VA_ARGS__);

#define LogIfTrue(result, formatStr, ...)                               \
  if (result)                                                           \
  {                                                                     \
    sprintf(LogMessage + strlen(LogMessage), formatStr, ##__VA_ARGS__); \
  }

#define LogIfBadContrast(backgroundColor, textColor, formatStr, ...)    \
  int rDiff = abs(backgroundColor.r - textColor.r);                     \
  int gDiff = abs(backgroundColor.g - textColor.g);                     \
  int bDiff = abs(backgroundColor.b - textColor.b);                     \
                                                                        \
  if ((rDiff + gDiff + bDiff) / 3 < CONTRAST_LIMIT)                     \
  {                                                                     \
    sprintf(LogMessage + strlen(LogMessage), formatStr, ##__VA_ARGS__); \
  }

#define LogDraw() DrawText(LogMessage, 0, 0, 20, BLUE)
#define LogSet(message)                             \
  strncpy(LogMessage, message, sizeof(LogMessage)); \
  LogMessage[sizeof(LogMessage) - 1] = '\0';
#endif

#endif  // PICKLE_LOG

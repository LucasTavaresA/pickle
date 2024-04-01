#ifndef LOG_C
#define LOG_C

#include <stdio.h>
#include <string.h>

#ifdef RELEASE
#define LogAppend(...)
#define LogIf(...)
#define LogCheckBadContrast(...)
#define LogDraw(...)
#define LogSet(...)
#else
static char LogMessage[10240] = "";

#define LogAppend(formatStr, ...) \
  sprintf(LogMessage + strlen(LogMessage), formatStr, ##__VA_ARGS__);

#define LogIf(condition, code) \
  if (condition)               \
  {                            \
    code                       \
  }

#define LogDraw() DrawText(LogMessage, 0, 0, 20, BLUE)

#define LogSet(message)                             \
  strncpy(LogMessage, message, sizeof(LogMessage)); \
  LogMessage[sizeof(LogMessage) - 1] = '\0';
#endif  // RELEASE

#endif  // LOG_C

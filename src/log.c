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
#ifndef LOG_MESSAGE_LENGTH
#define LOG_MESSAGE_LENGTH 8000
#endif

static char LogMessage[LOG_MESSAGE_LENGTH] = {0};

#define LogAppend(formatStr, ...) \
  sprintf(LogMessage + strlen(LogMessage), formatStr, ##__VA_ARGS__);

#define LogIf(condition, code) \
  if (condition)               \
  {                            \
    code                       \
  }

#ifndef LOG_FONT_SIZE
#define LOG_FONT_SIZE 20
#endif

#define LogDraw() DrawText(LogMessage, 0, 0, LOG_FONT_SIZE, BLUE)

#define LogSet(message)                             \
  strncpy(LogMessage, message, LOG_MESSAGE_LENGTH); \
  LogMessage[sizeof(LogMessage) - 1] = '\0';
#endif  // RELEASE

#endif  // LOG_C

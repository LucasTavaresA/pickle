#ifndef PICKLE_GLOBALS
#define PICKLE_GLOBALS

#include "../raylib/src/raylib.h"

#define ARRAY_LENGTH(arr) sizeof(arr) / sizeof(arr[0])
#define APP_NAME "pickle"
#define TEXT_SPACING 2
#define ROUNDNESS 0.2f
#define INITIAL_REPEAT_INTERVAL 0.3f  // Initial repeat interval in seconds
#define MIN_REPEAT_INTERVAL 0.03f     // Minimum repeat interval in seconds
#define FOREGROUND_COLOR BLACK
#define BACKGROUND_COLOR WHITE
#define PALETTE_ROW_AMOUNT 2
#define PALETTE_COL_AMOUNT (COLORS_AMOUNT / PALETTE_ROW_AMOUNT)
#define PALETTE_ROW_PERCENTAGE 100 / (PALETTE_ROW_AMOUNT)
#define PALETTE_COL_PERCENTAGE 100 / (PALETTE_COL_AMOUNT)
#define HIGHLIGHT_COLOR \
  (Color)               \
  {                     \
    180, 180, 180, 255  \
  }
#define RED_HIGHLIGHT_COLOR \
  (Color)                   \
  {                         \
    255, 122, 122, 255      \
  }

typedef struct
{
  char* Name;
  int Color;
} Slice;

static int ScreenWidth = 0;
static int ScreenHeight = 0;
static int ScreenPadding = 0;
static int Padding = 0;
static int MouseX = 0;
static int MouseY = 0;
static int MousePressedX = 0;
static int MousePressedY = 0;
static int TouchCount = 0;
static Font Fonte;
static int FontSize = 0;
static bool MenuOpened = false;
static bool ButtonWasPressed = false;
static float ButtonPressedTime = 0;
static bool Clicked = false;
static int TypingIndex = -1;
static float KeyRepeatInterval = INITIAL_REPEAT_INTERVAL;

// LucasTA: This is the coolest shit ever
#define COLOR_LIST \
  X(LIGHTGRAY)     \
  X(GRAY)          \
  X(DARKGRAY)      \
  X(YELLOW)        \
  X(GOLD)          \
  X(ORANGE)        \
  X(PINK)          \
  X(RED)           \
  X(MAROON)        \
  X(GREEN)         \
  X(LIME)          \
  X(DARKGREEN)     \
  X(SKYBLUE)       \
  X(BLUE)          \
  X(DARKBLUE)      \
  X(PURPLE)        \
  X(VIOLET)        \
  X(DARKPURPLE)    \
  X(BEIGE)         \
  X(BROWN)         \
  X(DARKBROWN)     \
  X(WHITE)         \
  X(BLACK)         \
  X(MAGENTA)

#define X(color) COLOR_##color,
enum ColorEnum
{
  COLOR_LIST COLORS_AMOUNT
};
#undef X

#define X(color) color,
static const Color Colors[] = {COLOR_LIST};
#undef X

#define X(color) {#color, COLOR_##color},
static const Slice DefaultSlices[] = {COLOR_LIST};
#undef X

static Slice Slices[COLORS_AMOUNT];
static int SlicesCount = 0;

#endif  // PICKLE_GLOBALS

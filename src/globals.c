#ifndef PICKLE_GLOBALS
#define PICKLE_GLOBALS

#include "../raylib/src/raylib.h"

#define ARRAY_LENGTH(arr) sizeof(arr) / sizeof(arr[0])
#define APP_NAME "pickle"
#define TEXT_SPACING 2
#define ROUNDNESS 0.2f
#define KEY_REPEAT_INTERVAL 0.2f
#define FOREGROUND_COLOR BLACK
#define BACKGROUND_COLOR WHITE
#define HIGHLIGHT_COLOR LIGHTGRAY
#define RED_HIGHLIGHT_COLOR \
  (Color)                   \
  {                         \
    255, 122, 122, 255      \
  }

typedef struct
{
  float Width;
  Color Color;
  Color BorderColor;
  float BorderThickness;
} Column;

typedef struct
{
  float Height;
  int ColumnCount;
  Column* Columns;
} Row;

typedef struct
{
  char* Name;
  int Color;
} Slice;

static float ScreenWidth = 0;
static float ScreenHeight = 0;
static float ScreenPadding = 0;
static float MouseX = 0;
static float MouseY = 0;
static float MousePressedX = 0;
static float MousePressedY = 0;
static float BackspacePressedTime = 0;
static int TouchCount = 0;
static Font Fonte;
static float FontSize = 0;
static bool MenuOpened = false;
static bool ButtonWasPressed = false;
static bool Clicked = false;
static int TypingIndex = -1;

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
static const Color Colors[] = {
  COLOR_LIST
};
#undef X

#define X(color) {#color, COLOR_##color},
static const Slice DefaultSlices[] = {
  COLOR_LIST
};
#undef X


static Slice Slices[COLORS_AMOUNT];
static int SlicesCount = 0;

#endif  // PICKLE_GLOBALS

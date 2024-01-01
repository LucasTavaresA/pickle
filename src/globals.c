#ifndef PICKLE_GLOBALS
#define PICKLE_GLOBALS

#include "../raylib/src/raylib.h"

#define ARRAY_LENGTH(arr) sizeof(arr) / sizeof(arr[0])
#define APP_NAME "pickle"
#define TEXT_SPACING 2
#define ROUNDNESS 0.2f
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
  Color Color;
} Slice;

static float ScreenWidth = 0;
static float ScreenHeight = 0;
static float ScreenPadding = 0;
static float MouseX = 0;
static float MouseY = 0;
static float MousePressedX = 0;
static float MousePressedY = 0;
static int TouchCount = 0;
static Font Fonte;
static float FontSize = 0;
static bool MenuOpened = false;
static bool ButtonWasPressed = false;

#define ColorsAmount ARRAY_LENGTH(Colors)
static const Color Colors[] = {
    LIGHTGRAY,   // Light Gray
    GRAY,        // Gray
    DARKGRAY,    // Dark Gray
    YELLOW,      // Yellow
    GOLD,        // Gold
    ORANGE,      // Orange
    PINK,        // Pink
    RED,         // Red
    MAROON,      // Maroon
    GREEN,       // Green
    LIME,        // Lime
    DARKGREEN,   // Dark Green
    SKYBLUE,     // Sky Blue
    BLUE,        // Blue
    DARKBLUE,    // Dark Blue
    PURPLE,      // Purple
    VIOLET,      // Violet
    DARKPURPLE,  // Dark Purple
    BEIGE,       // Beige
    BROWN,       // Brown
    DARKBROWN,   // Dark Brown
    WHITE,       // White
    BLACK,       // Black
    MAGENTA,     // Magenta
};

static Slice DefaultSlices[] = {
    {"Light Gray", LIGHTGRAY},
    {"Gray", GRAY},
    {"Dark Gray", DARKGRAY},
    {"Yellow", YELLOW},
    {"Gold", GOLD},
    {"Orange", ORANGE},
    {"Pink", PINK},
    {"Red", RED},
    {"Maroon", MAROON},
    {"Green", GREEN},
    {"Lime", LIME},
    {"Dark Green", DARKGREEN},
    {"Sky Blue", SKYBLUE},
    {"Blue", BLUE},
    {"Dark Blue", DARKBLUE},
    {"Purple", PURPLE},
    {"Violet", VIOLET},
    {"Dark Purple", DARKPURPLE},
    {"Beige", BEIGE},
    {"Brown", BROWN},
    {"Dark Brown", DARKBROWN},
    {"White", WHITE},
    {"Black", BLACK},
    {"Magenta", MAGENTA},
};

static Slice Slices[ColorsAmount];
static int SlicesCount = 0;

#endif // PICKLE_GLOBALS

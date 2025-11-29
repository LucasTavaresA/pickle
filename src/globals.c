#ifndef PICKLE_GLOBALS
#define PICKLE_GLOBALS

#include "../raylib/src/raylib.h"

#define ARRAY_LENGTH(arr) sizeof(arr) / sizeof(arr[0])

// NOTE(LucasTA): Odd number off colors is not supported XD
#define COLOR_LIST \
	X(GRAY)          \
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
static const Color COLORS[] = {COLOR_LIST};
#undef X

#define SLICE_NAME_SIZE 256
typedef struct
{
	char Name[SLICE_NAME_SIZE];
	int Color;
} Slice;

#define X(color) {#color "\0", COLOR_##color},
static const Slice DEFAULT_SLICES[] = {COLOR_LIST};
#undef X

static Slice Slices[COLORS_AMOUNT];
static int SlicesCount = 0;

#define PALETTE_ROW_AMOUNT 2
#define PALETTE_COL_AMOUNT ((int)(COLORS_AMOUNT / PALETTE_ROW_AMOUNT))
#define PALETTE_ROW_PERCENTAGE (100.0f / (PALETTE_ROW_AMOUNT))
#define PALETTE_COL_PERCENTAGE (100.0f / (PALETTE_COL_AMOUNT))

static const char APP_NAME[] = "pickle";
static const char WHEEL_TEXT[] = "SPIN!";
static const int TEXT_SPACING = 2;
static const float ROUNDNESS = 0.2f;
static const Color FOREGROUND_COLOR = BLACK;
// NOTE(LucasTA): background should not be a color in COLOR_LIST
static const Color BACKGROUND_COLOR = LIGHTGRAY;
static const Color HIGHLIGHT_COLOR = WHITE;
static const Color HOVERED_COLOR = GRAY;
static const Color PRESSED_COLOR = DARKGRAY;
static const Color RED_PRESSED_COLOR = MAROON;
static const Color RED_HOVERED_COLOR = (Color){255, 122, 122, 255};
static const Color GREEN_PRESSED_COLOR = DARKGREEN;
static const Color GREEN_HOVERED_COLOR = (Color){255, 255, 122, 255};

#ifdef PLATFORM_ANDROID
static Vector2 StartTouchPosition;
static int TouchCount = 0;
static const float INITIAL_REPEAT_INTERVAL = 0.8f;
#else
static const float INITIAL_REPEAT_INTERVAL = 0.3f;
static float MouseScroll = 0;
#endif

static const float MIN_REPEAT_INTERVAL = INITIAL_REPEAT_INTERVAL / 10;
static float ButtonPressedTime = 0;
static float KeyRepeatInterval = INITIAL_REPEAT_INTERVAL;
static float MenuScrollOffset = 0;
static float DeltaTime = 0;
static float WheelAcceleration = 0;
static float WheelAccelerationRate = 0;
static float WheelAngle = 0;
static Vector2 WheelTextSize = {0};
static bool ButtonWasPressed = false;
static bool Clicked = false;
static bool Dragging = false;
static Font Fonte;
static int ScreenWidth = 0;
static int ScreenHeight = 0;
static int Padding = 0;
static int Border = 0;
static int MouseX = 0;
static int MouseY = 0;
static int MousePressedX = 0;
static int MousePressedY = 0;
static int FontSize = 0;
static int TypingIndex = -1;

enum SceneEnum
{
	SCENE_WHEEL,
	SCENE_MENU,
#ifdef PLATFORM_ANDROID
	SCENE_KEYBOARD
#endif
};

static int CurrentScene = SCENE_WHEEL;

enum StateEnum
{
	STATE_WINNER_POPUP = -3,
	STATE_SPINNING = -2,
	STATE_NO_WINNER = -1,
	STATE_WINNER = 0,
};

static int WheelPickedIndex = STATE_NO_WINNER;

#include "../assets/MenuIcon.h"
#include "../assets/TrashIcon.h"
#include "../assets/icon_xhdpi.h"

#define ICON_LIST   \
	X(Window, WINDOW) \
	X(Menu, MENU)     \
	X(Trash, TRASH)

#define X(Name, NAME)                                               \
	static Image Name##Image = {NAME##ICON_DATA, NAME##ICON_WIDTH,    \
															NAME##ICON_HEIGHT, NAME##ICON_FORMAT, \
															PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};   \
	static Texture2D Name##Texture;
ICON_LIST
#undef X

#endif	// PICKLE_GLOBALS

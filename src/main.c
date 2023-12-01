#include <math.h>
#include <stdio.h>
#include <string.h>

#include "../raylib/src/raylib.h"

#ifdef RELEASE
#define LogIfTrue(...)
#define LogSet(...)
#define LogDraw(...)
#else
static char LogMessage[1024] = "";

#define LogIfTrue(result, formatStr, ...)                               \
  if (result)                                                           \
  {                                                                     \
    sprintf(LogMessage + strlen(LogMessage), formatStr, ##__VA_ARGS__); \
  }

#define LogDraw() DrawText(LogMessage, 0, 0, 20, RED)
#define LogSet(message)                             \
  strncpy(LogMessage, message, sizeof(LogMessage)); \
  LogMessage[sizeof(LogMessage) - 1] = '\0';
#endif

#define ARRAY_LENGTH(arr) sizeof(arr) / sizeof(arr[0])
#define APP_NAME "pickle"
#define ROUNDNESS 0.2f
#define FOREGROUND_COLOR BLACK
#define BACKGROUND_COLOR WHITE
#define HIGHLIGHT_COLOR LIGHTGRAY
#define RED_HIGHLIGHT_COLOR \
  (Color)                   \
  {                         \
    255, 122, 122, 255      \
  }
#define CONTRAST_LIMIT 128

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

// TODO(LucasTA): manipulating this in the code directly, maybe i should
// operate on functions, or even make a separate file for this
static bool SlicesDefault = true;
static int SlicesCount = ColorsAmount;
static char* Slices[ColorsAmount] = {
    "Light Gray", "Gray",  "Dark Gray",  "Yellow", "Gold",   "Orange",
    "Pink",       "Red",   "Maroon",     "Green",  "Lime",   "Dark Green",
    "Sky Blue",   "Blue",  "Dark Blue",  "Purple", "Violet", "Dark Purple",
    "Beige",      "Brown", "Dark Brown", "White",  "Black",  "Magenta",
};
static Color SlicesColors[ColorsAmount];

static bool IsPointInsideRect(int x,
                              int y,
                              int recX,
                              int recY,
                              int recWidth,
                              int recHeight)
{
  return x >= recX && x <= recX + recWidth && y >= recY &&
         y <= recY + recHeight;
}

static Color GetForegroundColor(Color color)
{
  if ((color.r + color.g + color.b) / 3 < CONTRAST_LIMIT)
  {
    return WHITE;
  }
  else
  {
    return BLACK;
  }
}

static void DrawCross(float x,
                      float y,
                      float angle,
                      float length,
                      float thickness,
                      Color color)
{
  float radians = DEG2RAD * angle;

  Vector2 startHorizontal = {x - length / 2 * cosf(radians),
                             y - length / 2 * sinf(radians)};
  Vector2 endHorizontal = {x + length / 2 * cosf(radians),
                           y + length / 2 * sinf(radians)};

  Vector2 startVertical = {x - length / 2 * cosf(radians + PI / 2),
                           y - length / 2 * sinf(radians + PI / 2)};
  Vector2 endVertical = {x + length / 2 * cosf(radians + PI / 2),
                         y + length / 2 * sinf(radians + PI / 2)};

  DrawLineEx(startHorizontal, endHorizontal, thickness, color);
  DrawLineEx(startVertical, endVertical, thickness, color);
}

static void DrawRectangleGrid(float x,
                              float y,
                              float width,
                              float height,
                              float padding,
                              Row* rows,
                              int rows_amount)
{
  LogIfTrue(x < 0 || y < 0 || width <= 0 || height <= 0 ||
                x + width > ScreenWidth || y + height > ScreenHeight,
            "ERROR: Rectangle grid is outside of the screen!\n");

  float availableHeight = height - (padding * (rows_amount - 1));
  float curY = y;
  float takenHeight = 0;

  for (int i = 0; i < rows_amount; i++)
  {
    float rowLength = availableHeight * rows[i].Height / 100;

    float availableWidth = width - (padding * (rows[i].ColumnCount - 1));
    float curX = x;
    float takenWidth = 0;

    for (int j = 0; j < rows[i].ColumnCount; j++)
    {
      float colLength = availableWidth * rows[i].Columns[j].Width / 100;
      Rectangle rect = (Rectangle){curX, curY, colLength, rowLength};

      DrawRectangleRec(rect, rows[i].Columns[j].Color);

      if (rows[i].Columns[j].BorderThickness > 0)
      {
        DrawRectangleLinesEx(rect, rows[i].Columns[j].BorderThickness,
                             rows[i].Columns[j].BorderColor);
      }

      curX += colLength + padding;
      takenWidth += colLength;

      LogIfTrue(takenWidth > availableWidth,
                "ERROR: Rectangle grid %d row %d column takes more than the "
                "available width!\n",
                i + 1, j + 1);
    }

    curY += rowLength + padding;
    takenHeight += rowLength;

    LogIfTrue(
        takenHeight > availableHeight,
        "ERROR: Rectangle grid %d row takes more than the available height!\n",
        i + 1);
  }
}

int main()
{
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
  InitWindow(ScreenWidth, ScreenHeight, APP_NAME);

#ifndef PLATFORM_ANDROID
  ChangeDirectory("assets");
#endif

  Fonte = LoadFont("iosevka-regular.ttf");

#ifndef PLATFORM_ANDROID
  ChangeDirectory("..");
#endif

  SetTextureFilter(Fonte.texture, TEXTURE_FILTER_BILINEAR);

  while (!WindowShouldClose())
  {
    // key events
    {
      if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_Q))
      {
        MenuOpened = false;
      }

      if (IsKeyPressed(KEY_SPACE))
      {
        MenuOpened = !MenuOpened;
      }
    }

    // update screen information
    {
      ScreenWidth = GetScreenWidth();
      ScreenHeight = GetScreenHeight();
      ScreenPadding =
          (ScreenWidth < ScreenHeight ? ScreenWidth / 16 : ScreenHeight / 32);
      FontSize = (ScreenWidth < ScreenHeight ? ScreenWidth : ScreenHeight) / 30;
    }

    // update mouse and touch information
    {
      MouseX = GetMouseX();
      MouseY = GetMouseY();
      TouchCount = GetTouchPointCount();

      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
      {
        MousePressedX = MouseX;
        MousePressedY = MouseY;
      }
    }

    // Draw
    {
      BeginDrawing();
      ClearBackground(BACKGROUND_COLOR);

      float corner_button_size =
          (ScreenWidth < ScreenHeight ? ScreenWidth : ScreenHeight) / 8;
      float corner_button_padding = ScreenWidth / 250;
      Rectangle corner_button = {
          ScreenWidth - ScreenPadding - corner_button_size, ScreenPadding,
          corner_button_size, corner_button_size};

      if (!MenuOpened)
      {
        // Draw the wheel
        {
          Vector2 center = {ScreenWidth / 2, ScreenHeight / 2};
          float sectionSize = 360.0f / SlicesCount;
          float startAngle = 0;
          float endAngle = sectionSize;
          float radius =
              (ScreenWidth < ScreenHeight ? ScreenWidth : ScreenHeight) / 2 -
              ScreenPadding;

          // Draw a border on the wheel
          DrawRing(center, radius - 2, radius + 2, 0, 360, 0, FOREGROUND_COLOR);

          for (int i = 0; i < SlicesCount; i++)
          {
            DrawCircleSector(center, radius, startAngle, endAngle, 0,
                             Colors[i]);

            float midAngle = ((startAngle + endAngle) / 2) - FontSize / 8;
            Vector2 labelPosition = {
                center.x + (radius / 2) * cosf(midAngle * DEG2RAD),
                center.y + (radius / 2) * sinf(midAngle * DEG2RAD)};

            DrawTextPro(Fonte, Slices[i], labelPosition, (Vector2){0.5f, 0.5f},
                        midAngle, FontSize, 2, GetForegroundColor(Colors[i]));

            startAngle += sectionSize;
            endAngle += sectionSize;
          }

          // Draw a circle in the middle of the whell
          float inner_circle_radius = radius / 4;

          DrawCircleV(center, inner_circle_radius, FOREGROUND_COLOR);
          DrawRing(center, inner_circle_radius, inner_circle_radius + 2, 0, 360,
                   0, BACKGROUND_COLOR);

          // Draw the wheel paddle
          float paddle_bottom = center.y + radius + 20;

          DrawTriangle((Vector2){center.x, paddle_bottom - 80},
                       (Vector2){center.x - 15, paddle_bottom},
                       (Vector2){center.x + 15, paddle_bottom}, RED);
        }

        // Draw a menu button
        {
          if (IsPointInsideRect(MouseX, MouseY, corner_button.x,
                                corner_button.y, corner_button.width,
                                corner_button.height))
          {
            if ((IsCursorOnScreen() || TouchCount > 0))
            {
              DrawRectangleRounded(corner_button, 0, 0, HIGHLIGHT_COLOR);
            }
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
              MenuOpened = true;
            }
          }

          DrawRectangleRoundedLines(corner_button, ROUNDNESS, 0,
                                    corner_button_padding, FOREGROUND_COLOR);

          // Draw a menu icon
          Row rows[] = {
              {33.33f, 1, (Column[]){(Column){100, FOREGROUND_COLOR}}},
              {33.33f, 1, (Column[]){(Column){100, FOREGROUND_COLOR}}},
              {33.33f, 1, (Column[]){(Column){100, FOREGROUND_COLOR}}},
          };

          DrawRectangleGrid(corner_button.x + (corner_button_size / 8),
                            corner_button.y + (corner_button_size / 8) +
                                corner_button_padding / 2,
                            corner_button_size * 3 / 4,
                            corner_button_size * 3 / 4, corner_button_padding,
                            rows, ARRAY_LENGTH(rows));
        }
      }
      else
      {
        // draw the slices menu
        {
          float side_padding =
              (ScreenWidth < ScreenHeight ? 0 : ScreenPadding * 8);
          float button_height = (ScreenWidth < ScreenHeight ? ScreenHeight / 6
                                                            : ScreenHeight / 3);
          float button_border = ScreenWidth / 500;

          if (SlicesDefault)
          {
            // draw a button to add a slice
            Rectangle add_button = {
                side_padding, 0, ScreenWidth - side_padding * 2, button_height};

            if (!IsPointInsideRect(MouseX, MouseY, corner_button.x,
                                   corner_button.y, corner_button.width,
                                   corner_button.height) &&
                IsPointInsideRect(MouseX, MouseY, add_button.x, add_button.y,
                                  add_button.width, add_button.height))
            {
              if ((IsCursorOnScreen() || TouchCount > 0))
              {
                DrawRectangleRec(add_button, HIGHLIGHT_COLOR);
              }
              if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
              {
                // SlicesDefault = false;
                // SlicesCount = 1;
                // Slices[0] = "";
                // SlicesColors[0] = Colors[0];
              }
            }
            else
            {
              DrawRectangleRec(add_button, BACKGROUND_COLOR);
            }

            DrawRectangleLinesEx(add_button, button_border, FOREGROUND_COLOR);

            DrawCross(add_button.x + add_button.width / 2,
                      add_button.y + add_button.height / 2, 0,
                      button_height / 2, button_height / 10, FOREGROUND_COLOR);
          }
          else
          {
            for (int i = 0; i < SlicesCount; i++)
            {
              Rectangle button_rect = {side_padding, i * button_height,
                                       ScreenWidth - side_padding * 2,
                                       button_height};

              DrawRectangleRec(button_rect, BACKGROUND_COLOR);
              DrawRectangleLinesEx(button_rect, button_border,
                                   FOREGROUND_COLOR);
              // https://github.com/raysan5/raylib/blob/e7a486fa81adac1833253c849ca73c5b3f7ef361/examples/text/text_input_box.c
            }

            // draw a button to add a slice
            if (SlicesCount < ColorsAmount)
            {
            }
          }
        }

        // Draw a close button
        {
          if (IsPointInsideRect(MouseX, MouseY, corner_button.x,
                                corner_button.y, corner_button.width,
                                corner_button.height))
          {
            if ((IsCursorOnScreen() || TouchCount > 0))
            {
              DrawRectangleRounded(corner_button, 0, 0, RED_HIGHLIGHT_COLOR);
            }
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
              MenuOpened = false;
            }
          }

          DrawRectangleRoundedLines(corner_button, ROUNDNESS, 0,
                                    corner_button_padding, RED);

          DrawCross(corner_button.x + corner_button.width / 2,
                    corner_button.y + corner_button.height / 2, 45,
                    corner_button_size, corner_button_size / 8, RED);
        }
      }

      LogDraw();
      LogSet("");

      EndDrawing();
    }
  }

  CloseWindow();
  return 0;
}

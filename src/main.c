#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../raylib/src/raylib.h"

#ifdef RELEASE
#define LogIfTrue(...)
#define LogIfBadContrast(...)
#define LogSet(...)
#define LogDraw(...)
#else
static char LogMessage[1024] = "";

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
#define CONTRAST_LIMIT 85

// picks between black and white depending on the passed color
#define GetContrastedTextColor(color) \
  ((color.r + color.g + color.b) / 3 < CONTRAST_LIMIT) ? WHITE : BLACK

#define IsPointInsideRect(x, y, recX, recY, recWidth, recHeight) \
  (x >= recX && x <= recX + recWidth && y >= recY && y <= recY + recHeight)

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

#define DEFAULT_SLICES                                                      \
  (Slice[])                                                                 \
  {                                                                         \
    {"Light Gray", LIGHTGRAY}, {"Gray", GRAY}, {"Dark Gray", DARKGRAY},     \
        {"Yellow", YELLOW}, {"Gold", GOLD}, {"Orange", ORANGE},             \
        {"Pink", PINK}, {"Red", RED}, {"Maroon", MAROON}, {"Green", GREEN}, \
        {"Lime", LIME}, {"Dark Green", DARKGREEN}, {"Sky Blue", SKYBLUE},   \
        {"Blue", BLUE}, {"Dark Blue", DARKBLUE}, {"Purple", PURPLE},        \
        {"Violet", VIOLET}, {"Dark Purple", DARKPURPLE}, {"Beige", BEIGE},  \
        {"Brown", BROWN}, {"Dark Brown", DARKBROWN}, {"White", WHITE},      \
        {"Black", BLACK}, {"Magenta", MAGENTA},                             \
  }

typedef struct
{
  char* Name;
  Color Color;
} Slice;

static Slice Slices[ColorsAmount];
static int SlicesCount = 0;

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

        LogIfBadContrast(
            rows[i].Columns[j].Color, rows[i].Columns[j].BorderColor,
            "ERROR: Bad border contrast at [%.0f, %.0f] Grid, [%d, %d] Column",
            x, y, i, j);
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

static void DrawWheel(float angle,
                      float radius,
                      Slice* slices,
                      int slice_amount)
{
  Vector2 center = {ScreenWidth / 2, ScreenHeight / 2};
  float sectionSize = 360.0f / slice_amount;
  float startAngle = 0;
  float endAngle = sectionSize;

  for (int i = 0; i < slice_amount; i++)
  {
    DrawCircleSector(center, radius, startAngle, endAngle, 0, slices[i].Color);

    float midAngle = ((startAngle + endAngle) / 2) - FontSize / 8;
    Vector2 labelPosition = {
        center.x + (radius / 2) * cosf(midAngle * DEG2RAD),
        center.y + (radius / 2) * sinf(midAngle * DEG2RAD)};

    DrawTextPro(Fonte, slices[i].Name, labelPosition, (Vector2){0.5f, 0.5f},
                midAngle, FontSize, 2, GetContrastedTextColor(Colors[i]));

    startAngle += sectionSize;
    endAngle += sectionSize;
  }

  // Draw a border on the wheel
  DrawRing(center, radius - 2, radius + 2, 0, 360, 0, FOREGROUND_COLOR);

  // Draw a circle in the middle of the wheel
  float inner_circle_radius = radius / 4;

  DrawCircleV(center, inner_circle_radius, FOREGROUND_COLOR);
  DrawRing(center, inner_circle_radius, inner_circle_radius + 2, 0, 360, 0,
           BACKGROUND_COLOR);

  // Draw the wheel paddle
  float paddle_bottom = center.y + radius + 20;

  DrawTriangle((Vector2){center.x, paddle_bottom - 80},
               (Vector2){center.x - 15, paddle_bottom},
               (Vector2){center.x + 15, paddle_bottom}, RED);
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
      ButtonWasPressed = false;

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

      float square_button_size =
          (ScreenWidth < ScreenHeight ? ScreenWidth : ScreenHeight) / 8;
      float square_button_padding = ScreenWidth / 250;
      Rectangle corner_button_rect = {
          ScreenWidth - square_button_size - square_button_padding,
          square_button_padding, square_button_size, square_button_size};

      if (!MenuOpened)
      {
        // Draw the wheel
        {
          float wheel_radius =
              (ScreenWidth < ScreenHeight ? ScreenWidth : ScreenHeight) / 2 -
              ScreenPadding;

          if (SlicesCount == 0)
          {
            DrawWheel(0, wheel_radius, DEFAULT_SLICES, ColorsAmount);
          }
          else
          {
            DrawWheel(0, wheel_radius, Slices, SlicesCount);
          }
        }

        // Draw a menu button
        {
          if (IsPointInsideRect(MouseX, MouseY, corner_button_rect.x,
                                corner_button_rect.y, corner_button_rect.width,
                                corner_button_rect.height))
          {
            if ((IsCursorOnScreen() || TouchCount > 0))
            {
              DrawRectangleRec(corner_button_rect, HIGHLIGHT_COLOR);
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !ButtonWasPressed)
            {
              ButtonWasPressed = true;
              MenuOpened = true;
            }
          }

          DrawRectangleLinesEx(corner_button_rect, square_button_padding,
                               FOREGROUND_COLOR);

          // Draw a menu icon
          Row rows[] = {
              {33.33f, 1, (Column[]){(Column){100, FOREGROUND_COLOR}}},
              {33.33f, 1, (Column[]){(Column){100, FOREGROUND_COLOR}}},
              {33.33f, 1, (Column[]){(Column){100, FOREGROUND_COLOR}}},
          };

          DrawRectangleGrid(corner_button_rect.x + (square_button_size / 8),
                            corner_button_rect.y + (square_button_size / 8),
                            square_button_size * 3 / 4,
                            square_button_size * 3 / 4, square_button_padding,
                            rows, ARRAY_LENGTH(rows));
        }
      }
      else
      {
        // draw the slices menu
        {
          float side_padding =
              (ScreenWidth < ScreenHeight ? 0 : ScreenPadding * 8);
          float slice_item_height =
              (ScreenWidth < ScreenHeight ? ScreenHeight / 6
                                          : ScreenHeight / 3);
          float slice_item_width = ScreenWidth - side_padding * 2;
          float slice_item_border = ScreenWidth / 500;

          // draw a button to add a slice
          if (SlicesCount == 0 || SlicesCount < ColorsAmount)
          {
            Rectangle add_button = {
                side_padding,
                (ScreenWidth < ScreenHeight
                     ? square_button_size + square_button_padding
                     : 0) +
                    slice_item_height * SlicesCount,
                slice_item_width, slice_item_height};

            if (!IsPointInsideRect(
                    MouseX, MouseY, corner_button_rect.x, corner_button_rect.y,
                    corner_button_rect.width, corner_button_rect.height) &&
                IsPointInsideRect(MouseX, MouseY, add_button.x, add_button.y,
                                  add_button.width, add_button.height))
            {
              if ((IsCursorOnScreen() || TouchCount > 0))
              {
                DrawRectangleRec(add_button, HIGHLIGHT_COLOR);
              }

              if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !ButtonWasPressed)
              {
                ButtonWasPressed = true;
                SlicesCount++;
                Slices[SlicesCount].Name = "";
                Slices[SlicesCount].Color = Colors[0];
              }
            }
            else
            {
              DrawRectangleRec(add_button, BACKGROUND_COLOR);
            }

            DrawRectangleLinesEx(add_button, slice_item_border,
                                 FOREGROUND_COLOR);

            DrawCross(add_button.x + add_button.width / 2,
                      add_button.y + add_button.height / 2, 0,
                      slice_item_height / 2, slice_item_height / 10,
                      FOREGROUND_COLOR);
          }

          for (int i = 0; i < SlicesCount; i++)
          {
            Rectangle item_rect = {
                side_padding,
                (ScreenWidth < ScreenHeight
                     ? square_button_size + square_button_padding
                     : 0) +
                    i * slice_item_height,
                slice_item_width, slice_item_height};

            DrawRectangleLinesEx(item_rect, slice_item_border,
                                 FOREGROUND_COLOR);
            // https://github.com/raysan5/raylib/blob/e7a486fa81adac1833253c849ca73c5b3f7ef361/examples/text/text_input_box.c

            // draw a trash button to delete slices
            {
              Rectangle trash_button = {
                  ScreenWidth - side_padding - square_button_size -
                      square_button_padding,
                  (ScreenWidth < ScreenHeight
                       ? square_button_size + square_button_padding
                       : 0) +
                      i * slice_item_height + square_button_padding,
                  square_button_size, square_button_size};

              if (IsPointInsideRect(MouseX, MouseY, trash_button.x,
                                    trash_button.y, trash_button.width,
                                    trash_button.height))
              {
                if ((IsCursorOnScreen() || TouchCount > 0))
                {
                  DrawRectangleRec(trash_button, RED_HIGHLIGHT_COLOR);
                }

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                    !ButtonWasPressed)
                {
                  ButtonWasPressed = true;
                  SlicesCount--;
                }
              }

              DrawRectangleLinesEx(trash_button, slice_item_border, RED);

              float trash_button_lid_handle_size = square_button_padding * 4;

              // draw a trash icon
              {
                // trash can bucket
                DrawRectangleRounded(
                    (Rectangle){trash_button.x + trash_button.width / 4,
                                trash_button.y + trash_button.height / 4,
                                square_button_size / 2, square_button_size / 2},
                    ROUNDNESS, 0, RED);

                // trash can bucket shape
                DrawRectangleGrid(
                    trash_button.x + trash_button.width / 4 +
                        square_button_padding,
                    trash_button.y + trash_button.height / 4 +
                        square_button_padding,
                    square_button_size / 2 - square_button_padding * 2,
                    square_button_size / 2 - square_button_padding * 2, 3,
                    (Row[]){100, 3,
                            (Column[]){(Column){33.33f, RED_HIGHLIGHT_COLOR},
                                       (Column){33.33f, RED_HIGHLIGHT_COLOR},
                                       (Column){33.33f, RED_HIGHLIGHT_COLOR}}},
                    1);

                // trash can handle
                DrawRectangleRec(
                    (Rectangle){trash_button.x + trash_button.width / 2 -
                                    trash_button_lid_handle_size / 2,
                                trash_button.y + trash_button.height / 6,
                                trash_button_lid_handle_size,
                                trash_button_lid_handle_size},
                    RED);

                // trash can lid
                DrawRectangleRec(
                    (Rectangle){trash_button.x + trash_button.width / 4 -
                                    trash_button_lid_handle_size / 4,
                                trash_button.y + trash_button.height / 4,
                                square_button_size / 2 +
                                    trash_button_lid_handle_size / 2,
                                square_button_size / 6},
                    RED);
              }
            }
          }
        }

        // Draw a close button
        {
          if (IsPointInsideRect(MouseX, MouseY, corner_button_rect.x,
                                corner_button_rect.y, corner_button_rect.width,
                                corner_button_rect.height))
          {
            if ((IsCursorOnScreen() || TouchCount > 0))
            {
              DrawRectangleRec(corner_button_rect, RED_HIGHLIGHT_COLOR);
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !ButtonWasPressed)
            {
              ButtonWasPressed = true;
              MenuOpened = false;
            }
          }

          DrawRectangleLinesEx(corner_button_rect, square_button_padding, RED);

          DrawCross(corner_button_rect.x + corner_button_rect.width / 2,
                    corner_button_rect.y + corner_button_rect.height / 2, 45,
                    square_button_size, square_button_size / 8, RED);
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

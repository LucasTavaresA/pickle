#ifndef PICKLE_DRAW
#define PICKLE_DRAW
#include <math.h>
#include <stdlib.h>

#include "../raylib/src/raylib.h"

#include "char.c"
#include "globals.c"
#include "log.c"

#define CONTRAST_LIMIT 90

#define IsPointInsideRect(x, y, recX, recY, recWidth, recHeight) \
  (x >= recX && x <= recX + recWidth && y >= recY && y <= recY + recHeight)

// picks between black and white depending on the passed color
#define GetContrastedTextColor(color) \
  ((color.r + color.g + color.b) / 3 < CONTRAST_LIMIT) ? WHITE : BLACK

#define CheckBadContrast(backgroundColor, textColor) \
  ((abs(backgroundColor.r - textColor.r) +           \
    abs(backgroundColor.g - textColor.g) +           \
    abs(backgroundColor.b - textColor.b)) /          \
       3 <                                           \
   CONTRAST_LIMIT)

typedef enum
{
  ShadowKind_Float,
  ShadowKind_Cast,
  ShadowKind_Pillar,
  ShadowKind_Cube,
  ShadowKind_TransparentCube,
} ShadowKind;

typedef struct
{
  int Distance;
  Color Color;
  ShadowKind Kind;
} ShadowStyle;

typedef struct
{
  int Width;
  Color Color;
  Color BorderColor;
  int BorderThickness;
} Column;

typedef struct
{
  int Height;
  int ColumnCount;
  Column* Columns;
} Row;

typedef struct
{
  int WidthPercentage;
  char* Text;
  int FontSize;
  bool RepeatPresses;
  Color TextColor;
  Color BackgroundColor;
  Color PressedColor;
  Color HoveredColor;
  Color BorderColor;
  int BorderThickness;
  ShadowStyle ShadowStyle;
  Texture2D Icon;
  void (*Callback)(void* CallbackArgs);
  void* CallbackArgs;
} Button;

typedef struct
{
  int HeightPercentage;
  int ColumnAmount;
  Button* Columns;
} ButtonRow;

static const ShadowStyle NO_SHADOW = (ShadowStyle){0};
static const Texture2D NO_ICON = (Texture2D){0};
static const Rectangle ICON_RECTANGLE = (Rectangle){0, 0, 24, 24};

static void DrawShadow(int x,
                       int y,
                       int width,
                       int height,
                       ShadowStyle shadowStyle,
                       Color outlineColor)
{
  DrawRectangle(x + shadowStyle.Distance, y + shadowStyle.Distance, width,
                height, shadowStyle.Color);

  if (shadowStyle.Kind != ShadowKind_Float)
  {
    // top right corner
    DrawTriangle(
        (Vector2){x + shadowStyle.Distance, y + height},
        (Vector2){x, y + height},
        (Vector2){x + shadowStyle.Distance, y + height + shadowStyle.Distance},
        shadowStyle.Color);

    // bottom left corner
    DrawTriangle(
        (Vector2){x + width, y}, (Vector2){x + width, y + shadowStyle.Distance},
        (Vector2){x + width + shadowStyle.Distance, y + shadowStyle.Distance},
        shadowStyle.Color);

    if (shadowStyle.Kind != ShadowKind_Cast)
    {
      if (shadowStyle.Kind == ShadowKind_Cube)
      {
        // left cube shadow outline
        DrawLine(x + width + shadowStyle.Distance, y + shadowStyle.Distance,
                 x + width + shadowStyle.Distance,
                 y + height + shadowStyle.Distance, outlineColor);

        // bottom cube shadow outline
        DrawLine(x + shadowStyle.Distance, y + height + shadowStyle.Distance,
                 x + width + shadowStyle.Distance,
                 y + height + shadowStyle.Distance, outlineColor);
      }
      else if (shadowStyle.Kind == ShadowKind_TransparentCube)
      {
        DrawRectangleLines(x + shadowStyle.Distance, y + shadowStyle.Distance,
                           width, height, outlineColor);
      }

      // top right outline
      DrawLine(x + width, y, x + width + shadowStyle.Distance,
               y + shadowStyle.Distance, outlineColor);

      // bottom left outline
      DrawLine(x, y + height, x + shadowStyle.Distance,
               y + height + shadowStyle.Distance, outlineColor);

      // bottom right outline
      DrawLine(x + width, y + height, x + width + shadowStyle.Distance,
               y + height + shadowStyle.Distance, outlineColor);
    }
  }
}

static void DrawTextBox(int x,
                        int y,
                        int width,
                        int height,
                        char* text,
                        int fontSize,
                        Color textColor,
                        Color backgroundColor,
                        Color borderColor,
                        int borderThickness,
                        ShadowStyle shadowStyle)
{
  LogIf((strcmp(text, "") != 0) && CheckBadContrast(backgroundColor, textColor),
        LogAppend("ERROR: The text at the %d,%d text box is not visible!\n", x,
                  y));

  if (fontSize == 0)
  {
    fontSize = FontSize;
  }

  Vector2 textSize;

  do
  {
    textSize = MeasureTextEx(Fonte, text, fontSize, TEXT_SPACING);
    fontSize -= 1;
  } while (textSize.x > width || textSize.y > height);

  LogIf(
      textSize.x > width || textSize.y > height,
      LogAppend("ERROR: The text at the %d,%d text box does not fit its box!\n",
                x, y));

  int textX = x + ((width - textSize.x) / 2);
  int textY = y + ((height - textSize.y) / 2);

  Rectangle rect = {x, y, width, height};

  if (shadowStyle.Distance > 0)
  {
    DrawShadow(x, y, width, height, shadowStyle, borderColor);
  }

  DrawRectangleRec(rect, backgroundColor);

  if (borderThickness > 0)
  {
    DrawRectangleLinesEx(rect, borderThickness, borderColor);
  }
  else if (borderThickness < 0)
  {
    DrawRectangleLinesEx(rect, Padding, borderColor);
  }

  DrawTextEx(Fonte, text, (Vector2){textX, textY}, fontSize, TEXT_SPACING,
             textColor);
}

static void DrawRectangleGrid(int x,
                              int y,
                              int width,
                              int height,
                              int padding,
                              const Row* rows,
                              int rows_amount)
{
  int availableHeight = height - (padding * (rows_amount - 1));
  int curY = y;
  int takenHeight = 0;

  for (int i = 0; i < rows_amount; i++)
  {
    int rowLength = availableHeight * rows[i].Height / 100;

    int availableWidth = width - (padding * (rows[i].ColumnCount - 1));
    int curX = x;
    int takenWidth = 0;

    for (int j = 0; j < rows[i].ColumnCount; j++)
    {
      int colLength = availableWidth * rows[i].Columns[j].Width / 100;
      Rectangle rect = (Rectangle){curX, curY, colLength, rowLength};

      DrawRectangleRec(rect, rows[i].Columns[j].Color);

      if (rows[i].Columns[j].BorderThickness > 0)
      {
        DrawRectangleLinesEx(rect, rows[i].Columns[j].BorderThickness,
                             rows[i].Columns[j].BorderColor);
      }
      else if (rows[i].Columns[j].BorderThickness < 0)
      {
        DrawRectangleLinesEx(rect, Padding, rows[i].Columns[j].BorderColor);
      }

      curX += colLength + padding;
      takenWidth += colLength;

      LogIf(takenWidth > availableWidth,
            LogAppend(
                "ERROR: Rectangle grid %d row %d column takes more than the "
                "available width!\n",
                i + 1, j + 1));
    }

    curY += rowLength + padding;
    takenHeight += rowLength;

    LogIf(takenHeight > availableHeight,
          LogAppend("ERROR: Rectangle grid %d row takes more than the "
                    "available height!\n",
                    i + 1));
  }
}

static void DrawWheel(float angle,
                      float radius,
                      const Slice* slices,
                      int slice_amount)
{
  Vector2 center = {(float)ScreenWidth / 2, (float)ScreenHeight / 2};
  float sectionSize = 360.0f / slice_amount;
  float startAngle = angle;
  float endAngle = sectionSize + angle;

  // Draw a border on the wheel
  DrawRing(center, radius - Border, radius + Border, 0, 360, 0,
           FOREGROUND_COLOR);

  for (int i = 0; i < slice_amount; i++)
  {
    DrawCircleSector(center, radius, startAngle, endAngle, 0,
                     COLORS[slices[i].Color]);

    float midAngle = ((startAngle + endAngle) / 2) - (float)FontSize / 5;

    Vector2 labelPosition = {
        center.x + (radius / 3) * cosf(midAngle * DEG2RAD),
        center.y + (radius / 3) * sinf(midAngle * DEG2RAD)};

    DrawTextPro(Fonte, slices[i].Name, labelPosition, (Vector2){0, 0},
                midAngle + 10, FontSize, TEXT_SPACING,
                GetContrastedTextColor(COLORS[slices[i].Color]));

    startAngle += sectionSize;
    endAngle += sectionSize;
  }

  // Draw a circle in the middle of the wheel
  float inner_circle_radius = radius / 4;

  DrawCircleV(center, inner_circle_radius, FOREGROUND_COLOR);
  DrawRing(center, inner_circle_radius, inner_circle_radius + Border, 0, 360, 0,
           HIGHLIGHT_COLOR);

  // Draw the wheel paddle
  float paddle_bottom = center.y + radius + 20;

  DrawTriangle((Vector2){center.x, paddle_bottom - 80},
               (Vector2){center.x - 15, paddle_bottom},
               (Vector2){center.x + 15, paddle_bottom}, RED);
  DrawTriangleLines((Vector2){center.x, paddle_bottom - 80},
                    (Vector2){center.x - 15, paddle_bottom},
                    (Vector2){center.x + 15, paddle_bottom}, FOREGROUND_COLOR);
}

static void DrawCross(int x,
                      int y,
                      float angle,
                      float length,
                      int thickness,
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

#define HandleKeypress                                                       \
  if (shadowStyle.Distance > 0)                                              \
  {                                                                          \
    x += shadowStyle.Distance;                                               \
    y += shadowStyle.Distance;                                               \
  }                                                                          \
                                                                             \
  DrawTextBox(x, y, width, height, text, fontSize, textColor, pressedColor,  \
              borderColor, borderThickness, shadowStyle);                    \
                                                                             \
  ButtonPressedTime += GetFrameTime();                                       \
                                                                             \
  if (repeatPresses && ButtonPressedTime >= KeyRepeatInterval)       \
  {                                                                          \
    ButtonWasPressed = true;                                                 \
                                                                             \
    if (callback != NULL)                                                    \
    {                                                                        \
      callback(callbackArgs);                                                \
    }                                                                        \
    else                                                                     \
    {                                                                        \
      LogAppend("The '%s' button does not have a command defined!\n", text); \
    }                                                                        \
                                                                             \
    ButtonPressedTime = 0;                                                   \
    KeyRepeatInterval = fmax(KeyRepeatInterval * INITIAL_REPEAT_INTERVAL,    \
                             MIN_REPEAT_INTERVAL);                           \
  }

static void DrawButton(int x,
                       int y,
                       int width,
                       int height,
                       char* text,
                       int fontSize,
                       bool repeatPresses,
                       Color textColor,
                       Color backgroundColor,
                       Color pressedColor,
                       Color hoveredColor,
                       Color borderColor,
                       int borderThickness,
                       ShadowStyle shadowStyle,
                       Texture2D icon,
                       void (*callback)(void* callbackArgs),
                       void* callbackArgs)
{
  if (!Dragging && !ButtonWasPressed &&
      IsMouseButtonReleased(MOUSE_BUTTON_LEFT) &&
      IsPointInsideRect(MouseX, MouseY, x, y, width, height) &&
      IsPointInsideRect(MousePressedX, MousePressedY, x, y, width, height))
  {
    ButtonWasPressed = true;
    ButtonPressedTime = 0;
    KeyRepeatInterval = INITIAL_REPEAT_INTERVAL;

    if (callback != NULL)
    {
      callback(callbackArgs);
    }
    else
    {
      LogAppend("The '%s' button does not have a command defined!\n", text);
    }
  }
  else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
  {
    if (IsPointInsideRect(MouseX, MouseY, x, y, width, height))
    {
      if (!Dragging && !ButtonWasPressed &&
          IsPointInsideRect(MousePressedX, MousePressedY, x, y, width, height))
      {
        HandleKeypress;
      }
      else
      {
        DrawTextBox(x, y, width, height, text, fontSize, textColor,
#ifdef PLATFORM_ANDROID
                    TouchCount > 0 ? hoveredColor : backgroundColor,
#else
                    hoveredColor,
#endif
                    borderColor, borderThickness, shadowStyle);
      }
    }
    else
    {
      if (!Dragging && !ButtonWasPressed &&
          IsPointInsideRect(MousePressedX, MousePressedY, x, y, width, height))
      {
        HandleKeypress;
      }
      else
      {
        DrawTextBox(x, y, width, height, text, fontSize, textColor,
                    backgroundColor, borderColor, borderThickness, shadowStyle);
      }
    }
  }
  else
  {
    if (IsPointInsideRect(MouseX, MouseY, x, y, width, height))
    {
      DrawTextBox(x, y, width, height, text, fontSize, textColor,
#ifdef PLATFORM_ANDROID
                  TouchCount > 0 ? hoveredColor : backgroundColor,
#else
                  hoveredColor,
#endif
                  borderColor, borderThickness, shadowStyle);
    }
    else
    {
      DrawTextBox(x, y, width, height, text, fontSize, textColor,
                  backgroundColor, borderColor, borderThickness, shadowStyle);
    }
  }

  if (icon.width > 0)
  {
    if (width < height)
    {
      height = width;
    }
    else
    {
      width = height;
    }

    DrawTexturePro(icon, ICON_RECTANGLE, (Rectangle){x, y, width, height},
                   (Vector2){0, 0}, 0, WHITE);
  }
}
#undef HandleKeypress

static void DrawButtonGrid(int x,
                           int y,
                           int width,
                           int height,
                           int padding,
                           const ButtonRow* rows,
                           int rows_amount)
{
  int availableHeight = height - (padding * (rows_amount - 1));
  int curY = y;
  int takenHeight = 0;

  for (int i = 0; i < rows_amount; i++)
  {
    int rowLength = availableHeight * rows[i].HeightPercentage / 100;

    int availableWidth = width - (padding * (rows[i].ColumnAmount - 1));
    int curX = x;
    int takenWidth = 0;

    for (int j = 0; j < rows[i].ColumnAmount; j++)
    {
      int colLength = availableWidth * rows[i].Columns[j].WidthPercentage / 100;

      DrawButton(
          curX, curY, colLength, rowLength, rows[i].Columns[j].Text,
          rows[i].Columns[j].FontSize, rows[i].Columns[j].RepeatPresses,
          rows[i].Columns[j].TextColor, rows[i].Columns[j].BackgroundColor,
          rows[i].Columns[j].PressedColor, rows[i].Columns[j].HoveredColor,
          rows[i].Columns[j].BorderColor, rows[i].Columns[j].BorderThickness,
          rows[i].Columns[j].ShadowStyle, rows[i].Columns[j].Icon,
          rows[i].Columns[j].Callback, rows[i].Columns[j].CallbackArgs);

      curX += colLength + padding;
      takenWidth += colLength;

      LogIf(takenWidth > availableWidth,
            LogAppend("ERROR: Button grid %d column takes more than the "
                      "available width!\n",
                      j + 1));
    }

    curY += rowLength + padding;
    takenHeight += rowLength;

    LogIf(
        takenHeight > availableHeight,
        LogAppend(
            "ERROR: Button grid %d row takes more than the available height!\n",
            i + 1));
  }
}

static void DrawTextField(int x,
                          int y,
                          int width,
                          int height,
                          Color textColor,
                          Color backgroundColor,
                          Color borderColor,
                          int fontSize,
                          int BorderThickness,
                          char* buffer)
{
  size_t nameLength = strlen(buffer);
  char displayName[nameLength + 2];
  snprintf(displayName, nameLength + 2, "%s|", buffer);

  DrawTextBox(x, y, width, height, displayName, fontSize, textColor,
              backgroundColor, borderColor, BorderThickness, NO_SHADOW);

  if (IsKeyPressed(KEY_BACKSPACE))
  {
    buffer[nameLength - 1] = '\0';
    ButtonPressedTime = 0;
    KeyRepeatInterval = INITIAL_REPEAT_INTERVAL;
  }
  else if (IsKeyDown(KEY_BACKSPACE))
  {
    ButtonPressedTime += GetFrameTime();

    if (ButtonPressedTime >= KeyRepeatInterval)
    {
      buffer[nameLength - 1] = '\0';
      ButtonPressedTime = 0;
      KeyRepeatInterval = fmax(KeyRepeatInterval * INITIAL_REPEAT_INTERVAL,
                               MIN_REPEAT_INTERVAL);
    }
  }
  else
  {
    ButtonPressedTime += GetFrameTime();

    if (ButtonPressedTime >= KeyRepeatInterval)
    {
      int keycode = GetCharPressed();

      if (keycode == ' ' || isalnum(keycode))
      {
        buffer[nameLength] = tolower(keycode);
        buffer[nameLength + 1] = '\0';

        ButtonPressedTime = 0;
        KeyRepeatInterval = fmax(KeyRepeatInterval * INITIAL_REPEAT_INTERVAL,
                                 MIN_REPEAT_INTERVAL);
      }
    }
  }
}

#endif  // PICKLE_DRAW

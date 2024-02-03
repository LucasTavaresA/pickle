#include <math.h>

#include "../raylib/src/raylib.h"

#include "globals.c"
#include "log.c"

#define IsPointInsideRect(x, y, recX, recY, recWidth, recHeight) \
  (x >= recX && x <= recX + recWidth && y >= recY && y <= recY + recHeight)

// picks between black and white depending on the passed color
#define GetContrastedTextColor(color) \
  ((color.r + color.g + color.b) / 3 < CONTRAST_LIMIT) ? WHITE : BLACK

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
  float Distance;
  Color Color;
  ShadowKind Kind;
} ShadowStyle;

typedef struct
{
  Color TextColor;
  Color BackgroundColor;
  Color PressedColor;
  Color HoveredColor;
  Color BorderColor;
  float BorderThickness;
  ShadowStyle ShadowStyle;
} ButtonStyle;

typedef struct
{
  float WidthPercentage;
  char* Text;
  ButtonStyle Style;
} Button;

typedef struct
{
  float HeightPercentage;
  int ButtonCount;
  Button Buttons[];
} ButtonRow;

static void DrawShadow(float x,
                       float y,
                       float width,
                       float height,
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

static void DrawTextBox(float x,
                        float y,
                        float width,
                        float height,
                        char* text,
                        Color textColor,
                        float fontSize,
                        Color backgroundColor,
                        float borderThickness,
                        Color borderColor,
                        ShadowStyle shadowStyle)
{
  LogIfBadContrast(
      backgroundColor, textColor,
      "ERROR: The text at the %.0f,%.0f text box is not visible!\n", x, y);

  Vector2 textSize = MeasureTextEx(Fonte, text, fontSize, TEXT_SPACING);

  LogIfTrue(textSize.x > width || textSize.y > height,
            "ERROR: The text at the %.0f,%.0f text box does not fit its box!\n",
            x, y);

  float textX = x + ((width - textSize.x) / 2);
  float textY = y + ((height - textSize.y) / 2);

  Rectangle rec = {x, y, width, height};

  if (shadowStyle.Distance > 0)
  {
    DrawShadow(x, y, width, height, shadowStyle, borderColor);
  }

  DrawRectangleRec(rec, backgroundColor);

  if (borderThickness > 0)
  {
    DrawRectangleLinesEx(rec, borderThickness, borderColor);
  }

  DrawTextEx(Fonte, text, (Vector2){textX, textY}, fontSize, 2, textColor);
}

static void DrawRectangleGrid(float x,
                              float y,
                              float width,
                              float height,
                              float padding,
                              Row* rows,
                              int rows_amount)
{
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
                      const Slice* slices,
                      int slice_amount)
{
  Vector2 center = {(float)ScreenWidth / 2, (float)ScreenHeight / 2};
  float sectionSize = 360.0f / slice_amount;
  float startAngle = angle;
  float endAngle = sectionSize + angle;

  for (int i = 0; i < slice_amount; i++)
  {
    DrawCircleSector(center, radius, startAngle, endAngle, 0, Colors[slices[i].Color]);

    float midAngle = ((startAngle + endAngle) / 2) - FontSize / 8;
    Vector2 labelPosition = {
        center.x + (radius / 2) * cosf(midAngle * DEG2RAD),
        center.y + (radius / 2) * sinf(midAngle * DEG2RAD)};

    DrawTextPro(Fonte, slices[i].Name, labelPosition, (Vector2){0.5f, 0.5f},
                midAngle, FontSize / 1.2, TEXT_SPACING,
                GetContrastedTextColor(Colors[i]));

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

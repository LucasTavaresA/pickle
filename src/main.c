#include "../raylib/src/raylib.h"

#include "char.c"
#include "draw.c"
#include "globals.c"
#include "log.c"

typedef struct
{
  int SliceIndex;
} ColorPickArgs;

static void ColorPickFunc(int buttonRow, int buttonColumn, void* _args)
{
  ColorPickArgs* args = (ColorPickArgs*)_args;
  Slices[args->SliceIndex].Color =
      buttonRow * (COLORS_AMOUNT / PALETTE_ROW_AMOUNT) + buttonColumn;
}

static ShadowStyle NoShadow = {0};

int main()
{
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
  InitWindow(ScreenWidth, ScreenHeight, APP_NAME);
  SetExitKey(0);

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
      if (IsKeyPressed(KEY_ESCAPE))
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
      FontSize = (ScreenWidth < ScreenHeight ? ScreenWidth : ScreenHeight) / 25;
      Padding = ScreenWidth / 250;
    }

    // update mouse and touch information
    {
      MouseX = GetMouseX();
      MouseY = GetMouseY();
      TouchCount = GetTouchPointCount();
      ButtonWasPressed = false;
      Clicked = false;

      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
      {
        MousePressedX = MouseX;
        MousePressedY = MouseY;
        Clicked = true;
      }
    }

    // Draw
    {
      BeginDrawing();
      ClearBackground(BACKGROUND_COLOR);

      int squareButtonSize =
          (ScreenWidth < ScreenHeight ? ScreenWidth : ScreenHeight) / 8;
      Rectangle cornerButtonRect = {ScreenWidth - squareButtonSize - Padding,
                                    Padding, squareButtonSize,
                                    squareButtonSize};

      if (!MenuOpened)
      {
        // Draw the wheel
        {
          int wheelRadius =
              (ScreenWidth < ScreenHeight ? ScreenWidth : ScreenHeight) / 2 -
              ScreenPadding;

          if (SlicesCount == 0)
          {
            DrawWheel(0, wheelRadius, DefaultSlices, COLORS_AMOUNT);
          }
          else
          {
            DrawWheel(0, wheelRadius, Slices, SlicesCount);
          }
        }

        // Draw a menu button
        {
          if (IsPointInsideRect(MouseX, MouseY, cornerButtonRect.x,
                                cornerButtonRect.y, cornerButtonRect.width,
                                cornerButtonRect.height))
          {
            if (IsCursorOnScreen() || TouchCount > 0)
            {
              // button background
              DrawRectangleRec(cornerButtonRect, HIGHLIGHT_COLOR);
            }

            if (!ButtonWasPressed && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) &&
                IsPointInsideRect(MousePressedX, MousePressedY,
                                  cornerButtonRect.x, cornerButtonRect.y,
                                  cornerButtonRect.width,
                                  cornerButtonRect.height))
            {
              ButtonWasPressed = true;
              MenuOpened = true;
            }
          }

          // button outline
          DrawRectangleLinesEx(cornerButtonRect, Padding, FOREGROUND_COLOR);

          // Draw a menu icon
          Row rows[] = {
              {33, 1,
               (Column[]){(Column){.Width = 100, .Color = FOREGROUND_COLOR}}},
              {33, 1,
               (Column[]){(Column){.Width = 100, .Color = FOREGROUND_COLOR}}},
              {33, 1,
               (Column[]){(Column){.Width = 100, .Color = FOREGROUND_COLOR}}},
          };

          DrawRectangleGrid(cornerButtonRect.x + ((float)squareButtonSize / 8),
                            cornerButtonRect.y + ((float)squareButtonSize / 8),
                            squareButtonSize * 3 / 4, squareButtonSize * 3 / 4,
                            Padding, rows, ARRAY_LENGTH(rows));
        }
      }
      else
      {
        // draw the slices menu
        {
          int sidePadding =
              (ScreenWidth < ScreenHeight ? 0 : ScreenPadding * 8);
          int sliceEntryHeight =
              (ScreenWidth < ScreenHeight ? ScreenHeight / 6
                                          : ScreenHeight / 3);
          int sliceEntryWidth = ScreenWidth - sidePadding * 2;
          int sliceEntryBorder = ScreenWidth / 500;

          // draw a button to add a slice
          if (SlicesCount == 0 || SlicesCount < COLORS_AMOUNT)
          {
            Rectangle addButtonRect = {
                sidePadding,
                (ScreenWidth < ScreenHeight ? squareButtonSize + Padding : 0) +
                    sliceEntryHeight * SlicesCount,
                sliceEntryWidth, sliceEntryHeight};

            if (IsPointInsideRect(MouseX, MouseY, addButtonRect.x,
                                  addButtonRect.y, addButtonRect.width,
                                  addButtonRect.height))
            {
              if (IsCursorOnScreen() || TouchCount > 0)
              {
                // button background
                DrawRectangleRec(addButtonRect, HIGHLIGHT_COLOR);
              }

              if (!ButtonWasPressed &&
                  IsMouseButtonReleased(MOUSE_BUTTON_LEFT) &&
                  IsPointInsideRect(MousePressedX, MousePressedY,
                                    addButtonRect.x, addButtonRect.y,
                                    addButtonRect.width, addButtonRect.height))
              {
                ButtonWasPressed = true;
                Slices[SlicesCount].Name =
                    strdup(DefaultSlices[SlicesCount].Name);
                Slices[SlicesCount].Color = DefaultSlices[SlicesCount].Color;
                SlicesCount++;
              }
            }

            // button outline
            DrawRectangleLinesEx(addButtonRect, sliceEntryBorder,
                                 FOREGROUND_COLOR);

            // plus sign
            DrawCross(addButtonRect.x + addButtonRect.width / 2,
                      addButtonRect.y + addButtonRect.height / 2, 0,
                      (float)sliceEntryHeight / 2, sliceEntryHeight / 10,
                      FOREGROUND_COLOR);
          }

          for (int i = 0; i < SlicesCount; i++)
          {
            Vector2 sliceNameTextSize = MeasureTextEx(
                Fonte, Slices[i].Name, FontSize * 2, TEXT_SPACING);

            Rectangle sliceEntryRect = {
                sidePadding,
                (ScreenWidth < ScreenHeight ? squareButtonSize + Padding : 0) +
                    i * sliceEntryHeight,
                sliceEntryWidth, sliceEntryHeight};

            // slice entry outline
            DrawRectangleLinesEx(sliceEntryRect, sliceEntryBorder,
                                 FOREGROUND_COLOR);

            // draw color palette
            {
              ButtonRow palette[PALETTE_ROW_AMOUNT] = {
                  {PALETTE_ROW_PERCENTAGE, PALETTE_COL_AMOUNT,
                   (Button[PALETTE_COL_AMOUNT]){{0}}},
                  {PALETTE_ROW_PERCENTAGE, PALETTE_COL_AMOUNT,
                   (Button[PALETTE_COL_AMOUNT]){{0}}}};
              int paletteX = sidePadding + Padding;
              int paletteY =
                  i * sliceEntryHeight + sliceNameTextSize.y + Padding * 2 +
                  (ScreenWidth < ScreenHeight ? squareButtonSize + Padding : 0);
              int paletteHeight =
                  sliceEntryHeight - sliceNameTextSize.y - Padding * 4;
              int paletteWidth = sliceEntryWidth - Padding - squareButtonSize;

              for (int c = 0; c < COLORS_AMOUNT; c++)
              {
                palette[c / (COLORS_AMOUNT / PALETTE_ROW_AMOUNT)]
                    .Columns[c % (COLORS_AMOUNT / PALETTE_ROW_AMOUNT)] =
                    (Button){PALETTE_COL_PERCENTAGE,
                             "",
                             FontSize,
                             FOREGROUND_COLOR,
                             Colors[c],
                             Colors[c],
                             Colors[c],
                             c == Slices[i].Color ? FOREGROUND_COLOR
                                                  : HIGHLIGHT_COLOR,
                             Padding,
                             NoShadow,
                             ColorPickFunc,
                             &(ColorPickArgs){i}};
              }

              DrawButtonGrid(paletteX, paletteY, paletteWidth, paletteHeight,
                             Padding, palette, PALETTE_ROW_AMOUNT);
            }

            // TODO(LucasTA):
            //   make android keyboard
            //   separate into function DrawTextField()
            // draw editable text box
            {
              Rectangle sliceTextFieldRect = {
                  sidePadding + Padding,
                  (ScreenWidth < ScreenHeight ? squareButtonSize + Padding
                                              : 0) +
                      i * sliceEntryHeight + Padding,
                  fmax(sliceNameTextSize.x, squareButtonSize) + Padding,
                  sliceNameTextSize.y};

              if (TypingIndex == i)
              {
                size_t nameLength = strlen(Slices[i].Name);
                char displayName[nameLength + 2];
                snprintf(displayName, nameLength + 2, "%s|", Slices[i].Name);

                DrawTextBox(sliceTextFieldRect.x, sliceTextFieldRect.y,
                            sliceTextFieldRect.width, sliceTextFieldRect.height,
                            displayName, FontSize, FOREGROUND_COLOR,
                            HIGHLIGHT_COLOR, FOREGROUND_COLOR, sliceEntryBorder,
                            NoShadow);

                if (IsKeyPressed(KEY_BACKSPACE))
                {
                  Slices[i].Name[nameLength - 1] = '\0';
                  ButtonPressedTime = 0;
                  KeyRepeatInterval = INITIAL_REPEAT_INTERVAL;
                }
                else if (IsKeyDown(KEY_BACKSPACE))
                {
                  ButtonPressedTime += GetFrameTime();

                  if (ButtonPressedTime >= KeyRepeatInterval)
                  {
                    Slices[i].Name[nameLength - 1] = '\0';
                    ButtonPressedTime = 0;
                    // Decrease repeat interval gradually down to the minimum
                    KeyRepeatInterval =
                        fmax(KeyRepeatInterval * 0.3f, MIN_REPEAT_INTERVAL);
                  }
                }
                else
                {
                  ButtonPressedTime += GetFrameTime();

                  if (ButtonPressedTime >= KeyRepeatInterval &&
                      sliceTextFieldRect.width <
                          sliceEntryRect.width - squareButtonSize * 2)
                  {
                    int keycode = GetCharPressed();

                    if (keycode == ' ' || isalnum(keycode))
                    {
                      Slices[i].Name[nameLength] = tolower(keycode);
                      Slices[i].Name[nameLength + 1] = '\0';

                      ButtonPressedTime = 0;
                      KeyRepeatInterval =
                          fmax(KeyRepeatInterval * 0.2f, MIN_REPEAT_INTERVAL);
                    }
                  }
                }
              }
              else if (IsPointInsideRect(MouseX, MouseY, sliceTextFieldRect.x,
                                         sliceTextFieldRect.y,
                                         sliceTextFieldRect.width,
                                         sliceTextFieldRect.height))
              {
                if (IsCursorOnScreen() || TouchCount > 0)
                {
                  DrawTextBox(sliceTextFieldRect.x, sliceTextFieldRect.y,
                              sliceTextFieldRect.width,
                              sliceTextFieldRect.height, Slices[i].Name,
                              FontSize, FOREGROUND_COLOR, HIGHLIGHT_COLOR,
                              HIGHLIGHT_COLOR, sliceEntryBorder, NoShadow);
                }

                if (!ButtonWasPressed &&
                    IsMouseButtonReleased(MOUSE_BUTTON_LEFT) &&
                    IsPointInsideRect(
                        MousePressedX, MousePressedY, sliceTextFieldRect.x,
                        sliceTextFieldRect.y, sliceTextFieldRect.width,
                        sliceTextFieldRect.height))
                {
                  ButtonWasPressed = true;
                  TypingIndex = i;
                }
              }
              else
              {
                DrawTextBox(sliceTextFieldRect.x, sliceTextFieldRect.y,
                            sliceTextFieldRect.width, sliceTextFieldRect.height,
                            Slices[i].Name, FontSize, FOREGROUND_COLOR,
                            BACKGROUND_COLOR, HIGHLIGHT_COLOR, sliceEntryBorder,
                            NoShadow);
              }
            }

            // draw a trash button to delete slices
            {
              Rectangle trashButtonRect = {
                  ScreenWidth - sidePadding - squareButtonSize - Padding,
                  (ScreenWidth < ScreenHeight ? squareButtonSize + Padding
                                              : 0) +
                      i * sliceEntryHeight + Padding,
                  squareButtonSize, squareButtonSize};

              if (IsPointInsideRect(MouseX, MouseY, trashButtonRect.x,
                                    trashButtonRect.y, trashButtonRect.width,
                                    trashButtonRect.height))
              {
                if (IsCursorOnScreen() || TouchCount > 0)
                {
                  // button background
                  DrawRectangleRec(trashButtonRect, RED_HIGHLIGHT_COLOR);
                }

                if (!ButtonWasPressed &&
                    IsMouseButtonReleased(MOUSE_BUTTON_LEFT) &&
                    IsPointInsideRect(MousePressedX, MousePressedY,
                                      trashButtonRect.x, trashButtonRect.y,
                                      trashButtonRect.width,
                                      trashButtonRect.height))
                {
                  ButtonWasPressed = true;
                  // remove entry and rearrange list
                  memmove(&Slices[i], &Slices[i + 1],
                          (SlicesCount - i - 1) * sizeof(Slice));
                  SlicesCount--;
                }
              }

              // button outline
              DrawRectangleLinesEx(trashButtonRect, sliceEntryBorder, RED);

              // draw a trash icon
              {
                int trashButtonLidHandleSize = Padding * 4;

                // trash can bucket
                DrawRectangleRounded(
                    (Rectangle){trashButtonRect.x + trashButtonRect.width / 4,
                                trashButtonRect.y + trashButtonRect.height / 4,
                                (float)squareButtonSize / 2,
                                (float)squareButtonSize / 2},
                    ROUNDNESS, 0, RED);

                // trash can bucket shape
                DrawRectangleGrid(
                    trashButtonRect.x + trashButtonRect.width / 4 + Padding,
                    trashButtonRect.y + trashButtonRect.height / 4 + Padding,
                    squareButtonSize / 2 - Padding * 2,
                    squareButtonSize / 2 - Padding * 2, 3,
                    (Row[]){
                        100, 3,
                        (Column[]){
                            (Column){.Width = 33, .Color = RED_HIGHLIGHT_COLOR},
                            (Column){.Width = 33, .Color = RED_HIGHLIGHT_COLOR},
                            (Column){.Width = 33, .Color = RED_HIGHLIGHT_COLOR}}},
                    1);

                // trash can handle
                DrawRectangleRec(
                    (Rectangle){trashButtonRect.x + trashButtonRect.width / 2 -
                                    (float)trashButtonLidHandleSize / 2,
                                trashButtonRect.y + trashButtonRect.height / 6,
                                trashButtonLidHandleSize,
                                trashButtonLidHandleSize},
                    RED);

                // trash can lid
                DrawRectangleRec(
                    (Rectangle){trashButtonRect.x + trashButtonRect.width / 4 -
                                    (float)trashButtonLidHandleSize / 4,
                                trashButtonRect.y + trashButtonRect.height / 4,
                                (float)squareButtonSize / 2 +
                                    (float)trashButtonLidHandleSize / 2,
                                (float)squareButtonSize / 6},
                    RED);
              }
            }
          }
        }

        // Draw a close button
        {
          if (IsPointInsideRect(MouseX, MouseY, cornerButtonRect.x,
                                cornerButtonRect.y, cornerButtonRect.width,
                                cornerButtonRect.height))
          {
            if (IsCursorOnScreen() || TouchCount > 0)
            {
              // button background
              DrawRectangleRec(cornerButtonRect, RED_HIGHLIGHT_COLOR);
            }

            if (!ButtonWasPressed && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) &&
                IsPointInsideRect(MousePressedX, MousePressedY,
                                  cornerButtonRect.x, cornerButtonRect.y,
                                  cornerButtonRect.width,
                                  cornerButtonRect.height))
            {
              ButtonWasPressed = true;
              MenuOpened = false;
            }
          }

          // button outline
          DrawRectangleLinesEx(cornerButtonRect, Padding, RED);

          // x sign
          DrawCross(cornerButtonRect.x + cornerButtonRect.width / 2,
                    cornerButtonRect.y + cornerButtonRect.height / 2, 45,
                    squareButtonSize, squareButtonSize / 8, RED);
        }
      }

      if (Clicked)
      {
        TypingIndex = -1;
      }

      LogAppend("INFO(Mouse): X %d Y %d PressedX %d PressedY %d \n", MouseX,
                MouseY, MousePressedX, MousePressedY);
      LogDraw();
      LogSet("");

      EndDrawing();
    }
  }

  CloseWindow();
  return 0;
}

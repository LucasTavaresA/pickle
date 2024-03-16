#include <ctype.h>

#include "../raylib/src/raylib.h"

#include "draw.c"
#include "globals.c"
#include "log.c"

typedef struct
{
  int SliceIndex;
} ColorPickArgs;

void ColorPick(int buttonRow, int buttonColumn, void* _args)
{
  ColorPickArgs* args = (ColorPickArgs*)_args;
  Slices[args->SliceIndex].Color =
      buttonRow * (COLORS_AMOUNT / PALETTE_ROW_AMOUNT) + buttonColumn;
}

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

      int square_button_size =
          (ScreenWidth < ScreenHeight ? ScreenWidth : ScreenHeight) / 8;
      Rectangle corner_button_rect = {
          ScreenWidth - square_button_size - Padding,
          Padding, square_button_size, square_button_size};

      if (!MenuOpened)
      {
        // Draw the wheel
        {
          int wheel_radius =
              (ScreenWidth < ScreenHeight ? ScreenWidth : ScreenHeight) / 2 -
              ScreenPadding;

          if (SlicesCount == 0)
          {
            DrawWheel(0, wheel_radius, DefaultSlices, COLORS_AMOUNT);
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
            if (IsCursorOnScreen() || TouchCount > 0)
            {
              // button background
              DrawRectangleRec(corner_button_rect, HIGHLIGHT_COLOR);
            }

            if (!ButtonWasPressed && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) &&
                IsPointInsideRect(MousePressedX, MousePressedY,
                                  corner_button_rect.x, corner_button_rect.y,
                                  corner_button_rect.width,
                                  corner_button_rect.height))
            {
              ButtonWasPressed = true;
              MenuOpened = true;
            }
          }

          // button outline
          DrawRectangleLinesEx(corner_button_rect, Padding,
                               FOREGROUND_COLOR);

          // Draw a menu icon
          Row rows[] = {
              {33, 1,
               (Column[]){(Column){.Width = 100, .Color = FOREGROUND_COLOR}}},
              {33, 1,
               (Column[]){(Column){.Width = 100, .Color = FOREGROUND_COLOR}}},
              {33, 1,
               (Column[]){(Column){.Width = 100, .Color = FOREGROUND_COLOR}}},
          };

          DrawRectangleGrid(
              corner_button_rect.x + ((float)square_button_size / 8),
              corner_button_rect.y + ((float)square_button_size / 8),
              square_button_size * 3 / 4, square_button_size * 3 / 4,
              Padding, rows, ARRAY_LENGTH(rows));
        }
      }
      else
      {
        // draw the slices menu
        {
          int side_padding =
              (ScreenWidth < ScreenHeight ? 0 : ScreenPadding * 8);
          int slice_item_height =
              (ScreenWidth < ScreenHeight ? ScreenHeight / 6
                                          : ScreenHeight / 3);
          int slice_item_width = ScreenWidth - side_padding * 2;
          int slice_item_border = ScreenWidth / 500;

          // draw a button to add a slice
          if (SlicesCount == 0 || SlicesCount < COLORS_AMOUNT)
          {
            Rectangle add_button = {
                side_padding,
                (ScreenWidth < ScreenHeight
                     ? square_button_size + Padding
                     : 0) +
                    slice_item_height * SlicesCount,
                slice_item_width, slice_item_height};

            if (IsPointInsideRect(MouseX, MouseY, add_button.x, add_button.y,
                                  add_button.width, add_button.height))
            {
              if (IsCursorOnScreen() || TouchCount > 0)
              {
                // button background
                DrawRectangleRec(add_button, HIGHLIGHT_COLOR);
              }

              if (!ButtonWasPressed &&
                  IsMouseButtonReleased(MOUSE_BUTTON_LEFT) &&
                  IsPointInsideRect(MousePressedX, MousePressedY, add_button.x,
                                    add_button.y, add_button.width,
                                    add_button.height))
              {
                ButtonWasPressed = true;
                Slices[SlicesCount].Name =
                    strdup(DefaultSlices[SlicesCount].Name);
                Slices[SlicesCount].Color = DefaultSlices[SlicesCount].Color;
                SlicesCount++;
              }
            }

            // button outline
            DrawRectangleLinesEx(add_button, slice_item_border,
                                 FOREGROUND_COLOR);

            // plus sign
            DrawCross(add_button.x + add_button.width / 2,
                      add_button.y + add_button.height / 2, 0,
                      (float)slice_item_height / 2, slice_item_height / 10,
                      FOREGROUND_COLOR);
          }

          for (int i = 0; i < SlicesCount; i++)
          {
            Rectangle item_rect = {
                side_padding,
                (ScreenWidth < ScreenHeight
                     ? square_button_size + Padding
                     : 0) +
                    i * slice_item_height,
                slice_item_width, slice_item_height};

            // slice item outline
            DrawRectangleLinesEx(item_rect, slice_item_border,
                                 FOREGROUND_COLOR);

            // draw color palette
            {
              ButtonRow palette[PALETTE_ROW_AMOUNT] = {
                  {PALETTE_ROW_PERCENTAGE, PALETTE_COL_AMOUNT,
                   (Button[PALETTE_COL_AMOUNT]){0}},
                  {PALETTE_ROW_PERCENTAGE, PALETTE_COL_AMOUNT,
                   (Button[PALETTE_COL_AMOUNT]){0}},
              };
              int palette_x = side_padding + Padding;
              int palette_y = i * slice_item_height + slice_item_height / 2 -
                              Padding +
                              (ScreenWidth < ScreenHeight
                                   ? square_button_size + Padding
                                   : 0);
              int palette_height = slice_item_height / 2;
              int palette_width =
                  slice_item_width - Padding * 2 -
                  (ScreenWidth < ScreenHeight ? square_button_size : 0);

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
                             (ShadowStyle){0},
                             ColorPick,
                             &(ColorPickArgs){i}};
              }

              DrawButtonGrid(palette_x, palette_y, palette_width,
                             palette_height, Padding, palette,
                             PALETTE_ROW_AMOUNT);
            }

            // TODO(LucasTA):
            //   make android keyboard
            //   separate into function DrawTextField()
            // draw editable text box
            {
              Vector2 item_name_text_size = MeasureTextEx(
                  Fonte, Slices[i].Name, FontSize * 2, TEXT_SPACING);

              Rectangle item_name_rect = {
                  side_padding + Padding,
                  (ScreenWidth < ScreenHeight
                       ? square_button_size + Padding
                       : 0) +
                      i * slice_item_height + Padding,
                  fmax(item_name_text_size.x, square_button_size) +
                      Padding,
                  item_name_text_size.y};

              ShadowStyle item_name_shadow = {0};

              if (TypingIndex == i)
              {
                size_t name_length = strlen(Slices[i].Name);
                char display_name[name_length + 2];
                snprintf(display_name, name_length + 2, "%s|", Slices[i].Name);

                DrawTextBox(item_name_rect.x, item_name_rect.y,
                            item_name_rect.width, item_name_rect.height,
                            display_name, FontSize, FOREGROUND_COLOR,
                            HIGHLIGHT_COLOR, FOREGROUND_COLOR,
                            slice_item_border, item_name_shadow);

                if (IsKeyPressed(KEY_BACKSPACE))
                {
                  Slices[i].Name[name_length - 1] = '\0';
                  ButtonPressedTime = 0;
                  KeyRepeatInterval = INITIAL_REPEAT_INTERVAL;
                }
                else if (IsKeyDown(KEY_BACKSPACE))
                {
                  ButtonPressedTime += GetFrameTime();

                  if (ButtonPressedTime >= KeyRepeatInterval)
                  {
                    Slices[i].Name[name_length - 1] = '\0';
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
                      item_name_rect.width <
                          item_rect.width - square_button_size * 2)
                  {
                    int keycode = GetCharPressed();

                    if (keycode == ' ' || isalnum(keycode))
                    {
                      Slices[i].Name[name_length] = tolower(keycode);
                      Slices[i].Name[name_length + 1] = '\0';

                      ButtonPressedTime = 0;
                      KeyRepeatInterval =
                          fmax(KeyRepeatInterval * 0.2f, MIN_REPEAT_INTERVAL);
                    }
                  }
                }
              }
              else if (IsPointInsideRect(MouseX, MouseY, item_name_rect.x,
                                         item_name_rect.y, item_name_rect.width,
                                         item_name_rect.height))
              {
                if (IsCursorOnScreen() || TouchCount > 0)
                {
                  DrawTextBox(item_name_rect.x, item_name_rect.y,
                              item_name_rect.width, item_name_rect.height,
                              Slices[i].Name, FontSize, FOREGROUND_COLOR,
                              HIGHLIGHT_COLOR, HIGHLIGHT_COLOR,
                              slice_item_border, item_name_shadow);
                }

                if (!ButtonWasPressed &&
                    IsMouseButtonReleased(MOUSE_BUTTON_LEFT) &&
                    IsPointInsideRect(MousePressedX, MousePressedY,
                                      item_name_rect.x, item_name_rect.y,
                                      item_name_rect.width,
                                      item_name_rect.height))
                {
                  ButtonWasPressed = true;
                  TypingIndex = i;
                }
              }
              else
              {
                DrawTextBox(item_name_rect.x, item_name_rect.y,
                            item_name_rect.width, item_name_rect.height,
                            Slices[i].Name, FontSize, FOREGROUND_COLOR,
                            BACKGROUND_COLOR, HIGHLIGHT_COLOR,
                            slice_item_border, item_name_shadow);
              }
            }

            // draw a trash button to delete slices
            {
              Rectangle trash_button = {
                  ScreenWidth - side_padding - square_button_size -
                      Padding,
                  (ScreenWidth < ScreenHeight
                       ? square_button_size + Padding
                       : 0) +
                      i * slice_item_height + Padding,
                  square_button_size, square_button_size};

              if (IsPointInsideRect(MouseX, MouseY, trash_button.x,
                                    trash_button.y, trash_button.width,
                                    trash_button.height))
              {
                if (IsCursorOnScreen() || TouchCount > 0)
                {
                  // button background
                  DrawRectangleRec(trash_button, RED_HIGHLIGHT_COLOR);
                }

                if (!ButtonWasPressed &&
                    IsMouseButtonReleased(MOUSE_BUTTON_LEFT) &&
                    IsPointInsideRect(MousePressedX, MousePressedY,
                                      trash_button.x, trash_button.y,
                                      trash_button.width, trash_button.height))
                {
                  ButtonWasPressed = true;
                  // remove item and rearrange list
                  memmove(&Slices[i], &Slices[i + 1],
                          (SlicesCount - i - 1) * sizeof(Slice));
                  SlicesCount--;
                }
              }

              // button outline
              DrawRectangleLinesEx(trash_button, slice_item_border, RED);

              int trash_button_lid_handle_size = Padding * 4;

              // draw a trash icon
              {
                // trash can bucket
                DrawRectangleRounded(
                    (Rectangle){trash_button.x + trash_button.width / 4,
                                trash_button.y + trash_button.height / 4,
                                (float)square_button_size / 2,
                                (float)square_button_size / 2},
                    ROUNDNESS, 0, RED);

                // trash can bucket shape
                DrawRectangleGrid(
                    trash_button.x + trash_button.width / 4 +
                        Padding,
                    trash_button.y + trash_button.height / 4 +
                        Padding,
                    square_button_size / 2 - Padding * 2,
                    square_button_size / 2 - Padding * 2, 3,
                    (Row[]){100, 3,
                            (Column[]){(Column){.Width = 33,
                                                .Color = RED_HIGHLIGHT_COLOR},
                                       (Column){.Width = 33,
                                                .Color = RED_HIGHLIGHT_COLOR},
                                       (Column){.Width = 33,
                                                .Color = RED_HIGHLIGHT_COLOR}}},
                    1);

                // trash can handle
                DrawRectangleRec(
                    (Rectangle){trash_button.x + trash_button.width / 2 -
                                    (float)trash_button_lid_handle_size / 2,
                                trash_button.y + trash_button.height / 6,
                                trash_button_lid_handle_size,
                                trash_button_lid_handle_size},
                    RED);

                // trash can lid
                DrawRectangleRec(
                    (Rectangle){trash_button.x + trash_button.width / 4 -
                                    (float)trash_button_lid_handle_size / 4,
                                trash_button.y + trash_button.height / 4,
                                (float)square_button_size / 2 +
                                    (float)trash_button_lid_handle_size / 2,
                                (float)square_button_size / 6},
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
            if (IsCursorOnScreen() || TouchCount > 0)
            {
              // button background
              DrawRectangleRec(corner_button_rect, RED_HIGHLIGHT_COLOR);
            }

            if (!ButtonWasPressed && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) &&
                IsPointInsideRect(MousePressedX, MousePressedY,
                                  corner_button_rect.x, corner_button_rect.y,
                                  corner_button_rect.width,
                                  corner_button_rect.height))
            {
              ButtonWasPressed = true;
              MenuOpened = false;
            }
          }

          // button outline
          DrawRectangleLinesEx(corner_button_rect, Padding, RED);

          // x sign
          DrawCross(corner_button_rect.x + corner_button_rect.width / 2,
                    corner_button_rect.y + corner_button_rect.height / 2, 45,
                    square_button_size, square_button_size / 8, RED);
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

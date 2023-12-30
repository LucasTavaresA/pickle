#include "../raylib/src/raylib.h"

#include "draw.c"
#include "globals.c"
#include "log.c"

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
            DrawWheel(0, wheel_radius, DefaultSlices, ColorsAmount);
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

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !ButtonWasPressed)
            {
              ButtonWasPressed = true;
              MenuOpened = true;
            }
          }

          // button outline
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
                // button background
                DrawRectangleRec(add_button, HIGHLIGHT_COLOR);
              }

              if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !ButtonWasPressed)
              {
                ButtonWasPressed = true;
                Slices[SlicesCount].Name = DefaultSlices[SlicesCount].Name;
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

            Vector2 item_name_text_size =
                MeasureTextEx(Fonte, Slices[i].Name, FontSize, TEXT_SPACING);

            Rectangle item_name_rect = {
                side_padding + square_button_padding,
                (ScreenWidth < ScreenHeight ? square_button_size : 0) +
                    i * slice_item_height + square_button_padding,
                item_name_text_size.x + square_button_padding,
                item_name_text_size.y + square_button_padding};

            ShadowStyle item_name_shadow = {0};

            // button outline
            DrawRectangleLinesEx(item_rect, slice_item_border,
                                 FOREGROUND_COLOR);

            // draw item name text box
            if (IsPointInsideRect(MouseX, MouseY, item_name_rect.x,
                                  item_name_rect.y, item_name_rect.width,
                                  item_name_rect.height))
            {
              DrawTextBox(item_name_rect.x, item_name_rect.y,
                          item_name_rect.width, item_name_rect.height,
                          Slices[i].Name, FOREGROUND_COLOR, HIGHLIGHT_COLOR,
                          slice_item_border, HIGHLIGHT_COLOR, item_name_shadow);
            }
            else
            {
              DrawTextBox(item_name_rect.x, item_name_rect.y,
                          item_name_rect.width, item_name_rect.height,
                          Slices[i].Name, FOREGROUND_COLOR, BACKGROUND_COLOR,
                          slice_item_border, HIGHLIGHT_COLOR, item_name_shadow);
            }

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
                  // button background
                  DrawRectangleRec(trash_button, RED_HIGHLIGHT_COLOR);
                }

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                    !ButtonWasPressed)
                {
                  ButtonWasPressed = true;
                  SlicesCount--;
                }
              }

              // button outline
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
              // button background
              DrawRectangleRec(corner_button_rect, RED_HIGHLIGHT_COLOR);
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !ButtonWasPressed)
            {
              ButtonWasPressed = true;
              MenuOpened = false;
            }
          }

          // button outline
          DrawRectangleLinesEx(corner_button_rect, square_button_padding, RED);

          // x sign
          DrawCross(corner_button_rect.x + corner_button_rect.width / 2,
                    corner_button_rect.y + corner_button_rect.height / 2, 45,
                    square_button_size, square_button_size / 8, RED);
        }
      }

      LogAppend("INFO(Mouse): X %.0f Y %.0f PressedX %.0f PressedY %.0f \n",
                MouseX, MouseY, MousePressedX, MousePressedY);
      LogDraw();
      LogSet("");

      EndDrawing();
    }
  }

  CloseWindow();
  return 0;
}

// TODO(LucasTA): refactor 'ScreenPadding', 'sliceEntryBorder' and 'Padding'
// into one value for borders and paddings
#include "../raylib/src/raylib.h"

#include "draw.c"
#include "globals.c"
#include "log.c"

#ifdef PLATFORM_ANDROID
typedef struct
{
  char* Buffer;
  int BufferLength;
  char KeyPressed;
  int FieldWidth;
  int MaxFieldWidth;
} KeyboardPressArgs;

void KeyboardPressFunc(int buttonRow, int buttonColumn, void* _args)
{
  KeyboardPressArgs* args = (KeyboardPressArgs*)_args;

  if (args->KeyPressed == '<')
  {
    args->Buffer[args->BufferLength - 1] = '\0';
  }
  else if (args->KeyPressed == '>')
  {
    CurrentScene = SCENE_MENU;
    TypingIndex = -1;
  }
  else if (args->FieldWidth < args->MaxFieldWidth)
  {
    args->Buffer[args->BufferLength] = tolower(args->KeyPressed);
    args->Buffer[args->BufferLength + 1] = '\0';
  }
}

#define KEYBOARD_ROWS 4
#define KEYBOARD_ROW_PERCENTAGE 100 / KEYBOARD_ROWS
#endif

typedef struct
{
  int FieldIndex;
} SelectTextFieldArgs;

static void SelectTextFieldFunc(int buttonRow, int buttonColumn, void* _args)
{
  SelectTextFieldArgs* args = (SelectTextFieldArgs*)_args;
  TypingIndex = args->FieldIndex;
}

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

static void AddEntryFunc(int buttonRow, int buttonColumn, void* _args)
{
  strncpy(Slices[SlicesCount].Name, DEFAULT_SLICES[SlicesCount].Name,
          SLICE_NAME_SIZE);
  Slices[SlicesCount].Color = DEFAULT_SLICES[SlicesCount].Color;
  SlicesCount++;
}

typedef struct
{
  int SliceIndex;
} RemoveEntryArgs;

static void RemoveEntryFunc(int buttonRow, int buttonColumn, void* _args)
{
  RemoveEntryArgs* args = (RemoveEntryArgs*)_args;
  memmove(&Slices[args->SliceIndex], &Slices[args->SliceIndex + 1],
          (SlicesCount - args->SliceIndex - 1) * sizeof(Slice));
  SlicesCount--;
}

static void ToggleMenuFunc(int buttonRow, int buttonColumn, void* _args)
{
  CurrentScene = !CurrentScene;
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

#define X(name) name##Icon = LoadTexture(#name "Icon.png");
  ICON_LIST
#undef X

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
        CurrentScene = !CurrentScene;
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
#ifdef PLATFORM_ANDROID
      TouchCount = GetTouchPointCount();
#endif

      MouseX = GetMouseX();
      MouseY = GetMouseY();
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
      int sidePadding = (ScreenWidth < ScreenHeight ? 0 : ScreenPadding * 8);
      float maxTextFieldWidth =
          ScreenWidth - sidePadding * 2 - squareButtonSize * 2;

      switch (CurrentScene)
      {
        case SCENE_MENU:
        {
          {
            int sliceEntryHeight =
                (ScreenWidth < ScreenHeight ? ScreenHeight / 6
                                            : ScreenHeight / 3);
            int sliceEntryWidth = ScreenWidth - sidePadding * 2;
            int sliceEntryBorder = ScreenWidth / 500;

            // draw a button to add a slice
            if (SlicesCount < COLORS_AMOUNT)
            {
              Rectangle addButtonRect = {
                  sidePadding,
                  (ScreenWidth < ScreenHeight ? squareButtonSize + Padding
                                              : 0) +
                      sliceEntryHeight * SlicesCount,
                  sliceEntryWidth, sliceEntryHeight};

              DrawButton(addButtonRect.x, addButtonRect.y, addButtonRect.width,
                         addButtonRect.height, "", FontSize, false,
                         FOREGROUND_COLOR, BACKGROUND_COLOR, PRESSED_COLOR,
                         HOVERED_COLOR, FOREGROUND_COLOR, Padding, NO_SHADOW,
                         NO_ICON, AddEntryFunc, 0, 0, 0);

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
                  (ScreenWidth < ScreenHeight ? squareButtonSize + Padding
                                              : 0) +
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
                    (ScreenWidth < ScreenHeight ? squareButtonSize + Padding
                                                : 0);
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
                               false,
                               FOREGROUND_COLOR,
                               COLORS[c],
                               COLORS[c],
                               COLORS[c],
                               GetContrastedTextColor(COLORS[c]),
                               c == Slices[i].Color ? Padding : 1,
                               NO_SHADOW,
                               NO_ICON,
                               ColorPickFunc,
                               &(ColorPickArgs){i}};
                }

                DrawButtonGrid(paletteX, paletteY, paletteWidth, paletteHeight,
                               Padding, palette, PALETTE_ROW_AMOUNT);
              }

              // draw editable text box
              {
                Rectangle sliceTextFieldRect = {
                    sidePadding + Padding,
                    (ScreenWidth < ScreenHeight ? squareButtonSize + Padding
                                                : 0) +
                        i * sliceEntryHeight + Padding,
                    fmax(sliceNameTextSize.x, squareButtonSize) + Padding,
                    sliceNameTextSize.y};

                if (TypingIndex != i)
                {
                  DrawButton(
                      sliceTextFieldRect.x, sliceTextFieldRect.y,
                      sliceTextFieldRect.width, sliceTextFieldRect.height,
                      Slices[i].Name, FontSize, false, FOREGROUND_COLOR,
                      BACKGROUND_COLOR, PRESSED_COLOR, HOVERED_COLOR,
                      FOREGROUND_COLOR, sliceEntryBorder, NO_SHADOW, NO_ICON,
                      SelectTextFieldFunc, 0, 0, &(SelectTextFieldArgs){i});
                }
                else
                {
#ifdef PLATFORM_ANDROID
                  CurrentScene = SCENE_KEYBOARD;
#else
                  DrawTextField(sliceTextFieldRect.x, sliceTextFieldRect.y,
                                sliceTextFieldRect.width,
                                sliceTextFieldRect.height, maxTextFieldWidth,
                                FOREGROUND_COLOR, HIGHLIGHT_COLOR,
                                FOREGROUND_COLOR, FontSize, sliceEntryBorder,
                                Slices[i].Name);
#endif
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

                DrawButton(trashButtonRect.x, trashButtonRect.y,
                           trashButtonRect.width, trashButtonRect.height, "",
                           FontSize, false, RED, BACKGROUND_COLOR,
                           RED_PRESSED_COLOR, RED_HOVERED_COLOR, RED, Padding,
                           NO_SHADOW, TrashIcon, RemoveEntryFunc, 0, 0,
                           &(RemoveEntryArgs){i});
              }
            }
          }

          // Draw a close button
          {
            DrawButton(cornerButtonRect.x, cornerButtonRect.y,
                       cornerButtonRect.width, cornerButtonRect.height, "",
                       FontSize, false, RED, BACKGROUND_COLOR,
                       RED_PRESSED_COLOR, RED_HOVERED_COLOR, RED, Padding,
                       NO_SHADOW, NO_ICON, ToggleMenuFunc, 0, 0, 0);

            // x sign
            DrawCross(cornerButtonRect.x + cornerButtonRect.width / 2,
                      cornerButtonRect.y + cornerButtonRect.height / 2, 45,
                      squareButtonSize, squareButtonSize / 8, RED);
          }

          if (Clicked)
          {
            TypingIndex = -1;
          }
        }
        break;
#ifdef PLATFORM_ANDROID
        case SCENE_KEYBOARD:
        {
          Vector2 sliceNameTextSize = MeasureTextEx(
              Fonte, Slices[TypingIndex].Name, FontSize * 2, TEXT_SPACING);
          int keyboardY = ScreenHeight / 1.5;
          int sliceEntryBorder = ScreenWidth / 500;

          Rectangle sliceTextFieldRect = {
              Padding, keyboardY - sliceNameTextSize.y - Padding,
              fmax(sliceNameTextSize.x, squareButtonSize) + Padding,
              sliceNameTextSize.y};

          DrawTextField(sliceTextFieldRect.x, sliceTextFieldRect.y,
                        sliceTextFieldRect.width, sliceTextFieldRect.height,
                        maxTextFieldWidth, FOREGROUND_COLOR, HIGHLIGHT_COLOR,
                        FOREGROUND_COLOR, FontSize, Padding,
                        Slices[TypingIndex].Name);

#define KEYBOARD_BUTTON(key, widthPercentage)                              \
  (Button)                                                                 \
  {                                                                        \
    .WidthPercentage = widthPercentage, .Text = key, .BorderThickness = 1, \
    .RepeatPresses = true, .TextColor = FOREGROUND_COLOR,                  \
    .BackgroundColor = HIGHLIGHT_COLOR, .PressedColor = PRESSED_COLOR,     \
    .HoveredColor = HOVERED_COLOR, .BorderColor = FOREGROUND_COLOR,        \
    .Callback = KeyboardPressFunc, .CallbackArgs = &(KeyboardPressArgs)    \
    {                                                                      \
      Slices[TypingIndex].Name, strlen(Slices[TypingIndex].Name), key[0],  \
          sliceTextFieldRect.width, maxTextFieldWidth                      \
    }                                                                      \
  }
          ButtonRow keyboard[KEYBOARD_ROWS] = {
              {
                  KEYBOARD_ROW_PERCENTAGE,
                  10,
                  (Button[10]){
                      KEYBOARD_BUTTON("q", 100 / 10),
                      KEYBOARD_BUTTON("w", 100 / 10),
                      KEYBOARD_BUTTON("e", 100 / 10),
                      KEYBOARD_BUTTON("r", 100 / 10),
                      KEYBOARD_BUTTON("t", 100 / 10),
                      KEYBOARD_BUTTON("y", 100 / 10),
                      KEYBOARD_BUTTON("u", 100 / 10),
                      KEYBOARD_BUTTON("i", 100 / 10),
                      KEYBOARD_BUTTON("o", 100 / 10),
                      KEYBOARD_BUTTON("p", 100 / 10),
                  },
              },
              {
                  KEYBOARD_ROW_PERCENTAGE,
                  9,
                  (Button[9]){
                      KEYBOARD_BUTTON("a", 100 / 9),
                      KEYBOARD_BUTTON("s", 100 / 9),
                      KEYBOARD_BUTTON("d", 100 / 9),
                      KEYBOARD_BUTTON("f", 100 / 9),
                      KEYBOARD_BUTTON("g", 100 / 9),
                      KEYBOARD_BUTTON("h", 100 / 9),
                      KEYBOARD_BUTTON("j", 100 / 9),
                      KEYBOARD_BUTTON("k", 100 / 9),
                      KEYBOARD_BUTTON("l", 100 / 9),
                  },
              },
              {
                  KEYBOARD_ROW_PERCENTAGE,
                  8,
                  (Button[8]){
                      KEYBOARD_BUTTON("z", 100 / 8),
                      KEYBOARD_BUTTON("x", 100 / 8),
                      KEYBOARD_BUTTON("c", 100 / 8),
                      KEYBOARD_BUTTON("v", 100 / 8),
                      KEYBOARD_BUTTON("b", 100 / 8),
                      KEYBOARD_BUTTON("n", 100 / 8),
                      KEYBOARD_BUTTON("m", 100 / 8),
                      (Button){
                          .WidthPercentage = 100 / 8,
                          .Text = "<-",
                          .BorderThickness = 1,
                          .RepeatPresses = true,
                          .TextColor = RED,
                          .BackgroundColor = HIGHLIGHT_COLOR,
                          .PressedColor = RED_PRESSED_COLOR,
                          .HoveredColor = RED_HOVERED_COLOR,
                          .BorderColor = RED,
                          .Callback = KeyboardPressFunc,
                          .CallbackArgs =
                              &(KeyboardPressArgs){
                                  Slices[TypingIndex].Name,
                                  strlen(Slices[TypingIndex].Name), '<',
                                  sliceTextFieldRect.width, maxTextFieldWidth}},
                  },
              },
              {KEYBOARD_ROW_PERCENTAGE, 3,
               (Button[3]){
                   (Button){100 / 3},
                   KEYBOARD_BUTTON(" ", 100 / 3),
               }},
          };
#undef KEYBOARD_BUTTON

          DrawButtonGrid(0, keyboardY, ScreenWidth, ScreenHeight - keyboardY, 0,
                         keyboard, KEYBOARD_ROWS);

          DrawButton(
              cornerButtonRect.x, sliceTextFieldRect.y, cornerButtonRect.width,
              sliceTextFieldRect.height, ">", FontSize, false, GREEN,
              HIGHLIGHT_COLOR, GREEN_PRESSED_COLOR, GREEN_HOVERED_COLOR, GREEN,
              1, NO_SHADOW, NO_ICON, KeyboardPressFunc, 0, 0,
              &(KeyboardPressArgs){Slices[TypingIndex].Name,
                                   strlen(Slices[TypingIndex].Name), '>'});
        }
        break;
#endif
        default:
          // Draw the wheel
          {
            {
              int wheelRadius =
                  (ScreenWidth < ScreenHeight ? ScreenWidth : ScreenHeight) /
                      2 -
                  ScreenPadding;

              if (SlicesCount == 0)
              {
                DrawWheel(0, wheelRadius, DEFAULT_SLICES, COLORS_AMOUNT);
              }
              else
              {
                DrawWheel(0, wheelRadius, Slices, SlicesCount);
              }
            }

            // Draw a menu button
            {
              DrawButton(cornerButtonRect.x, cornerButtonRect.y,
                         cornerButtonRect.width, cornerButtonRect.height, "",
                         FontSize, false, FOREGROUND_COLOR, BACKGROUND_COLOR,
                         PRESSED_COLOR, HOVERED_COLOR, FOREGROUND_COLOR,
                         Padding, NO_SHADOW, MenuIcon, ToggleMenuFunc, 0, 0, 0);
            }
          }
          break;
      }

      LogAppend("INFO(Mouse): X %d Y %d PressedX %d PressedY %d \n", MouseX,
                MouseY, MousePressedX, MousePressedY);
      LogDraw();
      LogSet("");

      EndDrawing();
    }
  }

#define X(name) UnloadTexture(name##Icon);
  ICON_LIST
#undef X
  CloseWindow();
  return 0;
}

#define FUNCS                                                      \
	FUNC(SelectTextField, int FieldIndex)                            \
	FUNC(ColorPick, int buttonRow, int buttonColumn, int SliceIndex) \
	FUNC(AddEntry)                                                   \
	FUNC(RemoveEntry, int SliceIndex)                                \
	FUNC(ToggleMenu)

#include "../raylib/src/raylib.h"
#include "../assets/iosevka-regular.h"
#include "funcs.c"

#include "draw.c"
#include "globals.c"
#include "log.c"
#include "math.c"

#ifdef PLATFORM_ANDROID
#	include <android_native_app_glue.h>
#	include <jni.h>

#	define ANDROID_INPUT_BUFFER_SIZE 1024
static char androidInputBuffer[ANDROID_INPUT_BUFFER_SIZE];
static int androidInputHead = 0;
static int androidInputTail = 0;
static bool androidDeletePressed = false;

// called from JNI when it uses override below
void AddAndroidInput(char c)
{
	int nextHead = (androidInputHead + 1) % ANDROID_INPUT_BUFFER_SIZE;
	if (nextHead != androidInputTail)
	{
		androidInputBuffer[androidInputHead] = c;
		androidInputHead = nextHead;
	}
}

// overriding JNI, called from Java
JNIEXPORT void JNICALL
Java_com_lucasta_pickle_NativeLoader_nativeOnTextInput(JNIEnv* env,
																											 jobject obj,
																											 jstring text)
{
	const char* str = (*env)->GetStringUTFChars(env, text, NULL);
	if (str != NULL)
	{
		for (int i = 0; str[i] != '\0'; i++)
		{
			AddAndroidInput(str[i]);
		}
	}
	(*env)->ReleaseStringUTFChars(env, text, str);
}

// overriding JNI, called from Java
JNIEXPORT void JNICALL
Java_com_lucasta_pickle_NativeLoader_nativeOnKeyDelete(JNIEnv* env, jobject obj)
{
	androidDeletePressed = true;
}

// "normal" c functions
char GetAndroidInput(void)
{
	if (androidInputHead == androidInputTail)
		return 0;
	char c = androidInputBuffer[androidInputTail];
	androidInputTail = (androidInputTail + 1) % ANDROID_INPUT_BUFFER_SIZE;
	return c;
}

void ShowAndroidKeyboard(void)
{
	extern struct android_app* GetAndroidApp(void);
	struct android_app* app = GetAndroidApp();
	if (app == NULL)
		return;

	JNIEnv* env = NULL;
	JavaVM* vm = app->activity->vm;
	(*vm)->AttachCurrentThread(vm, &env, NULL);

	jclass activityClass = (*env)->GetObjectClass(env, app->activity->clazz);
	jmethodID showKeyboard =
			(*env)->GetMethodID(env, activityClass, "showSoftKeyboard", "()V");
	if (showKeyboard != NULL)
	{
		(*env)->CallVoidMethod(env, app->activity->clazz, showKeyboard);
	}

	(*vm)->DetachCurrentThread(vm);
}

void HideAndroidKeyboard(void)
{
	extern struct android_app* GetAndroidApp(void);
	struct android_app* app = GetAndroidApp();
	if (app == NULL)
		return;

	JNIEnv* env = NULL;
	JavaVM* vm = app->activity->vm;
	(*vm)->AttachCurrentThread(vm, &env, NULL);

	jclass activityClass = (*env)->GetObjectClass(env, app->activity->clazz);
	jmethodID hideKeyboard =
			(*env)->GetMethodID(env, activityClass, "hideSoftKeyboard", "()V");
	if (hideKeyboard != NULL)
	{
		(*env)->CallVoidMethod(env, app->activity->clazz, hideKeyboard);
	}

	(*vm)->DetachCurrentThread(vm);
}
#endif

static void SelectTextFieldFunc(SelectTextFieldArgs args)
{
	TypingIndex = args.FieldIndex;
}

static void ColorPickFunc(ColorPickArgs args)
{
	Slices[args.SliceIndex].Color =
			args.buttonRow * (COLORS_AMOUNT / PALETTE_ROW_AMOUNT) + args.buttonColumn;
}

static void AddEntryFunc()
{
	strncpy(Slices[SlicesCount].Name, DEFAULT_SLICES[SlicesCount].Name,
					SLICE_NAME_SIZE);
	Slices[SlicesCount].Color = DEFAULT_SLICES[SlicesCount].Color;
	SlicesCount++;
}

static void RemoveEntryFunc(RemoveEntryArgs args)
{
	memmove(&Slices[args.SliceIndex], &Slices[args.SliceIndex + 1],
					(SlicesCount - args.SliceIndex - 1) * sizeof(Slice));
	SlicesCount--;
}

static void ToggleMenuFunc()
{
	CurrentScene = !CurrentScene;
}

#define DRAW_BUTTON(x, y, width, height, text, fontSize, repeatPresses,     \
										textColor, backgroundColor, pressedColor, hoveredColor, \
										borderColor, borderThickness, shadowStyle, icon, Name,  \
										argsPtr)                                                \
	DrawButton(x, y, width, height, text, fontSize, repeatPresses, textColor, \
						 backgroundColor, pressedColor, hoveredColor, borderColor,      \
						 borderThickness, shadowStyle, icon, Name##Wrapper, argsPtr)

int main()
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI |
								 FLAG_MSAA_4X_HINT);
#ifdef PLATFORM_WINDOWS
	InitWindow(800, 600, APP_NAME);
#else
	InitWindow(ScreenWidth, ScreenHeight, APP_NAME);
#endif
	SetExitKey(KEY_NULL);

#ifndef PLATFORM_ANDROID
	ChangeDirectory("assets");
#endif

	Fonte = LoadFont_Iosevka();

#define X(Name, NAME)                                          \
	Texture2D Name##Texture = LoadTextureFromImage(Name##Image); \
	SetTextureFilter(Name##Texture, TEXTURE_FILTER_POINT);
	ICON_LIST
#undef X

	ImageFormat(&WindowImage, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
	SetWindowIcon(WindowImage);

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
			FontSize = (ScreenWidth + ScreenHeight) / 70;
			Border = fmax((float)ScreenWidth / 500, 1);
			Padding = Border * 2;
			DeltaTime = GetFrameTime();
		}

		// update mouse and touch information
		{
#ifdef PLATFORM_ANDROID
			TouchCount = GetTouchPointCount();
#else
			MouseScroll = GetMouseWheelMove() * 32;
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
			int sidePadding = squareButtonSize + Padding;
			int menuX = (ScreenWidth < ScreenHeight ? 0 : squareButtonSize + Padding);
			int menuEntryWidth =
					ScreenWidth -
					(ScreenWidth < ScreenHeight ? sidePadding : sidePadding * 2);
			int menuVisibleEntries = (ScreenWidth < ScreenHeight ? 6 : 3);
			int menuEntryHeight = ScreenHeight / menuVisibleEntries;

			switch (CurrentScene)
			{
				case SCENE_MENU:
				{
					{
#ifdef PLATFORM_ANDROID
						if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
						{
							StartTouchPosition = GetTouchPosition(0);
						}

						if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
						{
							Vector2 currentTouchPosition = GetTouchPosition(0);

							float touchMoveDistance =
									fabsf(currentTouchPosition.y - StartTouchPosition.y);

							if (touchMoveDistance > 2 &&
									// NOTE(LucasTA): Don't scroll if touch starts outside the
									// list
									IsPointInsideRect(MousePressedX, MousePressedY, 0, 0,
																		ScreenWidth - cornerButtonRect.width,
																		ScreenHeight))
							{
								Dragging = true;
								MenuScrollOffset =
										clamp(MenuScrollOffset + currentTouchPosition.y -
															StartTouchPosition.y,
													clamp(SlicesCount - (menuVisibleEntries - 1), 0,
																COLORS_AMOUNT - menuVisibleEntries) *
															-menuEntryHeight,
													0);
								StartTouchPosition = currentTouchPosition;
							}
						}
#else
						if (MouseScroll > 0)
						{
							Dragging = true;
						}

						MenuScrollOffset =
								clamp(MenuScrollOffset + MouseScroll,
											clamp(SlicesCount - (menuVisibleEntries - 1), 0,
														COLORS_AMOUNT - menuVisibleEntries) *
													-menuEntryHeight,
											0);
#endif

						// draw a button to add a slice
						if (SlicesCount < COLORS_AMOUNT)
						{
							int addButtonY = MenuScrollOffset + menuEntryHeight * SlicesCount;

							DRAW_BUTTON(menuX, addButtonY, menuEntryWidth, menuEntryHeight,
													"", FontSize, false, FOREGROUND_COLOR,
													BACKGROUND_COLOR, PRESSED_COLOR, HOVERED_COLOR,
													FOREGROUND_COLOR, Border, NO_SHADOW, NO_ICON,
													AddEntry, 0);
							// plus sign
							DrawCross(menuX + menuEntryWidth / 2,
												addButtonY + menuEntryHeight / 2, 0,
												(float)menuEntryHeight / 2, menuEntryHeight / 10,
												FOREGROUND_COLOR);
						}

						for (int i = 0; i < SlicesCount; i++)
						{
							Vector2 sliceNameTextSize = MeasureTextEx(
									Fonte, Slices[i].Name, FontSize * 2, TEXT_SPACING);
							int menuEntryY = MenuScrollOffset + i * menuEntryHeight;

							Rectangle menuEntryRect = {menuX, menuEntryY, menuEntryWidth,
																				 menuEntryHeight};

							// slice entry background
							DrawRectangleRec(menuEntryRect, COLORS[Slices[i].Color]);

							// slice entry outline
							DrawRectangleLinesEx(menuEntryRect, Border, FOREGROUND_COLOR);

							// draw color palette
							{
								ButtonRow palette[PALETTE_ROW_AMOUNT] = {
										{PALETTE_ROW_PERCENTAGE, PALETTE_COL_AMOUNT,
										 (Button[PALETTE_COL_AMOUNT]){{0}}},
										{PALETTE_ROW_PERCENTAGE, PALETTE_COL_AMOUNT,
										 (Button[PALETTE_COL_AMOUNT]){{0}}}};
								int paletteX = menuX + Padding;
								int paletteY = menuEntryY + sliceNameTextSize.y + Padding * 2;
								int paletteHeight =
										menuEntryHeight - sliceNameTextSize.y - Padding * 4;
								int paletteWidth = menuEntryWidth - sidePadding - Padding;

								static ColorPickArgs colorPickArgs[COLORS_AMOUNT]
																									[COLORS_AMOUNT];

								for (int c = 0; c < COLORS_AMOUNT; c++)
								{
									colorPickArgs[i][c].SliceIndex = i;
									colorPickArgs[i][c].buttonColumn =
											c % (COLORS_AMOUNT / PALETTE_ROW_AMOUNT);
									colorPickArgs[i][c].buttonRow =
											c / (COLORS_AMOUNT / PALETTE_ROW_AMOUNT);

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
															 c == Slices[i].Color ? Padding : Border,
															 NO_SHADOW,
															 NO_ICON,
															 ColorPickWrapper,
															 &colorPickArgs[i][c]};
								}

								DrawButtonGrid(paletteX, paletteY, paletteWidth, paletteHeight,
															 Padding, palette, PALETTE_ROW_AMOUNT);
							}

							// draw editable text box
							{
								Rectangle menuEntryTextFieldRect = {
										menuX + Padding, menuEntryY + Padding,
										menuEntryWidth - sidePadding * 2, sliceNameTextSize.y};

								if (TypingIndex != i)
								{
									SelectTextFieldArgs args = {i};
									DRAW_BUTTON(
											menuEntryTextFieldRect.x, menuEntryTextFieldRect.y,
											menuEntryTextFieldRect.width,
											menuEntryTextFieldRect.height, Slices[i].Name, FontSize,
											false, FOREGROUND_COLOR, BACKGROUND_COLOR, PRESSED_COLOR,
											HOVERED_COLOR, FOREGROUND_COLOR, Border, NO_SHADOW,
											NO_ICON, SelectTextField, &args);
								}
								else
								{
#ifdef PLATFORM_ANDROID
									ShowAndroidKeyboard();
#endif
									DrawTextField(menuEntryTextFieldRect.x,
																menuEntryTextFieldRect.y,
																menuEntryTextFieldRect.width,
																menuEntryTextFieldRect.height, FOREGROUND_COLOR,
																HIGHLIGHT_COLOR, FOREGROUND_COLOR, FontSize,
																Border, Slices[i].Name);

#ifdef PLATFORM_ANDROID
									char inputChar;

									while ((inputChar = GetAndroidInput()) != 0)
									{
										int len = strlen(Slices[i].Name);
										if (len < SLICE_NAME_SIZE - 1)
										{
											Slices[i].Name[len] = tolower(inputChar);
											Slices[i].Name[len + 1] = '\0';
										}
									}

									if (androidDeletePressed)
									{
										int len = strlen(Slices[i].Name);
										if (len > 0)
										{
											Slices[i].Name[len - 1] = '\0';
										}

										androidDeletePressed = false;
									}
#endif
								}
							}

							// draw a trash button to delete slices
							{
								Rectangle trashButtonRect = {
										menuX + menuEntryWidth - sidePadding, menuEntryY + Padding,
										squareButtonSize, squareButtonSize};

								RemoveEntryArgs args = {i};
								DRAW_BUTTON(trashButtonRect.x, trashButtonRect.y,
														trashButtonRect.width, trashButtonRect.height, "",
														FontSize, false, RED, BACKGROUND_COLOR,
														RED_PRESSED_COLOR, RED_HOVERED_COLOR, RED, Padding,
														NO_SHADOW, TrashTexture, RemoveEntry, &args);
							}
						}
					}

					// Draw a close button
					{
						DRAW_BUTTON(cornerButtonRect.x, cornerButtonRect.y,
												cornerButtonRect.width, cornerButtonRect.height, "",
												FontSize, false, RED, BACKGROUND_COLOR,
												RED_PRESSED_COLOR, RED_HOVERED_COLOR, RED, Padding,
												NO_SHADOW, NO_ICON, ToggleMenu, 0);

						// x sign
						DrawCross(cornerButtonRect.x + cornerButtonRect.width / 2,
											cornerButtonRect.y + cornerButtonRect.height / 2, 45,
											squareButtonSize, squareButtonSize / 8, RED);
					}

					if (Clicked)
					{
						TypingIndex = -1;
#ifdef PLATFORM_ANDROID
						HideAndroidKeyboard();
#endif
					}
				}
				break;
				default:
					// Draw the wheel
					{
						{
							int wheelRadius = (ScreenWidth < ScreenHeight
																		 ? ScreenWidth / 2 - ScreenWidth / 16
																		 : ScreenHeight / 2 - ScreenHeight / 32);

							if (SlicesCount == 0)
							{
								WheelAngle -= 5 * DeltaTime;
								DrawWheel(WheelAngle, wheelRadius, DEFAULT_SLICES,
													COLORS_AMOUNT);
							}
							else
							{
								DrawWheel(WheelAngle, wheelRadius, Slices, SlicesCount);

								if (WheelAcceleration > 0)
								{
									WheelAngle -= WheelAcceleration * DeltaTime;
									WheelAcceleration -= WheelAccelerationRate;

									DrawRing((Vector2){(float)ScreenWidth / 2,
																		 (float)ScreenHeight / 2},
													 (float)wheelRadius / 10,
													 (float)wheelRadius / 10 + Padding, WheelAngle,
													 WheelAngle + (360.0f / SlicesCount), 0,
													 HIGHLIGHT_COLOR);
								}
								else if (WheelPickedIndex == STATE_WINNER_POPUP)
								{
									WaitTime(1);
									WheelPickedIndex = STATE_NO_WINNER;
								}
								else if (WheelPickedIndex >= STATE_WINNER)
								{
									Vector2 winnerTextSize =
											MeasureTextEx(Fonte, Slices[WheelPickedIndex].Name,
																		FontSize, TEXT_SPACING);

									Color winnerColor = COLORS[Slices[WheelPickedIndex].Color];

									winnerTextSize.x =
											fmin(winnerTextSize.x, wheelRadius * 2) + Padding * 4;
									winnerTextSize.y = winnerTextSize.y * 2;

									DrawTextBox(
											(ScreenWidth - winnerTextSize.x) / 2,
											(ScreenHeight - winnerTextSize.y) / 2, winnerTextSize.x,
											winnerTextSize.y, Slices[WheelPickedIndex].Name, FontSize,
											GetContrastedTextColor(winnerColor), winnerColor,
											GetContrastedTextColor(winnerColor), Padding, NO_SHADOW);

									WheelPickedIndex = STATE_WINNER_POPUP;
								}
								else
								{
									WheelTextSize =
											MeasureTextEx(Fonte, WHEEL_TEXT, FontSize, TEXT_SPACING);

									DrawTextEx(
											Fonte, WHEEL_TEXT,
											(Vector2){(float)ScreenWidth / 2 - WheelTextSize.x / 2,
																(float)ScreenHeight / 2 - WheelTextSize.y / 2},
											FontSize, TEXT_SPACING, HIGHLIGHT_COLOR);

									if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
											CheckCollisionPointCircle(
													(Vector2){MouseX, MouseY},
													(Vector2){(float)ScreenWidth / 2,
																		(float)ScreenHeight / 2},
													(float)wheelRadius / 4))
									{
										WheelPickedIndex = STATE_SPINNING;
#ifdef PLATFORM_ANDROID
										WheelAcceleration = GetRandomValue(500, 1000);
										WheelAccelerationRate = GetRandomValue(2, 4);
#else
										WheelAcceleration = GetRandomValue(1000, 2000);
										WheelAccelerationRate = (float)GetRandomValue(2, 4) / 10;
#endif
									}
									else if (WheelPickedIndex == STATE_SPINNING)
									{
										WheelPickedIndex =
												floor(fmod(fabs(WheelAngle - 90), 360.0) /
															(360.0 / SlicesCount));
									}
								}
							}
						}

						// Draw a menu button
						{
							DRAW_BUTTON(cornerButtonRect.x, cornerButtonRect.y,
													cornerButtonRect.width, cornerButtonRect.height, "",
													FontSize, false, FOREGROUND_COLOR, BACKGROUND_COLOR,
													PRESSED_COLOR, HOVERED_COLOR, FOREGROUND_COLOR,
													Padding, NO_SHADOW, MenuTexture, ToggleMenu, 0);
						}
					}
					break;
			}

			if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
			{
				Dragging = false;
			}

			LogAppend("INFO(Mouse): X %d Y %d PressedX %d PressedY %d \n", MouseX,
								MouseY, MousePressedX, MousePressedY);
			LogDraw();
			LogSet("");

			EndDrawing();
		}
	}

#define X(Name, NAME) UnloadTexture(Name##Texture);
	ICON_LIST
#undef X
	CloseWindow();
	return 0;
}
// Licensed under the GPL3 or later versions of the GPL license.
// See the LICENSE file in the project root for more information.

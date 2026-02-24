@echo off
setlocal enabledelayedexpansion

if not exist "lib\desktop\" mkdir "lib\desktop"

if not defined CLPATH set CLPATH=cl
set FILES=raylib\src\rcore.c raylib\src\rshapes.c raylib\src\rtextures.c raylib\src\rtext.c raylib\src\rmodels.c raylib\src\utils.c raylib\src\raudio.c raylib\src\rglfw.c

"%CLPATH%" /Zc:preprocessor /Ox /GL /D_CRT_SECURE_NO_WARNINGS /sdl /c /w /D_DEFAULT_SOURCE /DPLATFORM_DESKTOP /DGRAPHICS_API_OPENGL_33 /I"raylib\src" /I"raylib\src\external\glfw\include" %FILES% /Folib/desktop/
if errorlevel 1 exit /b 1

"%CLPATH%" /Zc:preprocessor /Ox /GL /D_CRT_SECURE_NO_WARNINGS /sdl /c /W4 /I"raylib/src/" /TC src/main.c /Folib/desktop/
if errorlevel 1 exit /b 1

"%CLPATH%" /Fe:pickle.exe lib/desktop/*.obj /link /LTCG kernel32.lib user32.lib shell32.lib winmm.lib gdi32.lib opengl32.lib /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup
if errorlevel 1 exit /b 1

echo Build completed successfully!

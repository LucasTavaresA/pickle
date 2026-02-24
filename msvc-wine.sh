#!/usr/bin/env sh
set -ex

if [ -d lib/desktop ]; then
	mkdir -p lib/desktop
fi

if [ -z "$CLPATH" ]; then
	echo "CLPATH is not set"
	exit 1
fi

: "${CLPATH:=cl}"

cl_obj() {
	"$CLPATH" /Zc:preprocessor /Ox /GL /D_CRT_SECURE_NO_WARNINGS /sdl /c "$@"
}

FILES="raylib\src\rcore.c raylib\src\rshapes.c raylib\src\rtextures.c raylib\src\rtext.c raylib\src\rmodels.c raylib\src\utils.c raylib\src\raudio.c raylib\src\rglfw.c"

cl_obj /w /D_DEFAULT_SOURCE /DPLATFORM_DESKTOP /DGRAPHICS_API_OPENGL_33 /I"raylib\src" /I"raylib\src\external\glfw\include" $FILES /Folib/desktop/
cl_obj /W4 /I"raylib/src/" /DRELEASE /PLATFORM_WINDOWS /TC src/main.c /Folib/desktop/
"$CLPATH" /Fe:pickle.exe lib/desktop/*.obj /link /LTCG kernel32.lib user32.lib shell32.lib winmm.lib gdi32.lib opengl32.lib /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup

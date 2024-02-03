#!/usr/bin/env sh
# shellcheck disable=2164,2086
RUN=0
LINUX=0
ANDROID=0
REDOWNLOAD=0
REBUILD=0
BUILD_FLAGS="-ggdb -std=c2x -Wall -Wextra -Wshadow"
PROGRAM="pickle"
MAINTAINER="lucasta"
SOURCE="https://dl.google.com/android/repository/"
CMD_TOOLS="commandlinetools-linux-10406996_latest.zip"
NDK="android-ndk-r26b"

print_help() {
	printf \
		"%s [linux|android] [-r -R -d -b]

-r           runs after building
-R           builds with -DRELEASE, disabling debug mode features
-d           re-downloads android sdk and ndk
-b           rebuild raylib
-h --help    show help\n" "$0"
}

main() {
	while [ "$#" -gt 0 ]; do
		case "$1" in
			"-r")
				RUN=1
				;;
			"-d")
				REDOWNLOAD=1
				;;
			"-b")
				REBUILD=1
				;;
			"-R")
				BUILD_FLAGS="-DRELEASE"
				;;
			"android")
				ANDROID=1
				;;
			"linux")
				LINUX=1
				;;
			"--help")
				print_help
				exit 0
				;;
			"-h")
				print_help
				exit 0
				;;
			*)
				echo "'$1' is not a valid argument!"
				print_help
				exit 1
				;;
		esac

		shift
	done

	if [ "$LINUX" = 0 ] && [ "$ANDROID" = 0 ]; then
		printf "You need to specify a platform to build!\n\n"
		print_help
		exit 1
	elif [ "$LINUX" = 1 ] && [ "$ANDROID" = 1 ]; then
		printf "You can only build for one platform at a time\n\n"
		print_help
		exit 1
	elif [ "$LINUX" = 1 ]; then
		if [ "$REBUILD" = 1 ]; then
			rm -rf ./lib/desktop/
		fi

		if [ ! -d "./lib/desktop/" ]; then
			echo "--------------------------------"
			echo "Building raylib for the desktop!"
			echo "--------------------------------"

			(
				cd raylib/src/
				make clean
				make PLATFORM=PLATFORM_DESKTOP
				mkdir --parents ../../lib/desktop/
				mv libraylib.a ../../lib/desktop/
				make clean
			)
		fi

		cc src/main.c -I./raylib/src/ $BUILD_FLAGS -L./lib/desktop/ -lraylib -lm -o $PROGRAM

		if [ "$RUN" = 1 ]; then
			./$PROGRAM
		fi

		exit 0
	elif [ "$ANDROID" = 1 ]; then
		if [ "$REDOWNLOAD" = 1 ]; then
			rm -rf android/sdk android/ndk
		fi

		if [ ! -d "android/sdk" ] || [ ! -d "android/ndk" ]; then
			echo "--------------------------------"
			echo "Downloading android sdk and ndk!"
			echo "--------------------------------"

			mkdir --parents android/sdk android/build assets lib/armeabi-v7a lib/arm64-v8a lib/x86 lib/x86_64 src
			(
				cd android/build
				mkdir --parents obj dex res/values assets
				mkdir --parents lib/armeabi-v7a lib/arm64-v8a lib/x86 lib/x86_64
				mkdir --parents res/drawable-ldpi res/drawable-mdpi res/drawable-hdpi res/drawable-xhdpi
			)

			if [ ! -e "archives/$CMD_TOOLS" ] || [ ! -e "archives/$NDK-linux.zip" ]; then
				wget -P ./archives/ "$SOURCE$CMD_TOOLS"
				wget -P ./archives/ "$SOURCE$NDK-linux.zip"

				unzip -q ./archives/$CMD_TOOLS -d android/sdk
				unzip -q ./archives/$NDK-linux.zip

				mv ./$NDK ./android/ndk
			fi

			(
				cd android/sdk/cmdline-tools/bin
				./sdkmanager --update --sdk_root=../..
				./sdkmanager --install "build-tools;29.0.3" --sdk_root=../..
				./sdkmanager --install "platform-tools" --sdk_root=../..
				./sdkmanager --install "platforms;android-29" --sdk_root=../..
			)
		fi

		if [ "$REBUILD" = 1 ]; then
			rm -rf ./lib/armeabi-v7a
		fi

		if [ ! -d "./lib/armeabi-v7a/" ]; then
			echo "--------------------------------"
			echo "Building raylib for android!"
			echo "--------------------------------"

			(
				cd raylib/src
				make clean
				make PLATFORM=PLATFORM_ANDROID ANDROID_NDK=../../android/ndk ANDROID_ARCH=arm ANDROID_API_VERSION=29
				mv libraylib.a ../../lib/armeabi-v7a
				make clean
				make PLATFORM=PLATFORM_ANDROID ANDROID_NDK=../../android/ndk ANDROID_ARCH=arm64 ANDROID_API_VERSION=29
				mv libraylib.a ../../lib/arm64-v8a
				make clean
				make PLATFORM=PLATFORM_ANDROID ANDROID_NDK=../../android/ndk ANDROID_ARCH=x86 ANDROID_API_VERSION=29
				mv libraylib.a ../../lib/x86
				make clean
				make PLATFORM=PLATFORM_ANDROID ANDROID_NDK=../../android/ndk ANDROID_ARCH=x86_64 ANDROID_API_VERSION=29
				mv libraylib.a ../../lib/x86_64
				make clean
			)
		fi

		ABIS="armeabi-v7a x86 x86_64"

		BUILD_TOOLS=android/sdk/build-tools/29.0.3
		TOOLCHAIN=android/ndk/toolchains/llvm/prebuilt/linux-x86_64
		NATIVE_APP_GLUE=android/ndk/sources/android/native_app_glue

		FLAGS="-ffunction-sections -funwind-tables -fstack-protector-strong -fPIC -Wall \
	-Wformat -Werror=format-security -no-canonical-prefixes \
	-DANDROID -DPLATFORM_ANDROID -D__ANDROID_API__=29 $BUILD_FLAGS"

		INCLUDES="-I. -I./raylib/src/ -I$NATIVE_APP_GLUE -I$TOOLCHAIN/sysroot/usr/include"

		# Copy icons
		cp assets/icon_ldpi.png android/build/res/drawable-ldpi/icon.png
		cp assets/icon_mdpi.png android/build/res/drawable-mdpi/icon.png
		cp assets/icon_hdpi.png android/build/res/drawable-hdpi/icon.png
		cp assets/icon_xhdpi.png android/build/res/drawable-xhdpi/icon.png

		# Copy other assets
		cp assets/* android/build/assets

		# ______________________________________________________________________________
		#
		#  Compile
		# ______________________________________________________________________________
		#
		for ABI in $ABIS; do
			case "$ABI" in
				"armeabi-v7a")
					CCTYPE="armv7a-linux-androideabi"
					ABI_FLAGS="-std=c99 -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16"
					;;

				"arm64-v8a")
					CCTYPE="aarch64-linux-android"
					ABI_FLAGS="-std=c99 -target aarch64 -mfix-cortex-a53-835769"
					;;

				"x86")
					CCTYPE="i686-linux-android"
					ABI_FLAGS=""
					;;

				"x86_64")
					CCTYPE="x86_64-linux-android"
					ABI_FLAGS=""
					;;
			esac
			CC="$TOOLCHAIN/bin/${CCTYPE}29-clang"

			# Compile native app glue
			# .c -> .o
			$CC -c $NATIVE_APP_GLUE/android_native_app_glue.c -o $NATIVE_APP_GLUE/native_app_glue.o \
				$INCLUDES -I$TOOLCHAIN/sysroot/usr/include/$CCTYPE $FLAGS $ABI_FLAGS

			# .o -> .a
			$TOOLCHAIN/bin/llvm-ar rcs lib/$ABI/libnative_app_glue.a $NATIVE_APP_GLUE/native_app_glue.o

			# Compile project
			$CC src/*.c -o android/build/lib/$ABI/libmain.so -shared \
				$INCLUDES -I$TOOLCHAIN/sysroot/usr/include/$CCTYPE $FLAGS $ABI_FLAGS \
				-Wl,-soname,libmain.so -Wl,--exclude-libs,libatomic.a -Wl,--build-id \
				-Wl,--no-undefined -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now \
				-Wl,--warn-shared-textrel -Wl,--fatal-warnings -u ANativeActivity_onCreate \
				-L. -Landroid/build/obj -Llib/$ABI \
				-lraylib -lnative_app_glue -llog -landroid -lEGL -lGLESv2 -lOpenSLES -latomic -lc -lm -ldl
		done

		# ______________________________________________________________________________
		#
		#  Build APK
		# ______________________________________________________________________________
		#
		$BUILD_TOOLS/aapt package -f -m \
			-S android/build/res -J android/build/src -M android/build/AndroidManifest.xml \
			-I android/sdk/platforms/android-29/android.jar

		# Compile NativeLoader.java
		javac -verbose -source 1.8 -target 1.8 -d android/build/obj \
			-bootclasspath jre/lib/rt.jar \
			-classpath android/sdk/platforms/android-29/android.jar:android/build/obj \
			-sourcepath src android/build/src/com/$MAINTAINER/$PROGRAM/R.java \
			android/build/src/com/$MAINTAINER/$PROGRAM/NativeLoader.java

		$BUILD_TOOLS/dx --verbose --dex --output=android/build/dex/classes.dex android/build/obj

		# Add resources and assets to APK
		$BUILD_TOOLS/aapt package -f \
			-M android/build/AndroidManifest.xml -S android/build/res -A assets \
			-I android/sdk/platforms/android-29/android.jar -F $PROGRAM.apk android/build/dex

		# Add libraries to APK
		(
			cd android/build

			for ABI in $ABIS; do
				../../$BUILD_TOOLS/aapt add ../../$PROGRAM.apk lib/$ABI/libmain.so
			done
		)

		# Sign and zipalign APK
		if [ ! -e "./password.txt" ]; then
			echo "You need a password to sign the apk with, create a file called password.txt in the project root with your password!"
			exit 1
		fi

		password="$(cat ./password.txt)"

		if [ ! -e "./android/$MAINTAINER.keystore" ]; then
			echo "There is no keystore, Creating one for you!"
			(
				cd android
				keytool -genkeypair -validity 1000 -dname "CN=$MAINTAINER,O=Android,C=ES" -keystore $MAINTAINER.keystore -storepass "$password" -keypass "$password" -alias projectKey -keyalg RSA
			)
		fi

		jarsigner -keystore android/$MAINTAINER.keystore -storepass "$password" -keypass "$password" \
			-signedjar $PROGRAM.apk $PROGRAM.apk projectKey

		$BUILD_TOOLS/zipalign -f 4 $PROGRAM.apk $PROGRAM.final.apk
		mv -f $PROGRAM.final.apk $PROGRAM.apk

		# Install to device or emulator
		if [ "$RUN" = 1 ]; then
			android/sdk/platform-tools/adb install -r $PROGRAM.apk
			android/sdk/platform-tools/adb shell am start -n com.$MAINTAINER.$PROGRAM/com.$MAINTAINER.$PROGRAM.NativeLoader
		fi

		exit 0
	fi
}

main "$@"

#!/usr/bin/env python3

import subprocess
import shlex
import pathlib
import sys
import os

plugin_root = pathlib.Path(__file__).parent.parent
maplibre_root = plugin_root / 'third-party/maplibre-native'

def run_command(command, cwd = plugin_root):
  subprocess.run(shlex.split(command), cwd=cwd, check=True)


maplibre_native_flutter = maplibre_root / 'platform/flutter'

# iOS
# run_command(f'rm -rf {maplibre_native_flutter}')
# run_command(f'cp -rf {plugin_root / "src"} {maplibre_native_flutter}')

# run_command(f'cmake -G Xcode -S . -B {maplibre_root / 'build-iphoneos'} -DCMAKE_OSX_SYSROOT=iphoneos -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_ARCHITECTURES=arm64', cwd=maplibre_native_flutter)
# run_command(f'cmake --build build-iphoneos --target flmln --config Debug', cwd=maplibre_root)

# run_command(f'cmake -G Xcode -S . -B {maplibre_root / 'build-iphonesimulator'} -DCMAKE_OSX_SYSROOT=iphonesimulator -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_ARCHITECTURES=arm64', cwd=maplibre_native_flutter)
# run_command('cmake --build build-iphonesimulator --target flmln --config Debug', cwd=maplibre_root)

# # # bundle frameworks into xcframework
# run_command('rm -rf ios/flmln/Artifacts/flmln.xcframework')
# run_command(f'xcodebuild -create-xcframework -framework {maplibre_root / 'build-iphoneos/Debug-iphoneos/flmln.framework'} -framework {maplibre_root / 'build-iphonesimulator/Debug-iphonesimulator/flmln.framework'} -output ios/flmln/Artifacts/flmln.xcframework')

# Android
run_command(f'rm -rf {maplibre_native_flutter}')
run_command(f'cp -rf {plugin_root / "src"} {maplibre_native_flutter}')

android_ndk_home = os.environ.get('ANDROID_NDK_HOME')
ndk_toolchain = f"{android_ndk_home}/build/cmake/android.toolchain.cmake"
print(ndk_toolchain)
run_command(f'cmake -G Ninja -S . -B {maplibre_root / "build-android-arm64-v8a"} -DCMAKE_SYSTEM_NAME=Android -DCMAKE_TOOLCHAIN_FILE={ndk_toolchain} -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=23 -DCMAKE_ANDROID_STL_TYPE=c++_shared', cwd=maplibre_native_flutter)
run_command(f'cmake --build build-android-arm64-v8a --target flmln --config Debug', cwd=maplibre_root)
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

run_command(f'rm -rf {maplibre_native_flutter}')
run_command(f'cp -rf {plugin_root / "src"} {maplibre_native_flutter}')

run_command(f'cmake -G Xcode -S . -B {maplibre_root / 'build-iphoneos'} -DCMAKE_OSX_SYSROOT=iphoneos -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_ARCHITECTURES=arm64', cwd=maplibre_native_flutter)
run_command(f'cmake --build build-iphoneos --target flmln --config Debug', cwd=maplibre_root)

run_command(f'cmake -G Xcode -S . -B {maplibre_root / 'build-iphonesimulator'} -DCMAKE_OSX_SYSROOT=iphonesimulator -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_ARCHITECTURES=arm64', cwd=maplibre_native_flutter)
run_command('cmake --build build-iphonesimulator --target flmln --config Debug', cwd=maplibre_root)

# # bundle frameworks into xcframework
run_command('rm -rf ios/flmln/Artifacts/flmln.xcframework')
run_command(f'xcodebuild -create-xcframework -framework {maplibre_root / 'build-iphoneos/Debug-iphoneos/flmln.framework'} -framework {maplibre_root / 'build-iphonesimulator/Debug-iphonesimulator/flmln.framework'} -output ios/flmln/Artifacts/flmln.xcframework')
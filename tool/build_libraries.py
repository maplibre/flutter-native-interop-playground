#!/usr/bin/env python3

import subprocess
import shlex
import pathlib
import sys

plugin_root = pathlib.Path(__file__).parent.parent
maplibre_native_root = plugin_root / 'third-party/maplibre-native'
bazel_opts = '-c dbg'

# copy contents of `src` to `third-party/maplibre-native/platform/flutter`
def copy_src_to_maplibre_native():
    print('Copying src to maplibre-native')

    src = plugin_root / 'src'
    maplibre_native = plugin_root / 'third-party/maplibre-native/platform/flutter'

    exclusion_list = ['MODULE.bazel']

    # remove old files
    subprocess.run(shlex.split(f'rm -rf {maplibre_native}'), check=False)

    # create directory
    maplibre_native.mkdir(parents=True, exist_ok=True)

    for item in src.iterdir():
        if item.name not in exclusion_list:
            subprocess.run(shlex.split(f'cp -rf {item} {maplibre_native}'), check=True)

    print('Copied src to maplibre-native')

    # patch platform/default/BUILD.bazel so that Flutter has visibility
    lines = []
    with open(maplibre_native_root / 'platform/default/BUILD.bazel', 'r') as f:
        lines = f.readlines()

    index = lines.index('        "//platform/macos:__pkg__",\n')
    lines.insert(index + 1, '        "//platform/flutter:__pkg__",\n')

    with open(maplibre_native_root / 'platform/default/BUILD.bazel', 'w') as f:
        f.writelines(lines)

    print('Patched platform/default/BUILD.bazel')

def build_ios_dynamic_library():
    # Run bazel build for iOS dynamic library
    subprocess.run(shlex.split(f'bazel build //platform/flutter:flmbgl_ios_xcframework --//:renderer=metal {bazel_opts}'), cwd=maplibre_native_root, check=True)

    # Copy the built xcframework to the plugin directory
    bazel_out_ios_xcframework = maplibre_native_root / 'bazel-bin/platform/flutter/flmbgl_ios_xcframework.xcframework.zip'
    ios_dynamic_library = plugin_root / 'ios/flmbgl/Artifacts/flmbgl_ios_xcframework.xcframework'

    # remove old xcframework & unzip the new one
    subprocess.run(shlex.split(f'rm -rf {ios_dynamic_library}'), check=False)
    subprocess.run(shlex.split(f'unzip -o {bazel_out_ios_xcframework} -d {ios_dynamic_library.parent}'), check=True)
    return ios_dynamic_library

def build_macos_dynamic_library():
    # Run bazel build for macOS dynamic library
    subprocess.run(shlex.split(f'bazel build //platform/flutter:flmbgl_macos_xcframework --//:renderer=metal {bazel_opts}'), cwd=maplibre_native_root, check=True)

    # Copy the built xcframework to the plugin directory
    bazel_out_macos_xcframework = maplibre_native_root / 'bazel-bin/platform/flutter/flmbgl_macos_xcframework.xcframework.zip'
    macos_dynamic_library = plugin_root / 'macos/flmbgl/Artifacts/flmbgl_macos_xcframework.xcframework'

    # remove old xcframework & unzip the new one
    subprocess.run(shlex.split(f'rm -rf {macos_dynamic_library}'), check=False)
    subprocess.run(shlex.split(f'unzip -o {bazel_out_macos_xcframework} -d {macos_dynamic_library.parent}'), check=True)
    return macos_dynamic_library

def run_ffigen():
    print('Running ffigen')
    subprocess.run(
        shlex.split('fvm dart run ffigen --config ffigen.yaml'),
        check=True,
        cwd=plugin_root
    )

def __run__():
    args = sys.argv[1:]

    copy_src_to_maplibre_native()

    has_args = len(args) > 0

    is_darwin = 'darwin' in args
    is_ios = is_darwin or 'ios' in args
    is_macos = is_darwin or 'macos' in args

    if not has_args or is_ios:
      print('🕛 Building iOS dynamic library')
      ios_dynamic_library = build_ios_dynamic_library()
      print('🔥 iOS dynamic library built successfully')

    if not has_args or is_macos:
      print('🕛 Building macOS dynamic library')
      macos_dynamic_library = build_macos_dynamic_library()
      print('🔥 macOS dynamic library built successfully')

    run_ffigen()

if __name__ == '__main__':
    __run__()
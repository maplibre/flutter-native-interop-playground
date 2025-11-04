#!/usr/bin/env python3

import subprocess
import shlex
import pathlib
import sys
import os

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
    if "flutter" not in lines[index + 1]:
        lines.insert(index + 1, '        "//platform/flutter:__pkg__",\n')

    with open(maplibre_native_root / 'platform/default/BUILD.bazel', 'w') as f:
        f.writelines(lines)

    print('Patched platform/default/BUILD.bazel')

def build_android():
    # Run bazel build for Android dynamic library
    abis = ['android_arm64-v8a']
    android_ndk_abi_folder_map = {
        'android_arm64-v8a': 'aarch64-linux-android',
        'android_armeabi-v7a': 'armv-linux-androideabi',
        'android_x86': 'i686-linux-android',
        'android_x86_64': 'x86_64-linux-android',
    }
    android_ndk_home = os.environ.get('ANDROID_NDK_HOME')
    # TODO: handle non-darwin hosts
    ndk_toolchain_path = f'{android_ndk_home}/toolchains/llvm/prebuilt/darwin-x86_64'

    for abi in abis:
        jni_lib_folder_name = abi.replace('android_', '')
        subprocess.run(shlex.split(f'bazel --output_base=build-{abi} build //platform/flutter:flmln_android --//:renderer=drawable {bazel_opts} --platforms=//platform/flutter:{abi}'), cwd=maplibre_native_root, check=True)
        bazel_out_android_so = maplibre_native_root / f'bazel-bin/platform/flutter/libflmln_android.so'
        android_dynamic_library = plugin_root / f'android/src/main/jniLibs/{jni_lib_folder_name}/libflmln.so'
        android_dynamic_library.parent.mkdir(parents=True, exist_ok=True)
        subprocess.run(shlex.split(f'cp -f {bazel_out_android_so} {android_dynamic_library}'), check=True)

        # copy c++_shared.so
        ndk_abi_folder = android_ndk_abi_folder_map[abi]
        cxx_shared_so_path = f'{ndk_toolchain_path}/sysroot/usr/lib/{ndk_abi_folder}/libc++_shared.so'
        android_cxx_shared_library = plugin_root / f'android/src/main/jniLibs/{jni_lib_folder_name}/libc++_shared.so'
        subprocess.run(shlex.split(f'cp -f {cxx_shared_so_path} {android_cxx_shared_library}'), check=True)


def build_ios():
    # Run bazel build for iOS dynamic library
    subprocess.run(shlex.split(f'bazel --output_base=build-ios build //platform/flutter:flmln_ios_xcframework --//:renderer=metal {bazel_opts}'), cwd=maplibre_native_root, check=True)

    # Copy the built xcframework to the plugin directory
    bazel_out_ios_xcframework = maplibre_native_root / 'bazel-bin/platform/flutter/flmln_ios_xcframework.xcframework.zip'
    ios_dynamic_library = plugin_root / 'ios/flmln/Artifacts/flmln_ios.xcframework'

    # remove old xcframework & unzip the new one
    subprocess.run(shlex.split(f'rm -rf {ios_dynamic_library}'), check=False)
    subprocess.run(shlex.split(f'unzip -o {bazel_out_ios_xcframework} -d {ios_dynamic_library.parent}'), check=True)
    return ios_dynamic_library

def build_macos():
    # Run bazel build for macOS dynamic library
    subprocess.run(shlex.split(f'bazel --output_base=build-macos build //platform/flutter:flmln_macos_xcframework --//:renderer=metal {bazel_opts}'), cwd=maplibre_native_root, check=True)

    # Copy the built xcframework to the plugin directory
    bazel_out_macos_xcframework = maplibre_native_root / 'bazel-bin/platform/flutter/flmln_macos_xcframework.xcframework.zip'
    macos_dynamic_library = plugin_root / 'macos/flmln/Artifacts/flmln_macos.xcframework'

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
    is_android = 'android' in args

    if not has_args or is_android:
        print('🕛 Building Android dynamic library')
        build_android()
        print('🔥 Android dynamic library built successfully')

    if not has_args or is_ios:
        print('🕛 Building iOS dynamic library')
        build_ios()
        print('🔥 iOS dynamic library built successfully')

    if not has_args or is_macos:
        print('🕛 Building macOS dynamic library')
        build_macos()
        print('🔥 macOS dynamic library built successfully')

    run_ffigen()

if __name__ == '__main__':
    __run__()
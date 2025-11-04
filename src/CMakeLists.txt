cmake_minimum_required(VERSION 3.25)
project(flmln CXX C OBJCXX OBJC)

# Configurations

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MLN_CORE_INCLUDE_DEPS ON)
set(MLN_WITH_GLFW OFF)
set(MLN_WITH_METAL ON)
set(MLN_WITH_RTTI ON)

set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0)
set(CMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH NO)

add_definitions(-DFLMLN_MTL -DFLMLN_IOS)

# maplibre-native

add_subdirectory(../.. ../build-maplibre-native EXCLUDE_FROM_ALL)

# flmln

add_library(flmln SHARED
  src/flmln.cpp
  src/flmln.h
  src/flmln/flutter_texture_interface.cpp
  src/flmln/flutter_texture_interface.hpp
  src/flmln/map_observer.hpp
  src/flmln/renderer_backend.cpp
  src/flmln/renderer_backend.hpp
  src/flmln/renderer_frontend.hpp
  src/flmln/mtl/renderer_backend.hpp
  # src/flmln/platform/darwin/flmln_darwin_stub.c
)

set_target_properties(flmln PROPERTIES
  PUBLIC_HEADER src/flmln.h
)

target_link_libraries(flmln PRIVATE mbgl-vendor-metal-cpp)
target_link_libraries(flmln PRIVATE mbgl-core)
target_link_libraries(flmln PRIVATE ios-sdk-static)

target_include_directories(flmln PRIVATE
  ../../src
)

target_include_directories(flmln PRIVATE
  ../../include
)

target_link_libraries(flmln PRIVATE
  "-framework Foundation"
  "-framework CoreFoundation"
  "-framework CoreGraphics"
  "-framework ImageIO"
  "-framework CoreText"
)

target_link_options(flmln PRIVATE
  "-undefined"
  "dynamic_lookup"
)

set_target_properties(mbgl-core PROPERTIES
  XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES"
)

set_target_properties(ios-sdk-static PROPERTIES
  XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES"
)

set(CMAKE_OBJC_FLAGS "-fobjc-arc")
set(CMAKE_OBJCXX_FLAGS "-fobjc-arc")
target_compile_options(mbgl-core PRIVATE -fobjc-arc)
target_compile_options(ios-sdk-static PRIVATE -fobjc-arc)

set_target_properties(flmln PROPERTIES
  FRAMEWORK TRUE
  MACOSX_FRAMEWORK_IDENTIFIER "com.kekland.flmln"
  XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.kekland.flmln"
  XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES"
)

# set_target_properties(FRAMEWORK)
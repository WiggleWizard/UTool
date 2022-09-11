cmake_minimum_required(VERSION 3.22)

project(UTool)

# Options
set(SKELETON_SOURCE_PATH "" CACHE STRING "Optional path to Skeleton. If empty, latest Skeleton library will be downloaded")
set(SKELETON_VERSION "" CACHE STRING "Optional. Specify a version for Skeleton if SKELETON_SOURCE_PATH not set")

add_subdirectory(thirdparty)
add_subdirectory(src)
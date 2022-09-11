cmake_minimum_required(VERSION 3.2)
project(simpleini)

CPMAddPackage(
    NAME simpleini
    GITHUB_REPOSITORY brofield/simpleini
    GIT_TAG 4.19
    VERSION 4.19)

set(SIMPLEINI_SOURCES
    ${simpleini_SOURCE_DIR}/ConvertUTF.c
)

# build as a static library
add_library(simpleini STATIC ${SIMPLEINI_SOURCES})

# include libraries
target_include_directories(simpleini PUBLIC ${simpleini_SOURCE_DIR})
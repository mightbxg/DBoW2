cmake_minimum_required(VERSION 3.10)

project(DBoW2 CXX C)

# define build options
option(BUILD_UTILS "Build utility executables" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_SHARED_LIBS "Build DBoW2 as a shared library" OFF)

# define build types

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

# find libraries
find_package(OpenCV 4.0 QUIET)
if(NOT OpenCV_FOUND)
    find_package(OpenCV 3.0 QUIET)
    if(NOT OpenCV_FOUND)
      message(FATAL_ERROR "OpenCV >= 3.0 not found")
    endif()
endif()
message(STATUS "Use OpenCV ${OpenCV_VERSION}")

# create a library
set(MAIN_LIB_NAME DBoW2)
file(GLOB_RECURSE HEDERS_FOR_IDE
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")
add_library(${MAIN_LIB_NAME}
    ${HEDERS_FOR_IDE}
    src/BowVector.cpp
    src/FBRIEF.cpp
    src/FeatureVector.cpp
    src/FORB.cpp
    src/QueryResults.cpp
    src/ScoringObject.cpp)
target_include_directories(${MAIN_LIB_NAME} PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>")
target_compile_features(${MAIN_LIB_NAME} PUBLIC cxx_std_11)
target_link_libraries(${MAIN_LIB_NAME} PUBLIC ${OpenCV_LIBS})
# set compile options
if(BUILD_SHARED_LIBS)
    target_compile_options(${MAIN_LIB_NAME} PRIVATE
      $<$<OR:$<CXX_COMPILER_ID:MSVC>>:
        /W4 /MD$<$<CONFIG:Debug>:d>>)
else()
    target_compile_options(${MAIN_LIB_NAME} PRIVATE
      $<$<OR:$<CXX_COMPILER_ID:MSVC>>:
        /W4 /MT$<$<CONFIG:Debug>:d>>)
endif()
target_compile_options(${MAIN_LIB_NAME} PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
      -Wall -Wextra -pedantic $<$<CONFIG:Debug>:-Og> $<$<CONFIG:Release>:-O3>>)

# install
install(DIRECTORY include/DBoW2
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(TARGETS DBoW2
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
configure_file(DBoW2.cmake.in ${PROJECT_BINARY_DIR}/DBoW2Config.cmake @ONLY)
install(FILES ${PROJECT_BINARY_DIR}/DBoW2Config.cmake
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/DBoW2)

# build utilities
if(BUILD_UTILS)
    add_subdirectory(util)
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

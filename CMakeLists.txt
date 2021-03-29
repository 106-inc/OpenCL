cmake_minimum_required(VERSION 3.12)

project(bitonic)

set(EX_NAME ${PROJECT_NAME}.o)

# option for debug (debug info)
set(PR_DEBUG_OPTIONS "-g")

# option for release (optimization)
set(PR_RELEASE_OPTIONS "-O2")

# some useful warnings
set(WARN_OPTIONS "-Wall -Wextra -Wunused -Wpedantic -Wno-old-style-cast -Waggressive-loop-optimizations -Wnon-virtual-dtor -Wuseless-cast")

find_package(OpenCL REQUIRED)

add_executable(${EX_NAME} main.cc biton.cc)

# Specify standard
target_compile_features(${EX_NAME} PRIVATE cxx_std_20)

target_compile_options(${EX_NAME} PRIVATE ${WARN_OPTIONS})

# Required libraries
target_link_libraries(${EX_NAME} OpenCL)

# Specify compiler options fro debug and release
target_compile_options(${EX_NAME} PRIVATE "$<$<CONFIG:DEBUG>:${PR_DEBUG_OPTIONS}>")
target_compile_options(${EX_NAME} PRIVATE "$<$<CONFIG:RELEASE>:${PR_RELEASE_OPTIONS}>")

target_compile_definitions(${EX_NAME} PRIVATE
  $<$<CONFIG:Debug>:DUMPALL>
)
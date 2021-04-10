cmake_minimum_required(VERSION 3.12)

project(bsort)

set(CORRECT_VERS ${PROJECT_NAME}.o)
set(TIME_VERS ${PROJECT_NAME}_time.o)

# option for debug (debug info)
set(PR_DEBUG_OPTIONS -g)

# option for release (optimization)
set(PR_RELEASE_OPTIONS -O2)

# some useful warnings
set(WARN_OPTIONS 
  -Wall -Wextra -Wunused -Wpedantic # classic
  -Wno-old-style-cast -Waggressive-loop-optimizations 
  -Wuseless-cast
  -Wunreachable-code)

find_package(OpenCL REQUIRED)

add_executable(${CORRECT_VERS} main.cc biton.cc err_what.cc)

add_executable(${TIME_VERS} main.cc biton.cc err_what.cc)

set(TARGETS ${CORRECT_VERS} ${TIME_VERS})


foreach (T_NAME ${TARGETS})
  # Specify standard
  target_compile_features(${T_NAME} PRIVATE cxx_std_20)

  # Specify warning options
  target_compile_options(${T_NAME} PRIVATE ${WARN_OPTIONS})

  # Required libraries
  target_link_libraries(${T_NAME} OpenCL)

  # Specify compiler options fro debug and release
  target_compile_options(${T_NAME} PRIVATE "$<$<CONFIG:DEBUG>:${PR_DEBUG_OPTIONS}>")
  target_compile_options(${T_NAME} PRIVATE "$<$<CONFIG:RELEASE>:${PR_RELEASE_OPTIONS}>")
  target_compile_definitions(${T_NAME} PRIVATE CL_HPP_ENABLE_EXCEPTIONS
                                               CL_HPP_TARGET_OPENCL_VERSION=210)

endforeach()

target_compile_definitions(${CORRECT_VERS} PRIVATE CORRECT)

target_compile_definitions(${TIME_VERS} PRIVATE TIME)




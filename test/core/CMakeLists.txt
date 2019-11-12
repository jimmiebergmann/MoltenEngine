project (CurseCoreTests)
cmake_minimum_required(VERSION 2.8)

find_package(Threads)

option(CODE_COVERAGE "Enables coverage reporting" OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CoreIncludeDir "${CMAKE_SOURCE_DIR}/core/include")
set(CoreSourceDir "${CMAKE_SOURCE_DIR}/core/source")
file(GLOB_RECURSE CoreSources "${CoreSourceDir}/*.c" "${CoreSourceDir}/*.cpp")

set(TestDir "${CMAKE_SOURCE_DIR}/test")
set(CoreTestDir "${TestDir}/core")
file(GLOB_RECURSE CoreTestSources "${CoreTestDir}/*.c" "${CoreTestDir}/*.cpp")


SET(gtest_force_shared_crt on CACHE BOOL
  "Use shared (DLL) run-time lib even when Google Test is built as static lib.")
add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/googletest/googletest
                 ${CMAKE_SOURCE_DIR}/vendor/googletest/googletest
                 EXCLUDE_FROM_ALL) # Add googletest directly to our build.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()


include_directories ("${CoreIncludeDir}")
add_executable(CurseCoreTest "${CoreTestSources}")

set_target_properties( CurseCoreTest
  PROPERTIES
  OUTPUT_NAME_DEBUG "CurseCoreTestDebug"
  OUTPUT_NAME_RELEASE "CurseCoreTest"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/test"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/test"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/test"
)


if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU") ## Code coverage
  set_target_properties(CurseCoreTest PROPERTIES COMPILE_FLAGS "--coverage")
  target_link_libraries(CurseCoreTest "gcov")
endif(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")

target_link_libraries(CurseCoreTest gtest_main)
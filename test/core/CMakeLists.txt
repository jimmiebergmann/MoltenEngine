project (CurseCoreTests)
cmake_minimum_required(VERSION 2.8)

find_package(Threads)

option(CODE_COVERAGE "Enables coverage reporting" OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

set(RootDir "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(CoreIncludeDir "${RootDir}/core/include")
set(CoreTestDir "${CMAKE_CURRENT_SOURCE_DIR}")
file(GLOB_RECURSE CoreTestSources "${CoreTestDir}/*.c" "${CoreTestDir}/*.cpp")

message("${CMAKE_CURRENT_SOURCE_DIR}")
message("${RootDir}")
message("${CoreIncludeDir}")
message("${CoreTestDir}")
message("${CoreTestSources}")


SET(gtest_force_shared_crt on CACHE BOOL
  "Use shared (DLL) run-time lib even when Google Test is built as static lib.")
add_subdirectory(${RootDir}/vendor/googletest/googletest
                 ${RootDir}/vendor/googletest/googletest
                 EXCLUDE_FROM_ALL)
				 
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()
include_directories ("${CoreIncludeDir}")

add_executable(CurseCoreTest "${CoreTestSources}")
set_target_properties( CurseCoreTest
  PROPERTIES
  OUTPUT_NAME_DEBUG "CurseCoreTestDebug"
  OUTPUT_NAME_RELEASE "CurseCoreTest"
  RUNTIME_OUTPUT_DIRECTORY "${RootDir}/bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${RootDir}/bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${RootDir}/bin"
)

if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU") ## Code coverage
  set_target_properties(CurseCoreTest PROPERTIES COMPILE_FLAGS "--coverage")
  target_link_libraries(CurseCoreTest "gcov")
endif(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")

target_link_libraries(CurseCoreTest Curse gtest_main)
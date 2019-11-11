project (CurseCoreTests)
cmake_minimum_required(VERSION 2.8)

find_package(Threads)

#SET(gtest_force_shared_crt on CACHE BOOL
#  "Use shared (DLL) run-time lib even when Google Test is built as static lib.")
#add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/googletest/googletest
#                 ${CMAKE_SOURCE_DIR}/vendor/googletest/googletest
#                 EXCLUDE_FROM_ALL) # Add googletest directly to our build.
#if (CMAKE_VERSION VERSION_LESS 2.8.11)
#  include_directories("${gtest_SOURCE_DIR}/include")
#endif()


set(CoreIncludeDir "${CMAKE_SOURCE_DIR}/core/include")
set(CoreSourceDir "${CMAKE_SOURCE_DIR}/core/source")
file(GLOB_RECURSE CoreSources "${CoreSourceDir}/*.c" "${CoreSourceDir}/*.cpp")

set(TestDir "${CMAKE_SOURCE_DIR}/test")
set(CoreTestDir "${TestDir}/core")
file(GLOB_RECURSE CoreTestSources "${CoreTestDir}/*.c" "${CoreTestDir}/*.cpp")


include_directories ("${CoreIncludeDir}")
add_executable(CurseCoreTest "${CoreTestSources}")

set_target_properties( CurseCoreTest
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin"
)

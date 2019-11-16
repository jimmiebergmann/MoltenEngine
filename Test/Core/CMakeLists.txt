project (CurseCoreTests)
cmake_minimum_required(VERSION 3.1)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../CMake/Tools.cmake)

option(CURSE_ENABLE_COVERAGE "Enables coverage reporting" OFF)

find_package(Threads)

# Set compiler flags
if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -fno-elide-constructors")
  set(CMAKE_CXX_FLAGS_DEBUG "-g")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

set(RootDir "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(CoreDir "${RootDir}/Core")
set(TestDir "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(TestHeadersDir "${TestDir}/Headers")
set(CoreHeadersDir "${CoreDir}/Headers")
set(CoreTestDir "${CMAKE_CURRENT_SOURCE_DIR}/Core")
file(GLOB_RECURSE TestSources "${TestDir}/*.c" "${TestDir}/*.cpp")
file(GLOB_RECURSE TestHeaders "${TestDir}/*.h" "${TestDir}/*.hpp")
file(GLOB_RECURSE CoreTestSources "${CoreTestDir}/*.c" "${CoreTestDir}/*.cpp")
file(GLOB_RECURSE CoreTestHeaders "${CoreTestDir}/*.h" "${CoreTestDir}/*.hpp")

message("Current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
message("Code coverage: ${CURSE_ENABLE_COVERAGE}")


SET(gtest_force_shared_crt on CACHE BOOL
  "Use shared (DLL) run-time lib even when Google Test is built as static lib.")
add_subdirectory(${RootDir}/Vendor/googletest/googletest
                 ${RootDir}/Vendor/googletest/googletest
                 EXCLUDE_FROM_ALL)

if (NOT TARGET Curse)
	add_subdirectory(${CoreDir} ${CoreDir} EXCLUDE_FROM_ALL)
endif() 
				 
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()
include_directories ("${TestHeadersDir}")
include_directories ("${CoreHeadersDir}")

add_executable(CurseCoreTest "${TestSources}" "${TestHeaders}" "${CoreTestSources}" "${CoreTestHeaders}")

CreateSourceGroups("${TestSources}" "${TestDir}")
CreateSourceGroups("${TestHeaders}" "${TestDir}")

CreateSourceGroups("${CoreTestSources}" "${CoreTestDir}")
CreateSourceGroups("${CoreTestHeaders}" "${CoreTestDir}")

set_target_properties( CurseCoreTest
  PROPERTIES
  OUTPUT_NAME_DEBUG "CurseCoreTestDebug"
  OUTPUT_NAME_RELEASE "CurseCoreTest"
  RUNTIME_OUTPUT_DIRECTORY "${RootDir}/Bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${RootDir}/Bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${RootDir}/Bin"
)

if(CURSE_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU") ## Code coverage
  set_target_properties(CurseCoreTest PROPERTIES COMPILE_FLAGS "--coverage")
  target_link_libraries(CurseCoreTest "gcov")
endif()

target_link_libraries(CurseCoreTest Curse gtest_main)
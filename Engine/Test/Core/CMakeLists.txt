project (MoltenCoreTests)
cmake_minimum_required(VERSION 3.16)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../CMake/Tools.cmake)

option(MOLTEN_ENABLE_COVERAGE "Enables coverage reporting" OFF)

find_package(Threads)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

# Get paths and files.
set(RootDir "${CMAKE_CURRENT_SOURCE_DIR}/../../..")
set(EngineDir "${RootDir}/Engine")
set(CoreDir "${EngineDir}/Core")
set(ThirdPartyDir "${EngineDir}/Vendor/ThirdParty")
set(TestDir "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(TestHeadersDir "${TestDir}/Headers")
set(CoreHeadersDir "${CoreDir}/Headers")
set(CoreTestDir "${CMAKE_CURRENT_SOURCE_DIR}/Core")
file(GLOB_RECURSE TestSources "${TestDir}/*.c" "${TestDir}/*.cpp")
file(GLOB_RECURSE TestHeaders "${TestDir}/*.h" "${TestDir}/*.hpp")
file(GLOB_RECURSE CoreTestSources "${CoreTestDir}/*.c" "${CoreTestDir}/*.cpp")
file(GLOB_RECURSE CoreTestHeaders "${CoreTestDir}/*.h" "${CoreTestDir}/*.hpp")

SET(gtest_force_shared_crt on CACHE BOOL
  "Use shared (DLL) run-time lib even when Google Test is built as static lib.")
add_subdirectory(${ThirdPartyDir}/googletest
                 ${ThirdPartyDir}/googletest
                 EXCLUDE_FROM_ALL)

if (NOT TARGET Molten)
	add_subdirectory(${CoreDir} ${CoreDir} EXCLUDE_FROM_ALL)
endif() 
				 
include_directories ("${TestHeadersDir}")
include_directories ("${CoreHeadersDir}")

add_executable(MoltenCoreTest "${TestSources}" "${TestHeaders}" "${CoreTestSources}" "${CoreTestHeaders}")

CreateSourceGroups("${TestSources}" "${TestDir}")
CreateSourceGroups("${TestHeaders}" "${TestDir}")

CreateSourceGroups("${CoreTestSources}" "${CoreTestDir}")
CreateSourceGroups("${CoreTestHeaders}" "${CoreTestDir}")

set_target_properties( MoltenCoreTest
  PROPERTIES
  OUTPUT_NAME_DEBUG "MoltenCoreTestDebug"
  OUTPUT_NAME_RELEASE "MoltenCoreTest"
  RUNTIME_OUTPUT_DIRECTORY "${RootDir}/Bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${RootDir}/Bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${RootDir}/Bin"
)

SetVisualStudioWorkingDir("MoltenCoreTest" "${RootDir}/Bin")

if(MOLTEN_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU") ## Code coverage
  set_target_properties(MoltenCoreTest PROPERTIES COMPILE_FLAGS "--coverage")
  target_link_libraries(MoltenCoreTest "gcov")
endif()

target_link_libraries(MoltenCoreTest Molten gtest_main)

# Set compiler flags
if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS_DEBUG "-g")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
  target_compile_options(MoltenCoreTest PRIVATE -Werror -Wall -Wextra -Wno-long-long)
endif()

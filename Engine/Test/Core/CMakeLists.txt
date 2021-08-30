cmake_minimum_required(VERSION 3.16)
if(POLICY CMP0092)
  cmake_policy(SET CMP0092 NEW)
endif()

project (MoltenCoreTests)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../CMake/Tools.cmake)

option(MOLTEN_ENABLE_COVERAGE "Enables coverage reporting" OFF)

find_package(Threads)

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

SetDefaultCompileOptions(MoltenCoreTest)
EnableMultiProcessorCompilation(MoltenCoreTest)

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
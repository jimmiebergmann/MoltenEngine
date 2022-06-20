cmake_minimum_required(VERSION 3.16)
if(POLICY CMP0092)
  cmake_policy(SET CMP0092 NEW)
endif()

project (MoltenCoreTests)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../CMake/Tools.cmake)

option(MOLTEN_ENABLE_COVERAGE "Enables coverage reporting" OFF)

find_package(Threads)

# Get directories and files.
set(RootDir "${CMAKE_CURRENT_SOURCE_DIR}/../../..")
set(EngineDir "${RootDir}/Engine")
set(CoreDir "${EngineDir}/Core")
set(CoreHeadersDir "${CoreDir}/Headers")
set(ThirdPartyDir "${EngineDir}/Vendor/ThirdParty")
set(CoreTestDir "${CoreDir}/Test")
set(CoreTestSourceDir "${CoreTestDir}/Source")
set(TestDir "${EngineDir}/Test")
set(TestIncludeDir "${TestDir}/Headers/Molten/Test")

set(TestMainSource "${TestDir}/Source/Molten/Test/TestMain.cpp")
file(GLOB_RECURSE CoreTestSources "${CoreTestSourceDir}/*.c" "${CoreTestSourceDir}/*.cpp")

# Add google test
if (NOT TARGET gtest)
	SET(gtest_force_shared_crt on CACHE BOOL
		"Use shared (DLL) run-time lib even when Google Test is built as static lib.")
	add_subdirectory(${ThirdPartyDir}/googletest ${ThirdPartyDir}/googletest EXCLUDE_FROM_ALL)
	RemoveWarningAsError(gtest)
	EnableMultiProcessorCompilation(gtest)
endif()

# Add molten core
if (NOT TARGET MoltenCore)
	add_subdirectory(${CoreDir} ${CoreDir} EXCLUDE_FROM_ALL)
endif() 

add_executable(MoltenCoreTests "${TestMainSource}" "${CoreTestSources}")
target_include_directories(MoltenCoreTests PUBLIC "${TestIncludeDir}")
target_include_directories(MoltenCoreTests PUBLIC "${CoreHeadersDir}")

# Configure defaults.
SetDefaultCompileOptions(MoltenCoreTests)
EnableMultiProcessorCompilation(MoltenCoreTests)
CreateSourceGroups("${CoreTestSources}" "${CoreTestSourceDir}")

set_target_properties( MoltenCoreTests
  PROPERTIES
  OUTPUT_NAME_DEBUG "MoltenCoreTestsDebug"
  OUTPUT_NAME_RELEASE "MoltenCoreTests"
  RUNTIME_OUTPUT_DIRECTORY "${RootDir}/Bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${RootDir}/Bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${RootDir}/Bin"
)

SetVisualStudioWorkingDir("MoltenCoreTests" "${RootDir}/Bin")

# Linking and setting compile definitions.
if(MOLTEN_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU") ## Code coverage
  set_target_properties(MoltenCoreTests PROPERTIES COMPILE_FLAGS "--coverage")
  target_link_libraries(MoltenCoreTests "gcov")
endif()

target_link_libraries(MoltenCoreTests MoltenCore gtest_main)

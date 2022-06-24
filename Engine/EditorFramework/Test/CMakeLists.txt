cmake_minimum_required(VERSION 3.16)
if(POLICY CMP0092)
  cmake_policy(SET CMP0092 NEW)
endif()

project (MoltenEditorFrameworkTests)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../CMake/Tools.cmake)

option(MOLTEN_ENABLE_COVERAGE "Enables coverage reporting" OFF)

find_package(Threads)

# Get directories and files.
set(RootDir "${CMAKE_CURRENT_SOURCE_DIR}/../../..")
set(EngineDir "${RootDir}/Engine")
set(CoreDir "${EngineDir}/Core")
set(CoreHeadersDir "${CoreDir}/Headers")
set(EditorFrameworkDir "${EngineDir}/EditorFramework")
set(EditorFrameworkHeadersDir "${EditorFrameworkDir}/Headers")
set(ThirdPartyDir "${EngineDir}/Vendor/ThirdParty")
set(EditorFrameworkTestDir "${EditorFrameworkDir}/Test")
set(EditorFrameworkTestSourceDir "${EditorFrameworkTestDir}/Source")
set(TestDir "${EngineDir}/Test")
set(TestIncludeDir "${TestDir}/Headers/Molten/Test")

set(TestMainSource "${TestDir}/Source/Molten/Test/TestMain.cpp")
file(GLOB_RECURSE TestSources "${EditorFrameworkTestSourceDir}/*.c" "${EditorFrameworkTestSourceDir}/*.cpp")

# Add google test
if (NOT TARGET gtest)
	SET(gtest_force_shared_crt on CACHE BOOL
		"Use shared (DLL) run-time lib even when Google Test is built as static lib.")
	add_subdirectory(${ThirdPartyDir}/googletest ${ThirdPartyDir}/googletest EXCLUDE_FROM_ALL)
	RemoveWarningAsError(gtest)
	EnableMultiProcessorCompilation(gtest)
endif()

# Add molten core
if (NOT TARGET MoltenEditorFramework)
	add_subdirectory(${EditorFrameworkDir} ${EditorFramework} EXCLUDE_FROM_ALL)
endif() 

add_executable(MoltenEditorFrameworkTests "${TestMainSource}" "${TestSources}")
target_include_directories(MoltenEditorFrameworkTests PUBLIC "${TestIncludeDir}")
target_include_directories(MoltenEditorFrameworkTests PUBLIC "${CoreHeadersDir}")
target_include_directories(MoltenEditorFrameworkTests PUBLIC "${EditorFrameworkHeadersDir}")

# Configure defaults.
SetDefaultCompileOptions(MoltenEditorFrameworkTests)
EnableMultiProcessorCompilation(MoltenEditorFrameworkTests)
CreateSourceGroups("${TestSources}" "${EditorFrameworkTestSourceDir}")

set_target_properties( MoltenEditorFrameworkTests
  PROPERTIES
  OUTPUT_NAME_DEBUG "MoltenEditorFrameworkTestsDebug"
  OUTPUT_NAME_RELEASE "MoltenEditorFrameworkTests"
  RUNTIME_OUTPUT_DIRECTORY "${RootDir}/Bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${RootDir}/Bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${RootDir}/Bin"
)

SetVisualStudioWorkingDir("MoltenEditorFrameworkTests" "${RootDir}/Bin")

# Linking and setting compile definitions.
if(MOLTEN_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU") ## Code coverage
  set_target_properties(MoltenEditorFrameworkTests PROPERTIES COMPILE_FLAGS "--coverage")
  target_link_libraries(MoltenEditorFrameworkTests "gcov")
endif()

target_link_libraries(MoltenEditorFrameworkTests MoltenEditorFramework gtest_main)

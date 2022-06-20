cmake_minimum_required(VERSION 3.16)
if(POLICY CMP0092)
  cmake_policy(SET CMP0092 NEW)
endif()

project (MoltenGraphicsTests)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../CMake/Tools.cmake)

option(MOLTEN_ENABLE_COVERAGE "Enables coverage reporting" OFF)

find_package(Threads)

# Get directories and files.
set(RootDir "${CMAKE_CURRENT_SOURCE_DIR}/../../..")
set(EngineDir "${RootDir}/Engine")
set(CoreDir "${EngineDir}/Core")
set(CoreHeadersDir "${CoreDir}/Headers")
set(GraphicsDir "${EngineDir}/Graphics")
set(GraphicsHeadersDir "${GraphicsDir}/Headers")
set(ThirdPartyDir "${EngineDir}/Vendor/ThirdParty")
set(GraphicsTestDir "${GraphicsDir}/Test")
set(GraphicsTestSourceDir "${GraphicsTestDir}/Source")
set(TestDir "${EngineDir}/Test")
set(TestIncludeDir "${TestDir}/Headers/Molten/Test")

set(TestMainSource "${TestDir}/Source/Molten/Test/TestMain.cpp")
file(GLOB_RECURSE GraphicsTestSources "${GraphicsTestSourceDir}/*.c" "${GraphicsTestSourceDir}/*.cpp")

# Add google test
if (NOT TARGET gtest)
	SET(gtest_force_shared_crt on CACHE BOOL
		"Use shared (DLL) run-time lib even when Google Test is built as static lib.")
	add_subdirectory(${ThirdPartyDir}/googletest ${ThirdPartyDir}/googletest EXCLUDE_FROM_ALL)
	RemoveWarningAsError(gtest)
	EnableMultiProcessorCompilation(gtest)
endif()

# Add molten core
if (NOT TARGET MoltenGraphics)
	add_subdirectory(${GraphicsDir} ${GraphicsDir} EXCLUDE_FROM_ALL)
endif() 

add_executable(MoltenGraphicsTests "${TestMainSource}" "${GraphicsTestSources}")
target_include_directories(MoltenGraphicsTests PUBLIC "${TestIncludeDir}")
target_include_directories(MoltenGraphicsTests PUBLIC "${CoreHeadersDir}")
target_include_directories(MoltenGraphicsTests PUBLIC "${GraphicsHeadersDir}")

# Configure defaults.
SetDefaultCompileOptions(MoltenGraphicsTests)
EnableMultiProcessorCompilation(MoltenGraphicsTests)
CreateSourceGroups("${GraphicsTestSources}" "${GraphicsTestSourceDir}")

set_target_properties( MoltenGraphicsTests
  PROPERTIES
  OUTPUT_NAME_DEBUG "MoltenGraphicsTestsDebug"
  OUTPUT_NAME_RELEASE "MoltenGraphicsTests"
  RUNTIME_OUTPUT_DIRECTORY "${RootDir}/Bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${RootDir}/Bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${RootDir}/Bin"
)

SetVisualStudioWorkingDir("MoltenGraphicsTests" "${RootDir}/Bin")

# Linking and setting compile definitions.
if(MOLTEN_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU") ## Code coverage
  set_target_properties(MoltenGraphicsTests PROPERTIES COMPILE_FLAGS "--coverage")
  target_link_libraries(MoltenGraphicsTests "gcov")
endif()

target_link_libraries(MoltenGraphicsTests MoltenGraphics gtest_main)

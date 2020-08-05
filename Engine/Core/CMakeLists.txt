project (Molten)
cmake_minimum_required(VERSION 3.16)
include(${CMAKE_CURRENT_SOURCE_DIR}/../CMake/Tools.cmake)

# Options.
option(MOLTEN_ENABLE_VULKAN "Enables Vulkan renderer." ON)
option(MOLTEN_ENABLE_GLSLANG "Enables glslang compiler." ON)
option(MOLTEN_ENABLE_OPENGL "Enables OpenGL renderer." ON)
option(MOLTEN_ENABLE_COVERAGE "Enables coverage reporting." OFF)

# Set C++ settings.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

# Main directories and files.
set(RootDir "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(EngineDir "${RootDir}/Engine")
set(CoreDir "${EngineDir}/Core")
set(VendorDir "${EngineDir}/Vendor")
set(ThirdPartyDir "${EngineDir}/Vendor/ThirdParty")
set(CoreHeadersDir "${CoreDir}/Headers")
set(CoreSourceDir "${CoreDir}/Source")
file(GLOB_RECURSE CoreHeaders "${CoreHeadersDir}/*.h" "${CoreHeadersDir}/*.hpp" "${CoreHeadersDir}/*.inl")
file(GLOB_RECURSE CoreSources "${CoreSourceDir}/*.c" "${CoreSourceDir}/*.cpp")

# Linux configurations.
if(UNIX AND NOT APPLE)
	option(MOLTEN_ENABLE_X11 "Enables X server." ON)

  set(LINUX TRUE)
	find_package(Threads)
	
	if(MOLTEN_ENABLE_OPENGL AND (NOT MOLTEN_ENABLE_X11))
		message(FATAL_ERROR "X11(MOLTEN_ENABLE_X11) is required if OpenGL(MOLTEN_ENABLE_OPENGL) is enabled.")
	endif()
else()
	set(MOLTEN_ENABLE_X11 OFF)
endif()

# Glslang configurations
if(MOLTEN_ENABLE_GLSLANG)
  include(${CMAKE_CURRENT_SOURCE_DIR}/../CMake/FindGlslang.cmake)
  option(BUILD_EXTERNAL "" OFF)
  option(ENABLE_HLSL "" OFF)
  option(ENABLE_CTEST "" OFF)
  option(ENABLE_SPVREMAPPER "" OFF)
  option(ENABLE_GLSLANG_BINARIES "" OFF)
  add_subdirectory("${Glslang_DIR}" "${Glslang_DIR}")
endif()

# Vulkan configurations
if(MOLTEN_ENABLE_VULKAN)
	include(${CMAKE_CURRENT_SOURCE_DIR}/../CMake/FindVulkan.cmake)  
endif()

# OpenGL configurations
if(MOLTEN_ENABLE_OPENGL)
	find_package(OpenGL)
endif()

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


include_directories ("${CoreHeadersDir}")
include_directories ("${ThirdPartyDir}")
add_library(Molten SHARED "${CoreSources}" "${CoreHeaders}")  

CreateSourceGroups("${CoreSources}" "${CoreSourceDir}")
CreateSourceGroups("${CoreHeaders}" "${CoreHeadersDir}")

target_compile_definitions(Molten PRIVATE MOLTEN_EXPORT)
install(TARGETS Molten DESTINATION "${RootDir}/Lib") # Install library
set_target_properties( Molten
  PROPERTIES
  OUTPUT_NAME_DEBUG "MoltenDebug"
  OUTPUT_NAME_RELEASE "Molten"
  ARCHIVE_OUTPUT_DIRECTORY "${RootDir}/Lib"
  ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${RootDir}/Lib"
  ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${RootDir}/Lib"
  LIBRARY_OUTPUT_DIRECTORY "${RootDir}/Lib"
  LIBRARY_OUTPUT_DIRECTORY_DEBUG "${RootDir}/Lib"
  LIBRARY_OUTPUT_DIRECTORY_RELEASE "${RootDir}/Lib"
  RUNTIME_OUTPUT_DIRECTORY "${RootDir}/Bin"
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${RootDir}/Bin"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${RootDir}/Bin"
)

# Linking and setting compile definitions.
if(MOLTEN_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set_target_properties(Molten PROPERTIES COMPILE_FLAGS "-lgcov --coverage")
  target_link_libraries(Molten "gcov")
endif()

if(MOLTEN_ENABLE_VULKAN AND Vulkan_FOUND)
  target_include_directories(Molten PUBLIC "${Vulkan_INCLUDE}")
  #target_include_directories(Molten PUBLIC "${Glslang_INCLUDE}")
  target_link_libraries(Molten "${Vulkan_LIBRARY}")
  #target_link_libraries(Molten ${Glslang_LIBRARY})
  #target_link_libraries(Molten ${SPIRV_LIBRARY})
  target_compile_definitions(Molten PUBLIC "MOLTEN_ENABLE_VULKAN")
  #target_compile_definitions(Molten PUBLIC "MOLTEN_ENABLE_GLSLANG")
endif()

if(MOLTEN_ENABLE_GLSLANG AND Glslang_FOUND)
  target_link_libraries(Molten "${Glslang_LIBRARY}" "${SPIRV_LIBRARY}")  
	target_compile_definitions(Molten PUBLIC "MOLTEN_ENABLE_GLSLANG")
endif()

if(MOLTEN_ENABLE_OPENGL AND OpenGL_FOUND)
	target_link_libraries(Molten "${OPENGL_LIBRARIES}")
	target_compile_definitions(Molten PUBLIC "MOLTEN_ENABLE_OPENGL")
endif()

if(MOLTEN_ENABLE_X11)
  target_link_libraries(Molten "X11")
	target_compile_definitions(Molten PUBLIC "MOLTEN_ENABLE_X11")
endif()

target_include_directories(Molten PUBLIC "${VendorDir}")


# Print summary
message("Found flag summary: ")
message("Vulkan_FOUND:  ${Vulkan_FOUND}  ")
message("Glslang_FOUND: ${Glslang_FOUND}")
message("OpenGL_FOUND:  ${OpenGL_FOUND}")

if(Glslang_FOUND)
  message("Vulkan_INCLUDE: ${Vulkan_INCLUDE}")
  message("Vulkan_LIBRARY: ${Vulkan_LIBRARY}")
endif()

if(Glslang_FOUND)
  message("Glslang_LIBRARY: ${Glslang_LIBRARY}")
  message("SPIRV_LIBRARY: ${SPIRV_LIBRARY}")
endif()

if(Glslang_FOUND)
  message("OPENGL_LIBRARIES: ${OPENGL_LIBRARIES}")
endif()
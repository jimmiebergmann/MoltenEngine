project (Curse)
cmake_minimum_required(VERSION 3.1)
include(${CMAKE_CURRENT_SOURCE_DIR}/../CMake/Tools.cmake)


# Options.
option(CURSE_ENABLE_VULKAN "Enables OpenGL." ON)
option(CURSE_ENABLE_OPENGL "Enables OpenGL." ON)
option(CURSE_ENABLE_COVERAGE "Enables coverage reporting." OFF)


# Linux configurations.
if(UNIX AND NOT APPLE)
	option(CURSE_ENABLE_X11 "Enables X server." ON)

	set(LINUX TRUE)
	find_package(Threads)
	
	if(CURSE_ENABLE_OPENGL AND (NOT CURSE_ENABLE_X11))
		message(FATAL_ERROR "X11(CURSE_ENABLE_X11) is required if OpenGL(CURSE_ENABLE_OPENGL) is enabled.")
	endif()
else()
	set(CURSE_ENABLE_X11 OFF)
endif()

# Vulkan configurations
if(CURSE_ENABLE_VULKAN)
	include(${CMAKE_CURRENT_SOURCE_DIR}/../CMake/FindVulkan.cmake)
	message("FOUND Vulkan: ${Vulkan_FOUND}, Lib: ${Vulkan_LIBRARY}")
endif()

# OpenGL configurations
if(CURSE_ENABLE_OPENGL)
	find_package(OpenGL)
	message("FOUND OPENGL: ${OpenGL_FOUND}, Lib: ${OPENGL_gl_LIBRARY}")
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


# Curse project generation
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

set(RootDir "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(CoreHeadersDir "${CMAKE_CURRENT_SOURCE_DIR}/Headers")
set(CoreSourceDir "${CMAKE_CURRENT_SOURCE_DIR}/Source")
file(GLOB_RECURSE CoreHeaders "${CoreHeadersDir}/*.h" "${CoreHeadersDir}/*.hpp" "${CoreHeadersDir}/*.inl")
file(GLOB_RECURSE CoreSources "${CoreSourceDir}/*.c" "${CoreSourceDir}/*.cpp")

message("Current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
message("Code coverage: ${CURSE_ENABLE_COVERAGE}")


#include_directories ("${CoreHeadersDir}")
add_library(Curse SHARED "${CoreSources}" "${CoreHeaders}")  
target_include_directories(Curse PUBLIC "${CoreHeadersDir}")

CreateSourceGroups("${CoreSources}" "${CoreSourceDir}")
CreateSourceGroups("${CoreHeaders}" "${CoreHeadersDir}")

target_compile_definitions(Curse PRIVATE CURSE_EXPORT)
install(TARGETS Curse DESTINATION "${RootDir}/Lib") # Install library
set_target_properties( Curse
  PROPERTIES
  OUTPUT_NAME_DEBUG "CurseDebug"
  OUTPUT_NAME_RELEASE "Curse"
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
if(CURSE_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set_target_properties(Curse PROPERTIES COMPILE_FLAGS "-lgcov --coverage")
  target_link_libraries(Curse "gcov")
endif()

if(CURSE_ENABLE_VULKAN AND Vulkan_FOUND )
	target_include_directories(Curse PUBLIC "${Vulkan_INCLUDE}")
	target_link_libraries(Curse ${Vulkan_LIBRARY})	
	target_compile_definitions(Curse PUBLIC "CURSE_ENABLE_VULKAN")
endif()

if(CURSE_ENABLE_OPENGL AND OpenGL_FOUND )
	target_link_libraries(Curse ${OPENGL_gl_LIBRARY})
	target_compile_definitions(Curse PUBLIC "CURSE_ENABLE_OPENGL")
endif()

if(CURSE_ENABLE_X11)
  target_link_libraries(Curse "X11")
	target_compile_definitions(Curse PUBLIC "CURSE_ENABLE_X11")
endif()

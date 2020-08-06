# Function for grouping files. Used for filters in Visual Studio.
function(CreateSourceGroups files rootPath)
  foreach(_source IN ITEMS ${files} )
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${rootPath}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
  endforeach()
endfunction(CreateSourceGroups)

# Function for setting working directory of Visual Studio project.
function(SetVisualStudioWorkingDir proj workingDir)
  if((${CMAKE_VERSION} VERSION_EQ "3.8.0") OR (${CMAKE_VERSION} VERSION_GREATER "3.8.0")) 
    set_property(TARGET ${proj} APPEND PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${workingDir}")
  endif()	
endfunction(SetVisualStudioWorkingDir)

# Set default compiler 
function(SetDefaultCompileOptions target)

  # Compiler flags.
  target_compile_options(${target} PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Gnu>,$<CXX_COMPILER_ID:Clang>>:
      $<IF:$<CONFIG:Debug>,
        -g  -Werror -Wall -Wextra -Wno-long-long, 
        -O3 -Werror -Wall -Wextra -Wno-long-long
      >
    >
    $<$<CXX_COMPILER_ID:MSVC>: 
      /W4 /WX
    >
  )

  # C++ standard version.
  set_target_properties(${target}
    PROPERTIES
      CXX_STANDARD 17
      CXX_STANDARD_REQUIRED OFF
      CXX_EXTENSIONS OFF
  )

endfunction(SetDefaultCompileOptions)


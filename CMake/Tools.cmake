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
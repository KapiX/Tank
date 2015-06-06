#
# - Haiku module for CMake
#
cmake_policy(SET CMP0026 OLD)

#
# Override the default add_executable() command and add our own.
#
function(ADD_EXECUTABLE TARGET)

	foreach(arg ${ARGN})
		if (${arg} MATCHES ".*rdef$")
			list(APPEND REAL_RDEFS ${arg})
		elseif(${arg} MATCHES ".*rsrc$")
			list(APPEND REAL_RSRCS ${arg})
		else(${arg} MATCHES ".*rdef$")
			list(APPEND REAL_SOURCES ${arg})
		endif(${arg} MATCHES ".*rdef$")
	endforeach(arg)

	# Call the original function with the filtered source list.
	_add_executable(${TARGET} ${REAL_SOURCES})

	# Check to make sure there are rdefs in the source file list.
	list(LENGTH REAL_RDEFS REAL_RDEFS_LENGTH)
	if (${REAL_RDEFS_LENGTH} GREATER 0)
		haiku_add_resource_defs(${TARGET} ${REAL_RDEFS})
	endif(${REAL_RDEFS_LENGTH} GREATER 0)

	# Check to make sure there are rsrcs in the source file list.
	list(LENGTH REAL_RSRCS REAL_RSRCS_LENGTH)
	if (${REAL_RSRCS_LENGTH} GREATER 0)
		haiku_add_resource_defs(${TARGET} ${REAL_RSRCS})
	endif(${REAL_RSRCS_LENGTH} GREATER 0)

	haiku_auto_mimetype(${TARGET})

endfunction(ADD_EXECUTABLE)

#
# Compile a resource definition file(.rdef) to a resource file(.rsrc)
#
function(HAIKU_COMPILE_RESOURCE_DEF RDEF_SOURCE)

	get_filename_component(rdefpath ${RDEF_SOURCE} ABSOLUTE)
	get_filename_component(rdeffile ${RDEF_SOURCE} NAME_WE)

	set(rsrcfile "${rdeffile}.rsrc")
	set(rsrcpath "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${rsrcfile}.dir/${rsrcfile}")

	add_custom_command(
		OUTPUT ${rsrcpath}
		COMMAND "rc" "-o" "${rsrcpath}" "${rdefpath}"
		DEPENDS ${rdefpath}
		COMMENT "Compiling resource ${rsrcpath}")

	add_custom_target(${rsrcfile} DEPENDS ${rsrcpath})

	set_source_files_properties(${rsrcfile} PROPERTIES GENERATED TRUE)

	set(rsrcpath "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${rsrcfile}.dir/${rsrcfile}" PARENT_SCOPE)

endfunction(HAIKU_COMPILE_RESOURCE_DEF)

#
# Compile resource definition files(.rdef) to resource files(.rsrc).
# Add resources from the .rsrc files to the target.
#
function(HAIKU_ADD_RESOURCE_DEFS TARGET)

	foreach(rdef ${ARGN})
		haiku_compile_resource_def(${rdef} rsrcpath)
		list(APPEND resources ${rsrcpath})
	endforeach(rdef ${ARGN})

	haiku_add_resources_internal(${TARGET} ${resources})

endfunction(HAIKU_ADD_RESOURCE_DEFS)



#
# Add resources from .rsrc files to the target.
#
function(HAIKU_ADD_RESOURCES TARGET)

	foreach(rsrc ${ARGN})
		list(APPEND rsrclist "${CMAKE_CURRENT_SOURCE_DIR}/${rsrc}")
	endforeach(rsrc ${ARGN})

	haiku_add_resources_internal(${TARGET} ${rsrclist})

endfunction(HAIKU_ADD_RESOURCES)



#
# Add resources from .rsrc files to the target.
# This is the internal version meant to be called from this module only.
#
function(HAIKU_ADD_RESOURCES_INTERNAL TARGET)

	get_target_property(TARGET_PATH ${TARGET} LOCATION)

	if(NOT TARGET_PATH)
		message(SEND_ERROR "Unable to determine target location for HAIKU_ADD_RESOURCE")
	endif(NOT TARGET_PATH)

	add_custom_command(
		TARGET ${TARGET}
		POST_BUILD
		COMMAND "xres" "-o" "${TARGET_PATH}" ${ARGN}
		COMMENT "Merging resources into ${TARGET_PATH}")

	foreach(rsrc ${ARGN})
		get_filename_component(rsrcfile ${rsrc} NAME)
	
		# FIXME: Need to fix the dependency so TARGET is rebuilt properly
		# and doesn't need this hack
		add_custom_target(
			${rsrcfile}_check
			COMMAND "/bin/sh" "-c" \"if \\[ \"${rsrc}\" -nt \"${TARGET_PATH}\" \\]\\;then rm -f \"${TARGET_PATH}\"\\;fi\")
	
		# Need this so that rsrcfile target is built before _check target
		add_dependencies(${rsrcfile}_check ${rsrcfile})
	
		add_dependencies(${TARGET} ${rsrcfile}_check)
	endforeach(rsrc ${ARGN})

endfunction(HAIKU_ADD_RESOURCES_INTERNAL)


#
# Run "mimeset" command to automatically set mimetype attributes using sniffer rules.
#
function(HAIKU_AUTO_MIMETYPE TARGET)

	get_target_property(TARGET_LOC ${TARGET} LOCATION)

	if(NOT TARGET_LOC)
		message(SEND_ERROR "Unable to determine target location for HAIKU_AUTO_MIMETYPE")
	endif(NOT TARGET_LOC)

	add_custom_command(
		TARGET ${TARGET}
		POST_BUILD
		COMMAND "mimeset" "-f" "${TARGET_LOC}"
		COMMENT "Setting mimetype for ${TARGET_LOC}")

endfunction(HAIKU_AUTO_MIMETYPE)

MACRO(CG_GET_TARGET_NAME)
  FILE(RELATIVE_PATH lib_name ${CMAKE_HOME_DIRECTORY}/exercises ${CMAKE_CURRENT_LIST_FILE})
  
  GET_FILENAME_COMPONENT(lib_name ${lib_name} PATH)
  FILE(TO_CMAKE_PATH ${lib_name} lib_name)
  STRING(REPLACE "/" "-" lib_name_plain ${lib_name})
  SET(target "${lib_name_plain}")
ENDMACRO()

MACRO(CG_CHECK_PACKAGE_DEPENDENCIES)
  SET(package_success TRUE)
  IF(external_depends)
    FOREACH(item ${external_depends})
    SET(item ${item}_FOUND)
      IF(package_success AND ${item})
        SET(package_success TRUE)
      ELSE()
        SET(package_success FALSE)
      ENDIF()
    ENDFOREACH()
  ENDIF()
ENDMACRO()

MACRO(CG_ADD_FOLDER_INCLUDE_HEADER)
  SET(DIR_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR})
  STRING(REPLACE "/" ";" DIR_INCLUDE ${DIR_INCLUDE})
  LIST(LENGTH DIR_INCLUDE DIR_INCLUDE_LENGTH)
  MATH(EXPR DIR_INCLUDE_LENGTH "${DIR_INCLUDE_LENGTH}-1")
  LIST(GET DIR_INCLUDE ${DIR_INCLUDE_LENGTH} FOLDER_HEADER_INCLUDE_NAME)
ENDMACRO()

MACRO(CG_GET_FOLDER_FILES)

  CG_ADD_FOLDER_INCLUDE_HEADER()
  FILE(GLOB_RECURSE header_files *.h *.hh *.hpp *.cuh ${FOLDER_HEADER_INCLUDE_NAME})  
  FILE(GLOB_RECURSE source_files *.cpp *.c *.cc *.cu)
  
ENDMACRO()
MACRO(CG_SET_LIBRARY_DEFINITIONS)
  FOREACH(item ${library_defs})
    SET_TARGET_PROPERTIES(${target} PROPERTIES COMPILE_DEFINITIONS ${item})
  ENDFOREACH()
ENDMACRO()

MACRO(CG_SET_INTERNAL_DEPENDENCIES)
IF(internal_depends)
  ADD_DEPENDENCIES(${target} ${internal_depends})
ENDIF()
ENDMACRO()

MACRO(CG_ADD_MODULE)
  ##get the target name, with prefix aura-#NAME#
    CG_GET_TARGET_NAME()

  ##check whether all dependencies have been found  
    CG_CHECK_PACKAGE_DEPENDENCIES()
    
  ##only build library if that is the case  
    IF(NOT package_success)
      MESSAGE("Not building " ${target} " because of missing dependencies")
      RETURN()
    ENDIF()
    
##get the headers, library sources and executable sources  
  CG_GET_FOLDER_FILES()
  
  #if user called add_module, find out whether exe for shared library is applicable
  IF("${compile_type}" STREQUAL "")
    FOREACH(item ${source_files})
      FILE(READ ${item} file_content)
      IF(file_content MATCHES "[ \t\n]+main[ \t\n]*\\(")
        SET(compile_type exe)
      ENDIF()
    ENDFOREACH() 
  ENDIF()
  IF("${compile_type}" STREQUAL "exe")
    MESSAGE(STATUS "Building executable " ${target}) 
  ELSE()
    MESSAGE(STATUS "Building shared library " ${target}) 
  ENDIF()
  
  IF("${header_files}" STREQUAL "" AND "${source_files}" STREQUAL "")
    MESSAGE("Not building library because no source files were given.")
    RETURN()
  ENDIF()  
  
  IF("${compile_type}" STREQUAL "exe")
    ADD_EXECUTABLE(${target} ${header_files} ${source_files})
  ELSE()
  ADD_LIBRARY(${target} SHARED ${header_files} ${source_files})
  ENDIF()
 # ADD_LIBRARY(${target} SHARED ${header_files} ${source_files})
  
  CG_SET_INTERNAL_DEPENDENCIES()
  
  TARGET_LINK_LIBRARIES(${target} ${link_libs}) 
  INCLUDE_DIRECTORIES(${include_dirs})
ENDMACRO()

MACRO(CG_ADD_EXECUTABLE)
  SET(compile_type exe)
  CG_ADD_MODULE()
ENDMACRO()

MACRO(CG_ADD_SHARED_LIBARY)
  SET(compile_type dll)
  CG_ADD_MODULE()
ENDMACRO()

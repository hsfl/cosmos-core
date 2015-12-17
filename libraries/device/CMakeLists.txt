#cmake_minimum_required(VERSION 2.8.12)

#MACRO(SUBDIRLIST result curdir)
#  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
#  SET(dirlist "")
#  FOREACH(child ${children})
#    IF(IS_DIRECTORY ${curdir}/${child})
#        LIST(APPEND dirlist ${child})
#    ENDIF()
#  ENDFOREACH()
#  SET(${result} ${dirlist})
#ENDMACRO()

## go into every folder to find more cmakelists
#SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

#message("-------------------------")
#message("-------------------------")
#message("add each device library into the main CosmosDevice lib:")


#file(GLOB directs "./*")

#foreach(direct ${directs})
#    get_filename_component(bext ${direct} EXT)

#    #message("${direct} ${bext}")
#    if (${bext} MATCHES "txt")

#    elseif (${bext} MATCHES "DS_Store")

#    elseif (${bext} MATCHES "qbs")

#    else()
#        get_filename_component(bdirect ${direct} NAME_WE)
#        add_subdirectory(${PROJECT_SOURCE_DIR}/programs/${bdirect})
#    endif()

#endforeach()

# we can also be specific about these folders since they do not change very often
if(${BUILD_PROGRAMS_AGENTS} MATCHES "YES")
    add_subdirectory(agents)
endif()

if(${BUILD_PROGRAMS_GENERAL} MATCHES "YES")
    add_subdirectory(general)
endif()

if(${BUILD_PROGRAMS_TESTS} MATCHES "YES")
    add_subdirectory(tests)
endif()

if(${BUILD_PROGRAMS_UNIX_TESTS} MATCHES "YES" AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_subdirectory(unix_tests)
endif()

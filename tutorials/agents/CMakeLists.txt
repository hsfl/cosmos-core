add_subdirectory(agent_001)
add_subdirectory(agent_002)
add_subdirectory(agent_008)

#foreach(direct ${directs})
#    get_filename_component(bext ${direct} EXT)

#    #message("${direct} ${bext}")
#    if (${bext} MATCHES "txt")

#    elseif (${bext} MATCHES "DS_Store")

#    elseif (${bext} MATCHES "qbs")

#    else()
#        get_filename_component(bdirect ${direct} NAME_WE)
#        add_subdirectory(${PROJECT_SOURCE_DIR}/agents/${bdirect})
#    endif()

#endforeach()

#add_subdirectory(agent_001)
#add_subdirectory(agent_002)
#add_subdirectory(agent_004)
#add_subdirectory(agent_008)
#add_subdirectory(agent_005)

file(GLOB directs "./*")
foreach(direct ${directs})
    get_filename_component(bext ${direct} EXT)

    if (${bext} MATCHES "")

    else()
        get_filename_component(bdirect ${direct} NAME_WE)
        add_subdirectory(${bdirect})
        message("Adding " ${bdirect})
    endif()

endforeach()

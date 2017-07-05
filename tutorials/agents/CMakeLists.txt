<<<<<<< HEAD
add_subdirectory(agent_001)
add_subdirectory(agent_002)
add_subdirectory(agent_006)
add_subdirectory(agent_004)
=======
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
>>>>>>> dccac9dc5af3d207728361847d1d04adbd558e3e

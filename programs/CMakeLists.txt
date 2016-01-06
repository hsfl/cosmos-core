file(GLOB directs "./*")

foreach(direct ${directs})
    get_filename_component(bext ${direct} EXT)

    #message("${direct} ${bext}")
    if (${bext} MATCHES "txt")

    elseif (${bext} MATCHES "DS_Store")

    elseif (${bext} MATCHES "qbs")

    else()
        get_filename_component(bdirect ${direct} NAME_WE)
        add_subdirectory(${PROJECT_SOURCE_DIR}/programs/${bdirect})
    endif()

endforeach()


# to make it faster add only the agents folder
# add_subdirectory(agents)

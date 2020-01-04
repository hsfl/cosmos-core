# a complete build will build all the essential libraries and programs
# but also every acessory program in core
# COMPLETE - everything is built
# BASIC    - only the essential libraries and prgram agents are build
# MINIMAL  - only the libraries are built (no agents, no programs)
set(CROSS_TYPE x86_64)
set(BUILD_TYPE COMPLETE) # COMPLETE, BASIC, MINIMAL, TESTS
set(BUILD_AGENTS_TYPE ALL)

# Build agents type list
# CORE - build the essential agents for COSMOS/core (default)
# NONE - doesn't build any agent
# GS   - builds ground station agents
if(${BUILD_TYPE} MATCHES "COMPLETE")
    set(BUILD_PROGRAMS YES) # YES, NO
    set(BUILD_PROGRAMS_AGENTS YES) # YES, NO
    set(BUILD_PROGRAMS_AGENTS_TYPE CORE) # CORE, ALL, GS (ground station)
    set(BUILD_PROGRAMS_GENERAL YES) # YES, NO
    set(BUILD_PROGRAMS_TESTS YES)
    set(BUILD_PROGRAMS_UNIX_TESTS YES)
    set(BUILD_PROGRAMS_DEVICES YES) # YES, NO

    set(BUILD_TUTORIALS YES) # YES, NO
endif()

if(${BUILD_TYPE} MATCHES "BASIC")
    set(BUILD_PROGRAMS YES) # YES, NO
    set(BUILD_PROGRAMS_AGENTS YES) # YES, NO
    set(BUILD_PROGRAMS_AGENTS_TYPE CORE) # CORE, ALL, GS (ground station)
    set(BUILD_PROGRAMS_GENERAL NO) # YES, NO
    set(BUILD_PROGRAMS_UNIX_TESTS NO)
    set(BUILD_PROGRAMS_DEVICES NO) # YES, NO

    set(BUILD_TUTORIALS NO) # YES, NO
endif()

if(${BUILD_TYPE} MATCHES "MINIMAL")
    set(BUILD_PROGRAMS NO) # YES, NO
    set(BUILD_PROGRAMS_AGENTS NO) # YES, NO
    set(BUILD_PROGRAMS_AGENTS_TYPE CORE) # CORE, ALL, GS (ground station)
    set(BUILD_PROGRAMS_GENERAL NO) # YES, NO
    set(BUILD_PROGRAMS_UNIX_TESTS NO)
    set(BUILD_PROGRAMS_DEVICES NO) # YES, NO

    set(BUILD_TUTORIALS NO) # YES, NO
endif()

if(${BUILD_TYPE} MATCHES "TESTS")
    set(BUILD_TESTS YES) # YES, NO
    set(BUILD_PROGRAMS NO) # YES, NO
    set(BUILD_PROGRAMS_AGENTS NO) # YES, NO
    set(BUILD_PROGRAMS_AGENTS_TYPE CORE) # CORE, ALL, GS (ground station)
    set(BUILD_PROGRAMS_GENERAL NO) # YES, NO
    set(BUILD_TUTORIALS NO) # YES, NO
    set(BUILD_PROGRAMS_UNIX_TESTS YES)
    set(BUILD_PROGRAMS_DEVICES YES) # YES, NO
endif()

#set(BUILD_LIBRARIES all)

# available options: all, arduino, astrodev cpu disk general i2c serial vn100
# if the BUILD_LIBRARIES_DEVICES was not set then include all device libraries by default
if (NOT BUILD_LIBRARIES_DEVICES)
    set(BUILD_LIBRARIES_DEVICES i2c)
endif()

# do you want to deploy all the files to the remote device?
set(DEPLOY_FILES_TO_REMOTE no)
set(DEPLOY_SOURCE_FILES_TO_REMOTE no)

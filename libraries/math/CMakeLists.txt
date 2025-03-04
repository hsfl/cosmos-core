cmake_minimum_required(VERSION 3.20)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(CosmosMath STATIC ${SOURCES} ${INCLUDES})
target_link_libraries(CosmosMath CosmosSupport ${COSMOS_LIBS})

install(TARGETS CosmosMath DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/math)


if(${DEPLOY_SRC_FILES_TO_REMOTE} MATCHES "YES")
    add_deployment_source_directory(${CMAKE_CURRENT_SOURCE_DIR} "src")
endif()



# project(vector_bindings)

# # Python bindings setup
# find_package(pybind11 REQUIRED)
# find_package(Python3 REQUIRED COMPONENTS Development)

# # Create a shared library for the vector module
# include_directories(${Python3_INCLUDE_DIRS})
# add_library(vector_lib vector.cpp)
# pybind11_add_module(Vectors bindings.cpp)
# target_link_libraries(Vectors PRIVATE vector_lib ${Python3_LIBRARIES})

# # Install the Python module
# install(TARGETS Vectors DESTINATION source/core/python/modules)

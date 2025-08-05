set(SOURCES
    UdpSender.cpp
    FileSender.cpp
)

set(INCLUDES
    UdpSender.h
    FileSender.h
)

add_library(FileSenderImpl STATIC
    ${SOURCES}
    ${INCLUDES}
)
target_link_libraries(FileSenderImpl CosmosTransfer CosmosNetwork)

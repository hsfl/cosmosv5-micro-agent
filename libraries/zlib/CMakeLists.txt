cmake_minimum_required(VERSION 3.20)

# zlib — zlib license
# Bundled directly in cosmosv5-micro-agent so micro-agent has no thirdparty dependency.
# License notice must be preserved in all distributions (see zlib.h header).
FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(localzlib STATIC ${SOURCES} ${INCLUDES})
target_include_directories(localzlib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS localzlib DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/micro-agent/zlib)

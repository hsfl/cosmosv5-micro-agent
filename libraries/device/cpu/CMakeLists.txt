cmake_minimum_required(VERSION 3.20)

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(CosmosDeviceCpu STATIC ${SOURCES} ${INCLUDES})
target_link_libraries(CosmosDeviceCpu CosmosData CosmosString CosmosSupport ${COSMOS_LIBS})

install(TARGETS CosmosDeviceCpu DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/device/cpu)

message("Build CosmosDeviceCpu from ${CMAKE_CURRENT_SOURCE_DIR}")

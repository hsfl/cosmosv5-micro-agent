cmake_minimum_required(VERSION 3.20)

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(CosmosDeviceArduino STATIC ${SOURCES} ${INCLUDES})
target_link_libraries(CosmosDeviceArduino CosmosCssl CosmosTime CosmosSupport ${COSMOS_LIBS})

install(TARGETS CosmosDeviceArduino DESTINATION lib)
install(FILES ${INCLUDES} DESTINATION include/device/arduino)

message("Build CosmosDeviceArduino from ${CMAKE_CURRENT_SOURCE_DIR}")

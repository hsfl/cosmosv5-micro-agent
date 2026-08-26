cmake_minimum_required(VERSION 3.20)

FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# Serial comm library (depends only on Kernel: configCosmos + sliplib)
add_library(CosmosCssl STATIC cssl_lib.cpp cssl_lib.h)
target_link_libraries(CosmosCssl CosmosSlip CosmosSupport ${COSMOS_LIBS})
install(TARGETS CosmosCssl DESTINATION lib)

# BeagleBone/BBB GPIO functions
add_library(CosmosBBFctns STATIC bbFctns.cpp bbFctns.h)
target_link_libraries(CosmosBBFctns CosmosSupport ${COSMOS_LIBS})
install(TARGETS CosmosBBFctns DESTINATION lib)

# PIC microcontroller comm
add_library(CosmosPicLib STATIC pic_lib.cpp pic_lib.h)
target_link_libraries(CosmosPicLib CosmosCssl CosmosSupport ${COSMOS_LIBS})
install(TARGETS CosmosPicLib DESTINATION lib)

install(FILES ${INCLUDES} DESTINATION include/device/general)

message("Build micro-agent device/general from ${CMAKE_CURRENT_SOURCE_DIR}")

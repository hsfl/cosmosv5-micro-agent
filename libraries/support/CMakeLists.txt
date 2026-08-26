cmake_minimum_required(VERSION 3.20)

FILE(GLOB INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# Data I/O: file, ring buffer, zlib compression
add_library(CosmosData STATIC datalib.cpp datalib.h)
target_link_libraries(CosmosData CosmosTime CosmosString localzlib CosmosSupport ${COSMOS_LIBS})
install(TARGETS CosmosData DESTINATION lib)

# Network: UDP/TCP socket abstraction
add_library(CosmosNetwork STATIC socketlib.cpp socketlib.h)
target_link_libraries(CosmosNetwork CosmosTime CosmosMath CosmosSupport ${COSMOS_LIBS})
install(TARGETS CosmosNetwork DESTINATION lib)

# Logging (depends on CosmosNetwork + CosmosData)
add_library(CosmosLog STATIC logger.cpp logger.h)
target_link_libraries(CosmosLog CosmosNetwork CosmosData CosmosTime CosmosString CosmosJson CosmosSupport ${COSMOS_LIBS})
install(TARGETS CosmosLog DESTINATION lib)

# File transfer protocol (no agentclass dep; transferclass goes in agent)
add_library(CosmosTransferLib STATIC transferlib.cpp transferlib.h)
target_link_libraries(CosmosTransferLib CosmosPacket CosmosData CosmosSupport ${COSMOS_LIBS})
install(TARGETS CosmosTransferLib DESTINATION lib)

install(FILES ${INCLUDES} DESTINATION include/support)

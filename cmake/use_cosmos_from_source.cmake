if(NOT DEFINED COSMOS_MICRO_AGENT_INCLUDED)
set(COSMOS_MICRO_AGENT_INCLUDED TRUE)

include(${COSMOS_SOURCE}/kernel/cmake/use_cosmos_from_source.cmake)

set(COSMOS_SOURCE_MICRO_AGENT ${COSMOS_SOURCE}/micro-agent)
message("Using COSMOS micro-agent from " ${COSMOS_SOURCE_MICRO_AGENT})

include_directories(${COSMOS_SOURCE_MICRO_AGENT}/libraries)
include_directories(${COSMOS_SOURCE_MICRO_AGENT}/libraries/device)

add_subdirectory(${COSMOS_SOURCE_MICRO_AGENT}/libraries/support              ${CMAKE_BINARY_DIR}/micro-agent/libraries/support)
add_subdirectory(${COSMOS_SOURCE_MICRO_AGENT}/libraries/device/cpu           ${CMAKE_BINARY_DIR}/micro-agent/libraries/device/cpu)
add_subdirectory(${COSMOS_SOURCE_MICRO_AGENT}/libraries/device/arduino       ${CMAKE_BINARY_DIR}/micro-agent/libraries/device/arduino)
add_subdirectory(${COSMOS_SOURCE_MICRO_AGENT}/libraries/device/general       ${CMAKE_BINARY_DIR}/micro-agent/libraries/device/general)

endif()

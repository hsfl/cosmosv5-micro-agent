if(NOT DEFINED COSMOS_MICRO_AGENT_INCLUDED)
set(COSMOS_MICRO_AGENT_INCLUDED TRUE)

get_filename_component(COSMOS_SOURCE_MICRO_AGENT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(DEFINED COSMOS_SOURCE)
    # Auto-initialize kernel
    if(NOT EXISTS "${COSMOS_SOURCE}/kernel/CMakeLists.txt")
        message(STATUS "cosmosv5: initializing kernel submodule...")
        execute_process(
            COMMAND git submodule update --init kernel
            WORKING_DIRECTORY "${COSMOS_SOURCE}"
            RESULT_VARIABLE _cosmos_git_result
        )
        if(NOT _cosmos_git_result EQUAL 0)
            message(FATAL_ERROR
                "cosmosv5: kernel submodule is not initialized and auto-init failed.\n"
                "Run from the cosmosv5 workspace root:\n"
                "  git submodule update --init kernel")
        endif()
    endif()
    include(${COSMOS_SOURCE}/kernel/cmake/use_cosmos_from_source.cmake)
else()
    message(FATAL_ERROR "COSMOS_SOURCE not set. Set COSMOS_SOURCE to the cosmosv5 workspace root (the directory containing thirdparty/, kernel/, etc.).")
endif()

message("Using COSMOS micro-agent from " ${COSMOS_SOURCE_MICRO_AGENT})

# Build zlib from micro-agent's bundled copy (issue #86).
# Sets COSMOS_ZLIB_INCLUDE_DIR so thirdparty/png can find zlib.h when included by modules+.
if(NOT TARGET localzlib)
    add_subdirectory(${COSMOS_SOURCE_MICRO_AGENT}/libraries/zlib ${CMAKE_BINARY_DIR}/micro-agent/libraries/zlib)
endif()
set(COSMOS_ZLIB_INCLUDE_DIR "${COSMOS_SOURCE_MICRO_AGENT}/libraries/zlib" CACHE PATH "zlib include directory (for libpng)")
include_directories(${COSMOS_SOURCE_MICRO_AGENT}/libraries/zlib)

include_directories(${COSMOS_SOURCE_MICRO_AGENT}/libraries)
include_directories(${COSMOS_SOURCE_MICRO_AGENT}/libraries/device)

add_subdirectory(${COSMOS_SOURCE_MICRO_AGENT}/libraries/support              ${CMAKE_BINARY_DIR}/micro-agent/libraries/support)
add_subdirectory(${COSMOS_SOURCE_MICRO_AGENT}/libraries/device/cpu           ${CMAKE_BINARY_DIR}/micro-agent/libraries/device/cpu)
add_subdirectory(${COSMOS_SOURCE_MICRO_AGENT}/libraries/device/arduino       ${CMAKE_BINARY_DIR}/micro-agent/libraries/device/arduino)
add_subdirectory(${COSMOS_SOURCE_MICRO_AGENT}/libraries/device/general       ${CMAKE_BINARY_DIR}/micro-agent/libraries/device/general)

endif()

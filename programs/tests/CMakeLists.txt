cmake_minimum_required(VERSION 3.20)

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR} : ")
message(${files})

foreach(file ${files})
    get_filename_component(bfile ${file} NAME_WE)
    add_executable(${bfile} ${bfile}.cpp)
    install(TARGETS ${bfile} DESTINATION bin)
endforeach()

target_link_libraries(errortest CosmosTime CosmosLog)
target_link_libraries(log_move_test CosmosData)
target_link_libraries(mathspeed CosmosDeviceCpu CosmosTime)

if(${BUILD_PROGRAMS_TESTS} MATCHES "YES")
    add_subdirectory(unit_tests)
endif()

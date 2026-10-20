cmake_minimum_required(VERSION 3.21)
project(ACPR_InputHistory VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_library(acpr_input_history SHARED
    source/inputHistory.cpp
)
target_include_directories(acpr_input_history PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
set_target_properties(acpr_input_history PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/ACPR_InputHistory
    PREFIX ""
    OUTPUT_NAME "ACPR_InputHistory"
)
target_link_options(acpr_input_history PRIVATE -static)

add_custom_command(
    OUTPUT ${BIN_DIR}/ACPR_InputHistory/config.json
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR}/ACPR_InputHistory
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/source/config.json
        ${BIN_DIR}/ACPR_InputHistory/config.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/config.json
    COMMENT "Copying ACPR_InputHistory config.json"
)
add_custom_target(copy_input_history_config ALL
    DEPENDS ${BIN_DIR}/ACPR_InputHistory/config.json
)

add_library(commandParser INTERFACE)

target_include_directories(commandParser INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(commandParser INTERFACE
    CLI11::CLI11
)
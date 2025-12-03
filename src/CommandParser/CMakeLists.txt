add_library(commandParser
    include/CommandParser.h
)

target_include_directories(commandParser PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

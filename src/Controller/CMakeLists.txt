add_library(controller
    src/Controller.cpp
    include/Controller.h
)

target_include_directories(controller PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(controller PUBLIC
    fileManager
    commandParser
)

target_link_libraries(controller PRIVATE
    fmt::fmt 
    tabulate::tabulate
)
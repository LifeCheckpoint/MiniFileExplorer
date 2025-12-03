add_library(controller
    src/Controller.cpp
)

target_link_libraries(controller PUBLIC
    fileManager
    commandParser
)

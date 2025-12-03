add_subdirectory(Models)
add_subdirectory(FileManager)
add_subdirectory(CommandParser)
add_subdirectory(Controller)

add_executable(MiniFileExplorer main.cpp)

target_link_libraries(MiniFileExplorer PUBLIC
    fileManager
    models
    commandParser
    controller
)

target_link_libraries(MiniFileExplorer PUBLIC
    fmt::fmt 
    tabulate::tabulate 
    replxx 
)

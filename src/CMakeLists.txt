add_subdirectory(Models)
add_subdirectory(FileManager)

add_executable(MiniFileExplorer main.cpp)

target_link_libraries(MiniFileExplorer PUBLIC
    FileManager
    Models
)

target_link_libraries(MiniFileExplorer PUBLIC
    fmt::fmt 
    tabulate::tabulate 
    replxx 
)

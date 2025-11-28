add_library(FileManager
    src/FileManager.cpp
    include/FileManager.h
)

target_include_directories(FileManager PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(FileManager PUBLIC 
    models
)
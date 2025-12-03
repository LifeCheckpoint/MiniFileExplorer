add_library(fileManager
    src/FileManager.cpp
    include/FileManager.h
)

target_include_directories(fileManager PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(fileManager PUBLIC 
    models
)
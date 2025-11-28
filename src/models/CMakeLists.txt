add_library(models
    src/status.cpp
    include/models.h
    include/status.h
)

target_include_directories(models PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

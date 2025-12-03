cmake_minimum_required(VERSION 3.14)
project(MiniFileExplorer VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

# Dependency management
include(FetchContent)

# fmt
FetchContent_Declare(fmt GIT_REPOSITORY https://github.com/fmtlib/fmt.git GIT_TAG 12.1.0)
FetchContent_MakeAvailable(fmt)

# tabulate
FetchContent_Declare(tabulate GIT_REPOSITORY https://github.com/p-ranav/tabulate.git GIT_TAG v1.5)
FetchContent_MakeAvailable(tabulate)

# replxx
# FetchContent_Declare(replxx GIT_REPOSITORY https://github.com/AmokHuginnsson/replxx.git GIT_TAG release-0.0.4)
# FetchContent_MakeAvailable(replxx)

# replxx (fork version)
FetchContent_Declare(replxx GIT_REPOSITORY https://github.com/GerHobbelt/replxx.git GIT_TAG 97a5401)
FetchContent_MakeAvailable(replxx)

# CLI11
FetchContent_Declare(CLI11 GIT_REPOSITORY https://github.com/CLIUtils/CLI11 GIT_TAG v2.6.1)
FetchContent_MakeAvailable(CLI11)

add_subdirectory(src)

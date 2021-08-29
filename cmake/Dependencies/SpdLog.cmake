# FetchContent_MakeAvailable was added in CMake 3.14
cmake_minimum_required(VERSION 3.14)

include(FetchContent)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY  https://github.com/gabime/spdlog
    GIT_TAG         v1.9.2
)

set(SPDLOG_BUILD_SHARED OFF CACHE INTERNAL "")
set(SPDLOG_INSTALL ON CACHE INTERNAL "")

FetchContent_MakeAvailable(spdlog)

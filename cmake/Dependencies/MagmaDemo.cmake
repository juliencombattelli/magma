# FetchContent_MakeAvailable was added in CMake 3.14
cmake_minimum_required(VERSION 3.14)

include(FetchContent)

FetchContent_Declare(
    magma-demo
    GIT_REPOSITORY  git@github.com:juliencombattelli/magma-demo
    GIT_TAG         origin/main
)

FetchContent_MakeAvailable(magma-demo)

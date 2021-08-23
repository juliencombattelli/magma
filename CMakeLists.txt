cmake_minimum_required(VERSION 3.20)
project(Magma VERSION 1.0.0)

set(CMAKE_CXX_EXTENSIONS FALSE)

find_package(glfw3 3.3 REQUIRED CONFIG)
find_package(glm REQUIRED CONFIG)
find_package(Vulkan REQUIRED)

include(${PROJECT_SOURCE_DIR}/cmake/Modules/MagmaCompileShader.cmake)

configure_file (
    "${PROJECT_SOURCE_DIR}/include/magma/Version.hpp.in"
    "${PROJECT_BINARY_DIR}/include/magma/Version.hpp"
)

add_library(magma src/Instance.cpp src/utils/Name.cpp)
target_compile_features(magma PUBLIC cxx_std_20)
target_include_directories(magma
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(magma
    PUBLIC
        glm::glm glfw Vulkan::Vulkan
    PRIVATE
        ${CMAKE_DL_LIBS}
)

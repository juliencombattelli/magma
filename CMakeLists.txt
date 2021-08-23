cmake_minimum_required(VERSION 3.20)
project(Magma)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

find_package(glfw3 3.3 REQUIRED CONFIG)
find_package(glm REQUIRED CONFIG)
find_package(Vulkan REQUIRED)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/Modules/MagmaCompileShader.cmake)

add_library(magma src/Instance.cpp src/utils/Name.cpp)
target_include_directories(magma PUBLIC include)
target_link_libraries(magma PUBLIC glm::glm glfw Vulkan::Vulkan ${CMAKE_DL_LIBS})

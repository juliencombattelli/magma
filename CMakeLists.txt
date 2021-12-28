cmake_minimum_required(VERSION 3.20)
project(Magma
    VERSION 1.0.0
    DESCRIPTION "Simple rendering engine based on Vulkan and GLFW"
    HOMEPAGE_URL "https://github.com/juliencombattelli/magma"
)

################################################################################
### Options definition
################################################################################

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(MAGMA_IS_USED_STANDALONE ON)
else()
    set(MAGMA_IS_USED_STANDALONE OFF)
endif()

option(MAGMA_ENABLE_TESTING "Enable building the tests" ${MAGMA_IS_USED_STANDALONE})

option(MAGMA_ENABLE_DEMO "Fetch and build demo app" ${MAGMA_IS_USED_STANDALONE})

################################################################################
### Global CMake configuration
################################################################################

set(CMAKE_CXX_EXTENSIONS FALSE)

################################################################################
### Dependency handling
################################################################################

find_package(glfw3 3.3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

include(${PROJECT_SOURCE_DIR}/cmake/Dependencies/SpdLog.cmake)

################################################################################
### Include utility scripts
################################################################################

include(${PROJECT_SOURCE_DIR}/cmake/Scripts/ForceColoredOutput.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/Scripts/Sanitizers.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/Scripts/StaticAnalyzers.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/Scripts/Warnings.cmake)

include(${PROJECT_SOURCE_DIR}/cmake/Modules/MagmaCompileShader.cmake)

################################################################################
### Magma library definition
################################################################################

include(GNUInstallDirs)

configure_file(
    "${PROJECT_SOURCE_DIR}/include/magma/Version.hpp.in"
    "${PROJECT_BINARY_DIR}/include/magma/Version.hpp"
)

add_library(Magma src/Instance.cpp src/stdx/Name.cpp)
target_project_warnings(Magma)
target_enable_sanitizers(Magma)
target_compile_features(Magma PUBLIC cxx_std_20)
target_include_directories(Magma
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(Magma
    PUBLIC
        glm::glm glfw Vulkan::Vulkan spdlog::spdlog
    PRIVATE
        ${CMAKE_DL_LIBS}
)

add_library(Magma::Magma ALIAS Magma)

################################################################################
### Test execution
################################################################################

if(MAGMA_ENABLE_TESTING)
    enable_testing()
    # add_subdirectory(test)
endif()

################################################################################
### Demo build
################################################################################

if(MAGMA_ENABLE_DEMO)
    include(${PROJECT_SOURCE_DIR}/cmake/Dependencies/MagmaDemo.cmake)
endif()

################################################################################
### Package config file generation and installation
################################################################################

set(ConfigPackageLocation ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

# Generate CMake package config file
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/Config/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/Config/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${ConfigPackageLocation}
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/Config/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install the package config and version files
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/Config/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/Config/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${ConfigPackageLocation}
)

################################################################################
### Magma library installation
################################################################################

# Install the library and the headers, and generate a target file
install(
    TARGETS Magma
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/include/magma ${PROJECT_BINARY_DIR}/include/magma
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PATTERN "*.in" EXCLUDE
)

# Install the target file
install(
    EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${ConfigPackageLocation}
)

# Install the CMake scripts
install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/cmake/Modules
    DESTINATION ${ConfigPackageLocation}
)

################################################################################
### CPack config files generation
################################################################################

include(CPack)

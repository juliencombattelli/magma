# Distributed under the MIT License.  See accompanying file License.txt for details.

#[=======================================================================[.rst:
MagmaCompileShader
------------------

.. contents::

Overview
^^^^^^^^

This module enables the compilation of GLSL and HLSL shaders into SPIR-V, Vulkan's shader binary.
It also enables the generation of shader packs consisting of an achive of shaders binaries.

Commands
^^^^^^^^

Compiling a shader
""""""""""""""""""

.. command:: Magma_AddShader

  .. code-block:: cmake

    Magma_AddShader(<target> <shaderFile>)

  The ``Magma_AddShader()`` function adds a custom target called ``<target>`` compiling a SPIR-V
  binary from the GLSL of HLSL shader source file ``<shaderFile>``. The actual file name of the
  shader built will be ``<target>.spirv`` and will be generated in ``PROJECT_BINARY_DIR/shaders``
  folder.

  .. code-block:: cmake

    Magma_AddShader(myAppShaderVertex shaders/myAppShader.vert)

Generating a shader pack
""""""""""""""""""""""""

.. command:: Magma_AddShaderPack

  .. code-block:: cmake

    Magma_AddShaderPack(<target> <shaderFile>...)

  The ``Magma_AddShaderPack()`` function adds a custom target called ``<target>`` compiling the
  shader source files listed in the command invocation, and creating a shader pack archive from the
  shaders binaries. The actual file name of the shader pack will be ``<target>.shaderpack`` and will
  be generated in ``PROJECT_BINARY_DIR`` folder.

  .. code-block:: cmake

    Magma_AddShaderPack(myAppShaderPacl shaders/myAppShader.vert shaders/myAppShader.frag)

#]=======================================================================]

cmake_minimum_required(VERSION 3.20)

function(_Magma_FindVulkanGlslc)
    find_package(Vulkan REQUIRED)
    if(NOT TARGET Vulkan::glslc)
        message(FATAL_ERROR "glslc not found")
    endif()
endfunction()

function(Magma_AddShader TARGET SHADER)
    _Magma_FindVulkanGlslc()
    cmake_path(ABSOLUTE_PATH SHADER OUTPUT_VARIABLE SHADER_PATH)
    cmake_path(GET SHADER FILENAME SHADER_NAME)
    add_custom_command(OUTPUT shaders/${SHADER_NAME}.spirv
        COMMAND ${CMAKE_COMMAND} -E make_directory shaders
        COMMAND Vulkan::glslc "${SHADER_PATH}" -o "shaders/${SHADER_NAME}.spirv"
        DEPENDS "${SHADER_PATH}"
        COMMENT "Compiling shader ${SHADER}"
        VERBATIM
    )
    add_custom_target(${TARGET} ALL DEPENDS shaders/${SHADER_NAME}.spirv)
endfunction()

function(Magma_AddShaderPack TARGET)
endfunction()

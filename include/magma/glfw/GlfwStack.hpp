#pragma once

#include <magma/glfw/GlfwError.hpp>
#include <magma/stdx/Memory.hpp>

#include <GLFW/glfw3.h>

namespace magma {

/**
 * @brief RAII wrapper for GLFW setup and teardown operations
 */
class GlfwStack {
public:
    /**
     * @brief Default constructor using the default error callback
     */
    GlfwStack()
        : GlfwStack(defaultErrorCallback)
    {
    }

    /**
     * @brief Delegated constructor performing GLFW initialization
     */
    GlfwStack(GLFWerrorfun errorCallback)
    {
        glfwSetErrorCallback(errorCallback);
        auto initSuccessful = glfwInit();
        (void)initSuccessful; // If glfwInit fails, error_callback will be called.
                              // So this return value can be ignored.
    }

    /**
     * @brief Destructor performing GLFW cleanup
     */
    ~GlfwStack() noexcept
    {
        glfwTerminate();
    }

private:
    /**
     * @brief Default error callback throwing a GlfwError exception
     */
    static void defaultErrorCallback(int error, const char* description)
    {
        // TODO ensure throwing an exception from this callback is not undefined behavior
        throw GlfwError(error, description);
    }
};

using GlfwWindow = stdx::UniquePtrFactory<GLFWwindow, glfwCreateWindow, glfwDestroyWindow>;

} // namespace magma

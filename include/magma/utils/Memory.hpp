#pragma once

#include <memory>

namespace utils::memory {

/**
 * @brief Factory class generating unique_ptr from allocator/deleter functions.
 *
 * This factory is useful to wrap pointers allocated by C APIs, providing RAII capabilities while
 * having a very small (if not none) overhead.
 *
 * @example
 *
 * // C APIs from GLFW3 allocating and deleting a GLFWwindow object
 * GLFWwindow *glfwCreateWindow(
 *      int width, int height, const char *title, GLFWmonitor *monitor, GLFWwindow *share);
 * void glfwDestroyWindow(GLFWwindow *window);
 *
 * // Convenience alias to a GLFW3 window unique_ptr
 * using GlfwWindow = UniquePtrFactory<GLFWwindow, &glfwCreateWindow, &glfwDestroyWindow>;
 *
 * // Allocation of the GLFWwindow object wrapped inside the window unique_ptr
 * // The deleter function is automatically called when window goes out of scope
 * GlfwWindow::Ptr window = GlfwWindow::create(800, 600, "test", nullptr, nullptr);
 *
 * // Usage of the window unique_ptr with a GLFW3 API
 * glfwMakeContextCurrent(window.get());
 */
template<typename T, auto allocator, auto deleter>
class UniquePtrFactory {
    struct Deleter;

public:
    using Ptr = std::unique_ptr<T, Deleter>;

    template<typename... TArgs>
    [[nodiscard]] static Ptr create(TArgs&&... args)
    {
        return Ptr { allocator(std::forward<TArgs>(args)...) };
    }

private:
    struct Deleter {
        template<typename... TArgs>
        auto operator()(TArgs&&... args) const
        {
            return deleter(std::forward<TArgs>(args)...);
        }
    };
};

} // namespace utils::memory

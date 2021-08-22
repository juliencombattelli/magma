#pragma once

#include <stdexcept>

/**
 * @brief Exception type for GLFW related errors
 */
class GlfwError : public std::runtime_error {
public:
    GlfwError(int error, const char* description)
        : std::runtime_error(std::string("from GLFW: ") + description)
        , error_{ error } {}

    [[nodiscard]] int error_code() const noexcept { return error_; }

private:
    const int error_;
};

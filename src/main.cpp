#include <iostream>

#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "glm/glm.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "chip8.h"

chip8 myChip8;


int main()
{
    if (!glfwInit())
        return 1;
    const char *glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac

    GLFWwindow *window =
        glfwCreateWindow(1280, 720, "CHIP8 Emulator", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    //Chip8 initializatio

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}